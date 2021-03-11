/*
 * gpio_adatper.h
 *
 * gpio driver adapter of linux
 *
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include "device_resource_if.h"
#include "gpio_core.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_mutex.h"

#define HDF_LOG_TAG linux_gpio_adapter

static int32_t LinuxGpioWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
    (void)cntlr;
    gpio_set_value(gpio, val);
    return HDF_SUCCESS;
}

static int32_t LinuxGpioRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    (void)cntlr;
    if (val != NULL) {
        *val = (gpio_get_value(gpio) == 0) ?
            GPIO_VAL_LOW : GPIO_VAL_HIGH;
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: val is NULL!\n", __func__);
    return HDF_ERR_BSP_PLT_API_ERR;
}

static int32_t LinuxGpioSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    int32_t ret;
    int val;

    switch (dir) {
        case GPIO_DIR_IN:
            ret = gpio_direction_input(gpio);
            break;
        case GPIO_DIR_OUT:
            val = gpio_get_value(gpio);
            if (val < 0) {
                ret = HDF_ERR_BSP_PLT_API_ERR;
            } else {
                ret = gpio_direction_output(gpio, val);
            }
            break;
        default:
            HDF_LOGE("%s: invalid dir:%d\n", __func__, dir);
            ret = HDF_ERR_INVALID_PARAM;
            break;
    }
    return ret;
}

static int32_t LinuxGpioGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir)
{
    int dirGet;

    dirGet = gpiod_get_direction(gpio_to_desc(gpio));
    if (dirGet < 0) {
        return HDF_ERR_BSP_PLT_API_ERR;
    }
    *dir = (dirGet == GPIOF_DIR_IN) ? GPIO_DIR_IN : GPIO_DIR_OUT;
    return HDF_SUCCESS;
}

static irqreturn_t LinuxGpioIrqBridge(int irq, void *data)
{
    int gpio = (int)(uintptr_t)data;
    GpioCntlrIrqCallback(GpioGetCntlr(gpio), gpio);
    return IRQ_HANDLED;
}

static int32_t LinuxGpioSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode,
    GpioIrqFunc func, void *arg)
{
    int ret, irq;
    unsigned long flags = 0;

    irq = gpio_to_irq(gpio);
    if (irq < 0) {
        HDF_LOGE("%s: gpio(%u) to irq fail:%d", __func__, gpio, irq);
        return HDF_ERR_BSP_PLT_API_ERR;
    }
    flags |= (mode & GPIO_IRQ_TRIGGER_RISING) == 0 ? 0 : IRQF_TRIGGER_RISING;
    flags |= (mode & GPIO_IRQ_TRIGGER_FALLING) == 0 ? 0 : IRQF_TRIGGER_FALLING;
    flags |= (mode & GPIO_IRQ_TRIGGER_HIGH) == 0 ? 0 : IRQF_TRIGGER_HIGH;
    flags |= (mode & GPIO_IRQ_TRIGGER_LOW) == 0 ? 0 : IRQF_TRIGGER_LOW;
    if ((mode & GPIO_IRQ_USING_THREAD) == 0) {
        ret = request_irq(irq, LinuxGpioIrqBridge, flags,
            "LinuxIrqBridge", (void *)(uintptr_t)gpio);
    } else {
        flags |= IRQF_ONESHOT; // one shot needed for linux thread irq
        ret = request_threaded_irq(irq, NULL, LinuxGpioIrqBridge, flags,
            "LinuxIrqBridge", (void *)(uintptr_t)gpio);
    }
    if (ret == 0) {
        disable_irq(irq); // disable on set
    }
    return (ret == 0) ? HDF_SUCCESS : HDF_ERR_BSP_PLT_API_ERR;
}

static int32_t LinuxGpioUnsetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    int irq;

    irq = gpio_to_irq(gpio);
    if (irq < 0) {
        HDF_LOGE("%s: gpio(%u) to irq fail:%d", __func__, gpio, irq);
        return HDF_ERR_BSP_PLT_API_ERR;
    }
    HDF_LOGE("%s: gona free irq:%d\n", __func__, irq);
    free_irq(irq, (void *)(uintptr_t)gpio);
    return HDF_SUCCESS;
}

static inline int32_t LinuxGpioEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    int irq;

    irq = gpio_to_irq(gpio);
    if (irq < 0) {
        HDF_LOGE("%s: gpio(%u) to irq fail:%d", __func__, gpio, irq);
        return HDF_ERR_BSP_PLT_API_ERR;
    }
    enable_irq(irq);
    return HDF_SUCCESS;
}

static inline int32_t LinuxGpioDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    int irq;

    irq = gpio_to_irq(gpio);
    if (irq < 0) {
        HDF_LOGE("%s: gpio(%u) to irq fail:%d", __func__, gpio, irq);
        return HDF_ERR_BSP_PLT_API_ERR;
    }
    disable_irq_nosync(irq); // nosync default in case used in own irq
    return HDF_SUCCESS;
}

static struct GpioMethod g_method = {
    .write = LinuxGpioWrite,
    .read = LinuxGpioRead,
    .setDir = LinuxGpioSetDir,
    .getDir = LinuxGpioGetDir,
    .setIrq = LinuxGpioSetIrq,
    .unsetIrq = LinuxGpioUnsetIrq,
    .enableIrq = LinuxGpioEnableIrq,
    .disableIrq = LinuxGpioDisableIrq,
};

static int32_t LinuxGpioBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static int32_t LinuxGpioInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct GpioCntlr *cntlr = NULL;

    HDF_LOGI("%s: Enter", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: Fail, device is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct GpioCntlr *)OsalMemCalloc(sizeof(*cntlr));
    if (cntlr == NULL) {
        HDF_LOGE("%s: malloc cntlr fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    cntlr->device = device;
    cntlr->ops = &g_method;
    cntlr->count = GPIO_NUM_DEFAULT;
    ret = GpioCntlrAdd(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: add gpio controller fail:%d!", __func__, ret);
        OsalMemFree(cntlr);
        return ret;
    }
    HDF_LOGI("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));
    return HDF_SUCCESS;
}

static void LinuxGpioRelease(struct HdfDeviceObject *device)
{
    struct GpioCntlr *cntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null!", __func__);
        return;
    }

    cntlr = GpioCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: no service binded!", __func__);
        return;
    }
    OsalMemFree(cntlr);
}

struct HdfDriverEntry g_gpioLinuxDriverEntry = {
    .moduleVersion = 1,
    .Bind = LinuxGpioBind,
    .Init = LinuxGpioInit,
    .Release = LinuxGpioRelease,
    .moduleName = "linux_gpio_adapter",
};
HDF_INIT(g_gpioLinuxDriverEntry);
