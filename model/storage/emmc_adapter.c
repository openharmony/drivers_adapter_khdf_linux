/*
 * emmc_adapter.c
 *
 * linux emmc driver implement.
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

#include <securec.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include "device_resource_if.h"
#include "hdf_log.h"
#include "emmc_if.h"
#include "mmc_corex.h"
#include "mmc_emmc.h"

#define HDF_LOG_TAG emmc_adapter_c

struct mmc_host *GetMmcHost(int32_t slot);

int32_t LinuxEmmcGetCid(struct EmmcDevice *dev, uint8_t *cid, uint32_t size)
{
    struct mmc_host *mmcHost = NULL;
    struct MmcCntlr *cntlr = NULL;

    if (dev == NULL || dev->mmc.cntlr == NULL) {
        HDF_LOGE("LinuxEmmcGetCid: dev or cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cid == NULL || size < EMMC_CID_LEN) {
        HDF_LOGE("LinuxEmmcGetCid: cid is null or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = dev->mmc.cntlr;
    mmcHost = (struct mmc_host *)cntlr->priv;
    if (mmcHost == NULL) {
        HDF_LOGE("LinuxEmmcGetCid: mmcHost is NULL!");
        return HDF_ERR_NOT_SUPPORT;
    }
    if (mmcHost->card == NULL) {
        HDF_LOGE("LinuxEmmcGetCid: card is null.");
        return HDF_ERR_NOT_SUPPORT;
    }
    if (memcpy_s(cid, sizeof(uint8_t) * size, (uint8_t *)(mmcHost->card->raw_cid),
        sizeof(mmcHost->card->raw_cid)) != EOK) {
        HDF_LOGE("LinuxEmmcGetCid: memcpy_s fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct EmmcDeviceOps g_emmcMethod = {
    .getCid = LinuxEmmcGetCid,
};

static void LinuxEmmcDeleteCntlr(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }

    if (cntlr->curDev != NULL) {
        MmcDeviceRemove(cntlr->curDev);
        OsalMemFree(cntlr->curDev);
    }
    MmcCntlrRemove(cntlr);
    OsalMemFree(cntlr);
}

static int32_t LinuxEmmcCntlrParse(struct MmcCntlr *cntlr, struct HdfDeviceObject *obj)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (obj == NULL || cntlr == NULL) {
        HDF_LOGE("LinuxEmmcCntlrParse: input para is NULL.");
        return HDF_FAILURE;
    }

    node = obj->property;
    if (node == NULL) {
        HDF_LOGE("LinuxEmmcCntlrParse: drs node is NULL.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint16 == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("LinuxEmmcCntlrParse: invalid drs ops fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint16(node, "hostId", &(cntlr->index), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxEmmcCntlrParse: read hostIndex fail!");
        return ret;
    }
    ret = drsOps->GetUint32(node, "devType", &(cntlr->devType), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxEmmcCntlrParse: read devType fail!");
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t LinuxEmmcBind(struct HdfDeviceObject *obj)
{
    struct MmcCntlr *cntlr = NULL;
    int32_t ret;

    if (obj == NULL) {
        HDF_LOGE("LinuxEmmcBind: Fail, obj is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct MmcCntlr *)OsalMemCalloc(sizeof(struct MmcCntlr));
    if (cntlr == NULL) {
        HDF_LOGE("LinuxEmmcBind: no mem for MmcCntlr.");
        return HDF_ERR_MALLOC_FAIL;
    }

    cntlr->ops = NULL;
    cntlr->hdfDevObj = obj;
    obj->service = &cntlr->service;
    ret = LinuxEmmcCntlrParse(cntlr, obj);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxEmmcBind: cntlr parse fail.");
        goto _ERR;
    }
    cntlr->priv = (void *)GetMmcHost((int32_t)cntlr->index);

    ret = MmcCntlrAdd(cntlr, false);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxEmmcBind: cntlr add fail.");
        goto _ERR;
    }

    ret = MmcCntlrAllocDev(cntlr, (enum MmcDevType)cntlr->devType);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxEmmcBind: alloc dev fail.");
        goto _ERR;
    }
    MmcDeviceAddOps(cntlr->curDev, &g_emmcMethod);
    HDF_LOGD("LinuxEmmcBind: Success!");
    return HDF_SUCCESS;

_ERR:
    LinuxEmmcDeleteCntlr(cntlr);
    HDF_LOGE("LinuxEmmcBind: Fail!");
    return HDF_FAILURE;
}

static int32_t LinuxEmmcInit(struct HdfDeviceObject *obj)
{
    (void)obj;

    HDF_LOGD("LinuxEmmcInit: Success!");
    return HDF_SUCCESS;
}

static void LinuxEmmcRelease(struct HdfDeviceObject *obj)
{
    if (obj == NULL) {
        return;
    }
    LinuxEmmcDeleteCntlr((struct MmcCntlr *)obj->service);
}

struct HdfDriverEntry g_emmcDriverEntry = {
    .moduleVersion = 1,
    .Bind = LinuxEmmcBind,
    .Init = LinuxEmmcInit,
    .Release = LinuxEmmcRelease,
    .moduleName = "HDF_PLATFORM_EMMC",
};
HDF_INIT(g_emmcDriverEntry);
