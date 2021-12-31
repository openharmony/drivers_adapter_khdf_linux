/*
 * regulator driver adapter of linux
 *
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
#include "regulator_adapter.h"
#include "regulator_adapter_consumer.h"
#include "regulator_core.h"
#include <linux/regulator/consumer.h>
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG regulator_linux_adapter
static int32_t LinuxRegulatorOpen(struct RegulatorNode *node)
{
    if (node == NULL || node->priv == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg == NULL) {
        const char *devname = dev_name(info->dev);
        if ((devname == NULL) || (strcmp(devname, info->devName) != 0)) {
            HDF_LOGE("%s:dev info error [%s][%s]!", __func__, devname, info->devName);
            return HDF_FAILURE;
        }

        info->adapterReg = regulator_get(info->dev, info->supplyName);
        if (IS_ERR(info->adapterReg)) {
            HDF_LOGE("%s: regulator_get [%s][%s] ERROR!", __func__, devname, info->supplyName);
            info->adapterReg = NULL;
            return HDF_FAILURE;
        }
        if (info->adapterReg == NULL) {
            HDF_LOGE("%s: regulator_get [%s][%s]!", __func__, devname, info->supplyName);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}
static int32_t LinuxRegulatorClose(struct RegulatorNode *node)
{
    if (node == NULL || node->priv == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg != NULL) {
        if (regulator_disable(info->adapterReg) != HDF_SUCCESS) {
            HDF_LOGE("%s:regulator_disable[%s][%s] FAIL", __func__, node->regulatorInfo.name, info->supplyName);
        }
        regulator_put(info->adapterReg);
        info->adapterReg = NULL;
    }
    return HDF_SUCCESS;
}
static int32_t LinuxRegulatorRemove(struct RegulatorNode *node)
{
    if (node == NULL || node->priv == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    HDF_LOGI("%s:regulator [%s][%s] release!", __func__, info->devName, info->supplyName);
    if (LinuxRegulatorClose(node) != HDF_SUCCESS) {
        HDF_LOGE("%s:LinuxRegulatorClose fail[%s][%s]!", __func__, info->devName, info->supplyName);
    }

    OsalMemFree(info);
    node->priv = NULL;
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorEnable(struct RegulatorNode *node)
{
    if (node == NULL || node->priv == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg == NULL) {
        HDF_LOGE("%s adapterReg null, please open dev", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    int ret = regulator_enable(info->adapterReg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:[%s][%s][%d] FAIL", __func__, node->regulatorInfo.name, info->supplyName, ret);
        return HDF_FAILURE;
    }
    if (regulator_is_enabled(info->adapterReg) > 0) {
        node->regulatorInfo.status = REGULATOR_STATUS_ON;
    } else {
        node->regulatorInfo.status = REGULATOR_STATUS_OFF;
    }
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorDisable(struct RegulatorNode *node)
{
    if (node == NULL || node->priv == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg == NULL) {
        HDF_LOGE("%s adapterReg null, please open dev", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (regulator_disable(info->adapterReg) != HDF_SUCCESS) {
        HDF_LOGE("%s:[%s][%s] FAIL", __func__, node->regulatorInfo.name, info->supplyName);
        return HDF_FAILURE;
    }
    // node maybe alwayson
    if (regulator_is_enabled(info->adapterReg) > 0) {
        node->regulatorInfo.status = REGULATOR_STATUS_ON;
    } else {
        node->regulatorInfo.status = REGULATOR_STATUS_OFF;
    }
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorForceDisable(struct RegulatorNode *node)
{
    if (node == NULL || node->priv == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg == NULL) {
        HDF_LOGE("%s adapterReg null, please open dev", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (regulator_force_disable(info->adapterReg) != HDF_SUCCESS) {
        HDF_LOGE("regulator_force_disable[%s][%s] FAIL", node->regulatorInfo.name, info->supplyName);
        return HDF_FAILURE;
    }
    node->regulatorInfo.status = REGULATOR_STATUS_OFF;
    HDF_LOGI("%s:[%s][%s] success!", __func__, info->devName, info->supplyName);
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorSetVoltage(struct RegulatorNode *node, uint32_t minUv, uint32_t maxUv)
{
    if (node == NULL || node->priv == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg == NULL) {
        HDF_LOGE("%s adapterReg null, please open dev", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (regulator_set_voltage(info->adapterReg, minUv, maxUv) != HDF_SUCCESS) {
        HDF_LOGE("%s: [%s][%s] FAIL", __func__, node->regulatorInfo.name, info->supplyName);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorGetVoltage(struct RegulatorNode *node, uint32_t *voltage)
{
    if (node == NULL || node->priv == NULL || voltage == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg == NULL) {
        HDF_LOGE("%s adapterReg null, please open dev", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    int ret = regulator_get_voltage(info->adapterReg);
    if (ret < 0) {
        HDF_LOGE("%s: [%s] FAIL", __func__, node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    *voltage = ret;
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorSetCurrent(struct RegulatorNode *node, uint32_t minUa, uint32_t maxUa)
{
    if (node == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg == NULL) {
        HDF_LOGE("%s adapterReg null, please open dev", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (regulator_set_current_limit(info->adapterReg, minUa, maxUa) != HDF_SUCCESS) {
        HDF_LOGE("%s: [%s][%s] FAIL", __func__, node->regulatorInfo.name, info->supplyName);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorGetCurrent(struct RegulatorNode *node, uint32_t *regCurrent)
{
    if (node == NULL || regCurrent == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg == NULL) {
        HDF_LOGE("%s adapterReg null, please open dev", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    int ret = regulator_get_current_limit(info->adapterReg);
    if (ret < 0) {
        HDF_LOGE("%s: [%s] FAIL", __func__, node->regulatorInfo.name);
        return HDF_FAILURE;
    }

    *regCurrent = ret;
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorGetStatus(struct RegulatorNode *node, uint32_t *status)
{
    if (node == NULL || status == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    struct LinuxRegulatorInfo *info = (struct LinuxRegulatorInfo *)node->priv;
    if (info->adapterReg == NULL) {
        HDF_LOGE("%s adapterReg null, please open dev", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (regulator_is_enabled(info->adapterReg) > 0) {
        *status = REGULATOR_STATUS_ON;
    } else {
        *status = REGULATOR_STATUS_OFF;
    }
    return HDF_SUCCESS;
}

static struct RegulatorMethod g_method = {
    .open = LinuxRegulatorOpen,
    .close = LinuxRegulatorClose,
    .release = LinuxRegulatorRemove,
    .enable = LinuxRegulatorEnable,
    .disable = LinuxRegulatorDisable,
    .forceDisable = LinuxRegulatorForceDisable,
    .setVoltage = LinuxRegulatorSetVoltage,
    .getVoltage = LinuxRegulatorGetVoltage,
    .setCurrent = LinuxRegulatorSetCurrent,
    .getCurrent = LinuxRegulatorGetCurrent,
    .getStatus = LinuxRegulatorGetStatus,
};

static struct device *g_consumer_dev;
int32_t LinuxRegulatorSetConsumerDev(struct device *dev)
{
    if (dev == NULL) {
        HDF_LOGE("%s: node null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    g_consumer_dev = dev;
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorReadHcs(struct RegulatorNode *regNode, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetString(node, "name", &(regNode->regulatorInfo.name), "ERROR");
    if ((ret != HDF_SUCCESS) || (regNode->regulatorInfo.name == NULL)) {
        HDF_LOGE("%s: read name fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s:name[%s]", __func__, regNode->regulatorInfo.name);

    ret = drsOps->GetUint8(node, "mode", &regNode->regulatorInfo.constraints.mode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read mode fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "minUv", &regNode->regulatorInfo.constraints.minUv, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read minUv fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "maxUv", &regNode->regulatorInfo.constraints.maxUv, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read maxUv fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "minUa", &regNode->regulatorInfo.constraints.minUa, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read minUa fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "maxUa", &regNode->regulatorInfo.constraints.maxUa, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read maxUa fail!", __func__);
        return HDF_FAILURE;
    }

    regNode->regulatorInfo.parentName = NULL;
    regNode->regulatorInfo.status = REGULATOR_STATUS_OFF;

    HDF_LOGI("%s: name[%s][%d]--[%d][%d]--[%d][%d]", __func__,
        regNode->regulatorInfo.name, regNode->regulatorInfo.constraints.mode, 
        regNode->regulatorInfo.constraints.minUv, regNode->regulatorInfo.constraints.maxUv, 
        regNode->regulatorInfo.constraints.minUa, regNode->regulatorInfo.constraints.maxUa);

    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorReadAdapterHcs(struct LinuxRegulatorInfo *info, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (info == NULL || drsOps == NULL || drsOps->GetString == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetString(node, "devName", &(info->devName), "ERROR");
    if ((ret != HDF_SUCCESS) || (info->devName == NULL)) {
        HDF_LOGE("%s: read devName fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s:devName[%s]", __func__, info->devName);

    ret = drsOps->GetString(node, "supplyName", &(info->supplyName), "ERROR");
    if ((ret != HDF_SUCCESS) || (info->supplyName == NULL)) {
        HDF_LOGE("%s: read supplyName fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s:supplyName[%s]", __func__, info->supplyName);

    info->dev = g_consumer_dev;
    return HDF_SUCCESS;
}

static int32_t LinuxRegulatorParseAndInit(struct HdfDeviceObject *device, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct RegulatorNode *regNode = NULL;
    struct LinuxRegulatorInfo *info = NULL;
    (void)device;

    regNode = (struct RegulatorNode *)OsalMemCalloc(sizeof(*regNode));
    if (regNode == NULL) {
        HDF_LOGE("%s: malloc regNode fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    info = (struct LinuxRegulatorInfo *)OsalMemCalloc(sizeof(*info));
    if (info == NULL) {
        HDF_LOGE("%s: malloc info fail!", __func__);
        OsalMemFree(regNode);
        regNode = NULL;
        return HDF_ERR_MALLOC_FAIL;
    }

    do {
        ret = LinuxRegulatorReadHcs(regNode, node);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read drs fail! ret:%d", __func__, ret);
            break;
        }

        ret = LinuxRegulatorReadAdapterHcs(info, node);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: LinuxRegulatorReadAdapterHcs fail! ret:%d", __func__, ret);
            break;
        }

        HDF_LOGI("%s: name[%s][%d]--[%d][%d]--[%d][%d]--[%s][%s]", __func__,
            regNode->regulatorInfo.name, regNode->regulatorInfo.constraints.mode,
            regNode->regulatorInfo.constraints.minUv, regNode->regulatorInfo.constraints.maxUv,
            regNode->regulatorInfo.constraints.minUa, regNode->regulatorInfo.constraints.maxUa,
            info->devName, info->supplyName);

        regNode->priv = (void *)info;
        regNode->ops = &g_method;

        ret = RegulatorNodeAdd(regNode);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: add regulator controller fail:%d!", __func__, ret);
            break;
        }
    } while (0);

    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail [%d]!", __func__, ret);
        OsalMemFree(regNode);
        regNode = NULL;
        OsalMemFree(info);
        info = NULL;
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

/* get all linux regulator, then add to hdf */
static int32_t LinuxRegulatorInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    const struct DeviceResourceNode *childNode = NULL;
    RegulatorAdapterConsumerInit();

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        ret = LinuxRegulatorParseAndInit(device, childNode);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:LinuxRegulatorParseAndInit fail", __func__);
            return HDF_FAILURE;
        }
    }
    return ret;
}
static int32_t LinuxRegulatorParseAndRelease(struct HdfDeviceObject *device, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct LinuxRegulatorInfo *info = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    (void)device;

    HDF_LOGI("LinuxRegulatorParseAndRelease");

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    const char *name;
    ret = drsOps->GetString(node, "name", &(name), "ERROR");
    if ((ret != HDF_SUCCESS) || (name == NULL)) {
        HDF_LOGE("%s: read name fail!", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGI("LinuxRegulatorParseAndRelease: name[%s]", name);

    ret = RegulatorNodeRemove(name);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LinuxRegulatorRelease fail:%d!", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("LinuxRegulatorParseAndRelease: name[%s] success", name);
    return HDF_SUCCESS;
}

static void LinuxRegulatorRelease(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: enter", __func__);
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return;
    }
    const struct DeviceResourceNode *childNode = NULL;

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        int ret = LinuxRegulatorParseAndRelease(device, childNode);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:LinuxRegulatorParseAndInit fail", __func__);
        }
    }
}

struct HdfDriverEntry g_regulatorLinuxDriverEntry = {
    .moduleVersion = 1,
    .Bind = LinuxRegulatorBind,
    .Init = LinuxRegulatorInit,
    .Release = LinuxRegulatorRelease,
    .moduleName = "linux_regulator_adapter",
};
HDF_INIT(g_regulatorLinuxDriverEntry);
