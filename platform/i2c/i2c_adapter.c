/*
 * i2c_adatper.h
 *
 * i2c driver adapter of linux
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

#include <linux/i2c.h>
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "i2c_core.h"
#include "osal_mem.h"

#define HDF_LOG_TAG i2c_linux_adapter

static struct i2c_msg *CreateLinuxI2cMsgs(struct I2cMsg *msgs, int16_t count)
{
    int16_t i;
    struct i2c_msg *linuxMsgs = NULL;

    linuxMsgs = (struct i2c_msg *)OsalMemCalloc(sizeof(*linuxMsgs) * count);
    if (linuxMsgs == NULL) {
        HDF_LOGE("%s: malloc linux msgs fail!", __func__);
        return NULL;
    }

    for (i = 0; i < count; i++) {
        linuxMsgs[i].addr = msgs[i].addr;
        linuxMsgs[i].buf = msgs[i].buf;
        linuxMsgs[i].len = msgs[i].len;
        linuxMsgs[i].flags = msgs[i].flags;
    }
    return linuxMsgs;
}

static inline void FreeLinxI2cMsgs(struct i2c_msg *msgs, int16_t count)
{
    OsalMemFree(msgs);
    (void)count;
}

static int32_t LinuxI2cTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count)
{
    int32_t ret;
    struct i2c_msg *linuxMsgs = NULL;

    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: cntlr or priv is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (msgs == NULL || count <= 0) {
        HDF_LOGE("%s: err parms! count:%d", __func__, count);
        return HDF_ERR_INVALID_PARAM;
    }

    linuxMsgs = CreateLinuxI2cMsgs(msgs, count);
    if (linuxMsgs == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = i2c_transfer((struct i2c_adapter *)cntlr->priv, linuxMsgs, count);
    FreeLinxI2cMsgs(linuxMsgs, count);
    return ret;
}

static struct I2cMethod g_method = {
    .transfer = LinuxI2cTransfer,
};

static int32_t LinuxI2cReadDrs(struct I2cCntlr *cntlr, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint16(node, "bus", (uint16_t *)&cntlr->busId, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus fail!", __func__);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t LinuxI2cBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static int32_t LinuxI2cParseAndInit(struct HdfDeviceObject *device, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct I2cCntlr *cntlr = NULL;
    struct i2c_adapter *adapter = NULL;

    HDF_LOGI("%s: Enter", __func__);
    (void)device;

    cntlr = (struct I2cCntlr *)OsalMemCalloc(sizeof(*cntlr));
    if (cntlr == NULL) {
        HDF_LOGE("%s: malloc cntlr fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = LinuxI2cReadDrs(cntlr, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs fail! ret:%d", __func__, ret);
        goto __ERR__;
    }

    adapter = i2c_get_adapter(cntlr->busId);
    if (adapter == NULL) {
        HDF_LOGE("%s: i2c_get_adapter fail!", __func__);
        goto __ERR__;
    }
    cntlr->priv = adapter;
    cntlr->ops = &g_method;
    ret = I2cCntlrAdd(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: add i2c controller fail:%d!", __func__, ret);
        goto __ERR__;
    }
    return HDF_SUCCESS;
 __ERR__:
    OsalMemFree(cntlr);
    return ret;
}

static int32_t LinuxI2cInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    const struct DeviceResourceNode *childNode = NULL;

    HDF_LOGI("%s: Enter", __func__);
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = HDF_SUCCESS;
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        ret = LinuxI2cParseAndInit(device, childNode);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }

    return ret;
}

static void LinuxI2cRemoveByNode(const struct DeviceResourceNode *node)
{
    int32_t ret;
    int16_t bus;
    struct I2cCntlr *cntlr = NULL;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return;
    }

    ret = drsOps->GetUint16(node, "bus", (uint16_t *)&bus, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus fail!", __func__);
        return;
    }

    cntlr = I2cCntlrGet(bus);
    if (cntlr != NULL && cntlr->priv == node) {
        I2cCntlrPut(cntlr);
        I2cCntlrRemove(cntlr);
        OsalMemFree(cntlr);
    }
    return;
}

static void LinuxI2cRelease(struct HdfDeviceObject *device)
{
    const struct DeviceResourceNode *childNode = NULL;

    HDF_LOGI("%s: enter", __func__);
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        LinuxI2cRemoveByNode(childNode);
    }
}

struct HdfDriverEntry g_i2cLinuxDriverEntry = {
    .moduleVersion = 1,
    .Bind = LinuxI2cBind,
    .Init = LinuxI2cInit,
    .Release = LinuxI2cRelease,
    .moduleName = "linux_i2c_adapter",
};
HDF_INIT(g_i2cLinuxDriverEntry);
