/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "regulator_adapter_consumer.h"
#include "regulator_adapter.h"
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/consumer.h>
#include "hdf_log.h"

#define HDF_LOG_TAG regulator_linux_adapter

static void RegulatorAdapterConsumerDevRelease(struct device *dev)
{
}

static struct platform_device RegulatorAdapterConsumerPlatformDevice = {
    .name = "regulator_adapter_consumer01",
    .id = -1,
    .dev = {
        .release = RegulatorAdapterConsumerDevRelease,
    }
};

struct platform_device *g_regulatorAdapterDev;
static int RegulatorAdapterConsumerPlatformProbe(struct platform_device *platform_dev)
{
    if (platform_dev != NULL) {
        g_regulatorAdapterDev = platform_dev;
        LinuxRegulatorSetConsumerDev(&platform_dev->dev);
        HDF_LOGI("%s success", __func__);
        return HDF_SUCCESS;
    }

    HDF_LOGE("%s: fail", __func__);
    return HDF_FAILURE;
}

static int RegulatorAdapterConsumerPlatformRemove(struct platform_device *platform_dev)
{
    if (platform_dev == NULL) {
        HDF_LOGE("%s: fail", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

static struct platform_driver RegulatorAdapterConsumerPlatformDriver = {
    .driver = {
        .name = "regulator_adapter_consumer01",
        .owner = THIS_MODULE,
    },
    .probe = RegulatorAdapterConsumerPlatformProbe,
    .remove = RegulatorAdapterConsumerPlatformRemove,
};

int RegulatorAdapterConsumerInit(void)
{
    int ret;

    ret = platform_device_register(&RegulatorAdapterConsumerPlatformDevice);
    if (ret != 0) {
        HDF_LOGE("%s: fail", __func__);
        return ret;
    }

    ret = platform_driver_register(&RegulatorAdapterConsumerPlatformDriver);
    return ret;
}

int __init RegulatorAdapterConsumerModuleInit(void)
{
    int ret;

    ret = platform_device_register(&RegulatorAdapterConsumerPlatformDevice);
    if (ret != 0) {
        HDF_LOGE("%s: fail", __func__);
        return ret;
    }
    ret = platform_driver_register(&RegulatorAdapterConsumerPlatformDriver);
    return ret;
}
void __exit RegulatorAdapterConsumerExit(void)
{
    platform_device_unregister(&RegulatorAdapterConsumerPlatformDevice);
    platform_driver_unregister(&RegulatorAdapterConsumerPlatformDriver);
}

MODULE_DESCRIPTION("Regulator Adapter Consumer Platform Device");
MODULE_LICENSE("GPL");
