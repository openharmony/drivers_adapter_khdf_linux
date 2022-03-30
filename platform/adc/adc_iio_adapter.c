/*
 * adc driver adapter of linux
 *
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <linux/fs.h>
#include <linux/kernel.h>
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "adc_core.h"

#define ADC_STRING_VALUE_LEN 15
#define ADC_DRIVER_PATHNAME_LEN 128
#define INT_MAX_VALUE 2147483647
#define FILE_MODE 0600
#define DECIMAL_SHIFT_LEFT 10

static char g_driverPathname[ADC_DRIVER_PATHNAME_LEN] = {0};
struct AdcIioDevice {
    struct AdcDevice device;
    volatile unsigned char *regBase;
    volatile unsigned char *pinCtrlBase;
    uint32_t regBasePhy;
    uint32_t regSize;
    uint32_t deviceNum;
    uint32_t dataWidth;
    uint32_t validChannel;
    uint32_t scanMode;
    uint32_t delta;
    uint32_t deglitch;
    uint32_t glitchSample;
    uint32_t rate;
};

static int32_t AdcIioRead(struct AdcDevice *device, uint32_t channel, uint32_t *val)
{
    int ret;
    loff_t pos = 0;
    struct file *fp = NULL;
    unsigned char strValue[ADC_STRING_VALUE_LEN] = {0};

    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: Illegal object", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (val == NULL) {
        HDF_LOGE("%s: Illegal parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    fp = (struct file *)device->priv;
    ret = kernel_read(fp, strValue, ADC_STRING_VALUE_LEN, &pos);
    if (ret < 0) {
        HDF_LOGE("%s: kernel_read fail %d", __func__, ret);
        return HDF_PLT_ERR_OS_API;
    }
    *val = simple_strtoul(strValue, NULL, 0);

    return HDF_SUCCESS;
}

static int32_t AdcIioStop(struct AdcDevice *device)
{
    int ret;
    struct file *fp = NULL;

    ret = HDF_FAILURE;
    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: Illegal object", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    fp = (struct file *)device->priv;
    if (!IS_ERR(fp)) {
        ret = filp_close(fp, NULL);
    }

    return ret;
}

static int32_t AdcIioStart(struct AdcDevice *device)
{
    struct file *fp = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    fp = filp_open(g_driverPathname, O_RDWR | O_NOCTTY | O_NDELAY, FILE_MODE);
    if (IS_ERR(fp)) {
        HDF_LOGE("filp open fail");
        return HDF_PLT_ERR_OS_API;
    }
    device->priv = fp;

    return HDF_SUCCESS;
}
static const struct AdcMethod g_method = {
    .read = AdcIioRead,
    .stop = AdcIioStop,
    .start = AdcIioStart,
};

static int32_t AdcIioReadDrs(struct AdcIioDevice *adcDevice, const struct DeviceResourceNode *node)
{
    int32_t ret;
    const char *drName = NULL;
    int32_t drNameLen;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL || drsOps->GetString == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = drsOps->GetString(node, "driver_name", &drName, "/sys/bus/iio/devices/iio:device0/in_voltage0-voltage0_raw");
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read driver_name failed", __func__);
        return ret;
    }
    drNameLen = strlen(drName);
    if (drNameLen > (ADC_DRIVER_PATHNAME_LEN - 1)) {
        HDF_LOGE("%s: Illegal length of drName", __func__);
        return HDF_FAILURE;
    } 
    ret = memcpy_s(g_driverPathname, ADC_DRIVER_PATHNAME_LEN, drName, drNameLen);
    if (ret != 0) {
        HDF_LOGE("%s: memcpy drName fail", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "deviceNum", &adcDevice->deviceNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read deviceNum failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "scanMode", &adcDevice->scanMode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read scanMode failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "rate", &adcDevice->rate, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read rate failed", __func__);
        return ret;
    }
    
    return HDF_SUCCESS;
}

static int32_t LinuxAdcInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct AdcIioDevice *adcDevice = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (device->property == NULL) {
        HDF_LOGE("%s: property of device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    adcDevice = (struct AdcIioDevice *)OsalMemCalloc(sizeof(*adcDevice));
    if (adcDevice == NULL) {
        HDF_LOGE("%s: alloc adcDevice failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    device->priv = adcDevice;
    ret = AdcIioReadDrs(adcDevice, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs failed:%d", __func__, ret);
        OsalMemFree(adcDevice);
        return ret;
    }

    adcDevice->device.devNum = adcDevice->deviceNum;
    adcDevice->device.ops = &g_method;
    ret = AdcDeviceAdd(&adcDevice->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: add adc device:%u failed", __func__, adcDevice->deviceNum);
        OsalMemFree(adcDevice);
        return ret;
    }

    return HDF_SUCCESS;
}

static void LinuxAdcRelease(struct HdfDeviceObject *device)
{
    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: Illegal parameter", __func__);
        return;
    }
    AdcDeviceRemove(device->priv);
    OsalMemFree(device->priv);
}

struct HdfDriverEntry g_adcLinuxDriverEntry = {
    .moduleVersion = 1,
    .Bind = NULL,
    .Init = LinuxAdcInit,
    .Release = LinuxAdcRelease,
    .moduleName = "linux_adc_adapter",
};
HDF_INIT(g_adcLinuxDriverEntry);
