/*
 * emmc_adatper.c
 *
 * hi35xx linux emmc driver implement.
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

#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <securec.h>
#include "emmc_core.h"
#include "hdf_log.h"

#define HDF_LOG_TAG emmc_adapter_c

struct mmc_host *himci_get_mmc_host(int slot);

int32_t Hi35xxLinuxEmmcGetCid(struct EmmcCntlr *cntlr, uint8_t *cid, uint32_t size)
{
    struct mmc_host *mmcHost = NULL;

    if (cntlr == NULL) {
        HDF_LOGE("Hi35xxLinuxEmmcGetCid: cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cid == NULL || size < EMMC_CID_LEN) {
        HDF_LOGE("Hi35xxLinuxEmmcGetCid: cid is null or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    mmcHost = (struct mmc_host *)cntlr->priv;
    if (mmcHost == NULL) {
        mmcHost = himci_get_mmc_host(cntlr->configData.hostId);
        if (mmcHost == NULL) {
            HDF_LOGE("Hi35xxLinuxEmmcGetCid: get_mmc_host fail again!");
            return HDF_ERR_NOT_SUPPORT;
        }
        cntlr->priv = (void *)mmcHost;
    }
    if (mmcHost->card == NULL) {
        HDF_LOGE("Hi35xxLinuxEmmcGetCid: card is null.");
        return HDF_ERR_NOT_SUPPORT;
    }
    if (memcpy_s(cid, sizeof(uint8_t) * size, (uint8_t *)(mmcHost->card->raw_cid),
        sizeof(mmcHost->card->raw_cid)) != EOK) {
        HDF_LOGE("Hi35xxLinuxEmmcGetCid: memcpy_s fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t Hi35xxLinuxEmmcFindHost(struct EmmcCntlr *cntlr, struct EmmcConfigData *data)
{
    if (cntlr == NULL || data == NULL) {
        HDF_LOGE("Hi35xxLinuxEmmcFindHost: cntlr or data is null.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr->priv = (void *)himci_get_mmc_host(data->hostId);
    if (cntlr->priv == NULL) {
        HDF_LOGE("Hi35xxLinuxEmmcFindHost: get_mmc_host fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct EmmcMethod g_emmcMethod = {
    .getCid = Hi35xxLinuxEmmcGetCid,
    .findHost = Hi35xxLinuxEmmcFindHost,
};

static int32_t Hi35xxLinuxEmmcBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("Hi35xxLinuxEmmcBind: Fail, device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    HDF_LOGD("Hi35xxLinuxEmmcBind: Success.");
    return (EmmcCntlrCreateAndBind(device) == NULL) ? HDF_FAILURE : HDF_SUCCESS;
}

static int32_t Hi35xxLinuxEmmcInit(struct HdfDeviceObject *device)
{
    struct EmmcCntlr *cntlr = NULL;
    int32_t ret;

    if (device == NULL) {
        HDF_LOGE("Hi35xxLinuxEmmcInit: device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = EmmcCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("Hi35xxLinuxEmmcInit: EmmcCntlrFromDevice fail.");
        return HDF_ERR_IO;
    }

    ret = EmmcFillConfigData(device, &(cntlr->configData));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Hi35xxLinuxEmmcInit: EmmcFillConfigData fail.");
        return HDF_ERR_IO;
    }

    cntlr->priv = (void *)himci_get_mmc_host(cntlr->configData.hostId);
    cntlr->method = &g_emmcMethod;
    HDF_LOGD("Hi35xxLinuxEmmcInit: Success!");
    return HDF_SUCCESS;
}

static void Hi35xxLinuxEmmcRelease(struct HdfDeviceObject *device)
{
    struct EmmcCntlr *cntlr = NULL;

    cntlr = EmmcCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("Hi35xxLinuxEmmcRelease: Fail, cntlr is NULL.");
        return;
    }

    EmmcCntlrDestroy(cntlr);
    HDF_LOGD("Hi35xxLinuxEmmcRelease: Success.");
}

struct HdfDriverEntry g_emmcDriverEntry = {
    .moduleVersion = 1,
    .Bind = Hi35xxLinuxEmmcBind,
    .Init = Hi35xxLinuxEmmcInit,
    .Release = Hi35xxLinuxEmmcRelease,
    .moduleName = "HDF_PLATFORM_EMMC",
};
HDF_INIT(g_emmcDriverEntry);
