/*
 * sdio_adapter.c
 *
 * linux sdio driver implement.
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
#include <linux/mmc/core.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include "device_resource_if.h"
#include "mmc_corex.h"
#include "mmc_sdio.h"

#define HDF_LOG_TAG sdio_adapter_c

#define DATA_LEN_ONE_BYTE 1
#define DATA_LEN_TWO_BYTES 2
#define DATA_LEN_FOUR_BYTES 4
#define MMC_SLOT_NUM 3
#define SDIO_RESCAN_WAIT_TIME 40

struct mmc_host *GetMmcHost(int32_t slot);
void SdioRescan(int32_t slot);

enum SleepTime {
    MS_10 = 10,
    MS_50 = 50,
};

static struct sdio_func *LinuxSdioGetFunc(struct SdioDevice *dev)
{
    if (dev == NULL) {
        HDF_LOGE("LinuxSdioGetFunc: dev is null.");
        return NULL;
    }
    return (struct sdio_func *)dev->sd.mmc.priv;
}

static int32_t LinuxSdioIncrAddrReadBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);
    int32_t ret = HDF_SUCCESS;
    uint16_t *output16 = NULL;
    uint32_t *output32 = NULL;

    if (func == NULL) {
        HDF_LOGE("LinuxSdioIncrAddrReadBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("LinuxSdioIncrAddrReadBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (size == DATA_LEN_ONE_BYTE) {
        *data = sdio_readb(func, addr, &ret);
        return ret;
    }
    if (size == DATA_LEN_TWO_BYTES) {
        output16 = (uint16_t *)data;
        *output16 = sdio_readw(func, addr, &ret);
        return ret;
    }
    if (size == DATA_LEN_FOUR_BYTES) {
        output32 = (uint32_t *)data;
        *output32 = sdio_readl(func, addr, &ret);
        return ret;
    }
    return sdio_memcpy_fromio(func, data, addr, size);
}

static int32_t LinuxSdioIncrAddrWriteBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    int32_t ret = HDF_SUCCESS;
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioIncrAddrWriteBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("LinuxSdioIncrAddrWriteBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (size == DATA_LEN_ONE_BYTE) {
        sdio_writeb(func, *data, addr, &ret);
        return ret;
    }
    if (size == DATA_LEN_TWO_BYTES) {
        sdio_writew(func, *(uint16_t *)data, addr, &ret);
        return ret;
    }
    if (size == DATA_LEN_FOUR_BYTES) {
        sdio_writel(func, *(uint32_t *)data, addr, &ret);
        return ret;
    }
    return sdio_memcpy_toio(func, addr, data, size);
}

static int32_t LinuxSdioFixedAddrReadBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioFixedAddrReadBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("LinuxSdioFixedAddrReadBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (scatterLen > 0) {
        HDF_LOGE("LinuxSdioFixedAddrReadBytes: not support!");
        return HDF_ERR_NOT_SUPPORT;
    }
    return sdio_readsb(func, data, addr, size);
}

static int32_t LinuxSdioFixedAddrWriteBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioFixedAddrWriteBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("LinuxSdioFixedAddrReadBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (scatterLen > 0) {
        HDF_LOGE("LinuxSdioFixedAddrWriteBytes: not support!");
        return HDF_ERR_NOT_SUPPORT;
    }
    return sdio_writesb(func, addr, data, size);
}

static int32_t LinuxSdioFunc0ReadBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    int32_t ret = HDF_SUCCESS;
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioFunc0ReadBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("LinuxSdioFunc0ReadBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    *data = sdio_f0_readb(func, addr, &ret);
    return ret;
}

static int32_t LinuxSdioFunc0WriteBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    int32_t ret = HDF_SUCCESS;
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioFunc0WriteBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("LinuxSdioFunc0WriteBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    sdio_f0_writeb(func, *data, addr, &ret);
    return ret;
}

static int32_t LinuxSdioSetBlockSize(struct SdioDevice *dev, uint32_t blockSize)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioSetBlockSize, func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    return sdio_set_block_size(func, blockSize);
}

static int32_t LinuxSdioGetCommonInfo(struct SdioDevice *dev,
    SdioCommonInfo *info, uint32_t infoType)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioGetCommonInfo: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (info == NULL) {
        HDF_LOGE("LinuxSdioGetCommonInfo: info is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (infoType != SDIO_FUNC_INFO) {
        HDF_LOGE("LinuxSdioGetCommonInfo: cur type %u is not support.", infoType);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (func->card == NULL) {
        HDF_LOGE("LinuxSdioGetCommonInfo fail, card is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (func->card->host == NULL) {
        HDF_LOGE("LinuxSdioGetCommonInfo fail, host is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    info->funcInfo.enTimeout = func->enable_timeout;
    info->funcInfo.maxBlockNum = func->card->host->max_blk_count;
    info->funcInfo.maxBlockSize = func->card->host->max_blk_size;
    info->funcInfo.maxRequestSize = func->card->host->max_req_size;
    info->funcInfo.funcNum = func->num;
    info->funcInfo.irqCap = func->card->host->caps & MMC_CAP_SDIO_IRQ;
    info->funcInfo.data = func;
    return HDF_SUCCESS;
}

static int32_t LinuxSdioSetCommonInfo(struct SdioDevice *dev,
    SdioCommonInfo *info, uint32_t infoType)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioSetCommonInfo: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (info == NULL) {
        HDF_LOGE("LinuxSdioSetCommonInfo: info is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (infoType != SDIO_FUNC_INFO) {
        HDF_LOGE("LinuxSdioSetCommonInfo: cur type %u is not support.", infoType);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (func->card == NULL) {
        HDF_LOGE("LinuxSdioSetCommonInfo fail, card is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (func->card->host == NULL) {
        HDF_LOGE("LinuxSdioSetCommonInfo fail, host is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    func->enable_timeout = info->funcInfo.enTimeout;
    func->card->host->max_blk_count = info->funcInfo.maxBlockNum;
    func->card->host->max_blk_size = info->funcInfo.maxBlockSize;
    func->card->host->max_req_size = info->funcInfo.maxRequestSize;
    func->num = info->funcInfo.funcNum;
    return HDF_SUCCESS;
}

static int32_t LinuxSdioFlushData(struct SdioDevice *dev)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioFlushData: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (func->card == NULL) {
        HDF_LOGE("LinuxSdioFlushData: card is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    return mmc_sw_reset(func->card->host);
}

static int32_t LinuxSdioClaimHost(struct SdioDevice *dev)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioClaimHost: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    sdio_claim_host(func);
    return HDF_SUCCESS;
}

static int32_t LinuxSdioReleaseHost(struct SdioDevice *dev)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioReleaseHost: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    sdio_release_host(func);
    return HDF_SUCCESS;
}

static int32_t LinuxSdioEnableFunc(struct SdioDevice *dev)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioEnableFunc: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    return sdio_enable_func(func);
}

static int32_t LinuxSdioDisableFunc(struct SdioDevice *dev)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioDisableFunc: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    return sdio_disable_func(func);
}

static int32_t LinuxSdioClaimIrq(struct SdioDevice *dev, SdioIrqHandler *handler)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioClaimIrq: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (handler == NULL) {
        HDF_LOGE("LinuxSdioClaimIrq: handler is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    return sdio_claim_irq(func, (sdio_irq_handler_t *)handler);
}

static int32_t LinuxSdioReleaseIrq(struct SdioDevice *dev)
{
    struct sdio_func *func = LinuxSdioGetFunc(dev);

    if (func == NULL) {
        HDF_LOGE("LinuxSdioReleaseIrq: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    return sdio_release_irq(func);
}

static struct sdio_func *LinuxSdioSearchFunc(uint32_t funcNum, uint16_t vendorId, uint16_t deviceId)
{
    struct mmc_card *card = NULL;
    struct mmc_host *host = NULL;
    struct sdio_func *func = NULL;
    uint32_t i, j;

    for (i = 0; i < MMC_SLOT_NUM; i++) {
        host = GetMmcHost(i);
        if (host == NULL) {
            continue;
        }
        card = host->card;
        if (card == NULL) {
            continue;
        }
        for (j = 0; j <= card->sdio_funcs; j++) {
            func = card->sdio_func[j];
            if ((func != NULL) &&
                (func->num == funcNum) &&
                (func->vendor == vendorId) &&
                (func->device == deviceId)) {
                HDF_LOGD("LinuxSdioSearchFunc: find func!");
                return func;
            }
        }
    }

    HDF_LOGE("LinuxSdioSearchFunc: get sdio func fail!");
    return NULL;
}

static int32_t LinuxSdioFindFunc(struct SdioDevice *dev, struct SdioFunctionConfig *configData)
{
    if (dev == NULL || configData == NULL) {
        HDF_LOGE("LinuxSdioFindFunc: dev or configData is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    dev->sd.mmc.priv = LinuxSdioSearchFunc(configData->funcNr, configData->vendorId, configData->deviceId);
    if (dev->sd.mmc.priv == NULL) {
        HDF_LOGE("LinuxSdioFindFunc: LinuxSdioSearchFunc fail.");
        return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static struct SdioDeviceOps g_sdioDeviceOps = {
    .incrAddrReadBytes = LinuxSdioIncrAddrReadBytes,
    .incrAddrWriteBytes = LinuxSdioIncrAddrWriteBytes,
    .fixedAddrReadBytes = LinuxSdioFixedAddrReadBytes,
    .fixedAddrWriteBytes = LinuxSdioFixedAddrWriteBytes,
    .func0ReadBytes = LinuxSdioFunc0ReadBytes,
    .func0WriteBytes = LinuxSdioFunc0WriteBytes,
    .setBlockSize = LinuxSdioSetBlockSize,
    .getCommonInfo = LinuxSdioGetCommonInfo,
    .setCommonInfo = LinuxSdioSetCommonInfo,
    .flushData = LinuxSdioFlushData,
    .enableFunc = LinuxSdioEnableFunc,
    .disableFunc = LinuxSdioDisableFunc,
    .claimIrq = LinuxSdioClaimIrq,
    .releaseIrq = LinuxSdioReleaseIrq,
    .findFunc = LinuxSdioFindFunc,
    .claimHost = LinuxSdioClaimHost,
    .releaseHost = LinuxSdioReleaseHost,
};

static bool LinuxSdioRescanFinish(struct MmcCntlr *cntlr)
{
    struct mmc_host *host = NULL;
    struct mmc_card *card = NULL;

    host = GetMmcHost(cntlr->index);
    if (host == NULL) {
        return false;
    }

    card = host->card;
    if (card == NULL) {
        return false;
    }
    if (card->sdio_funcs > 0) {
        return true;
    }
    return false;
}

static int32_t LinuxSdioRescan(struct MmcCntlr *cntlr)
{
    bool rescanFinish = false;
    uint32_t count = 0;

    if (cntlr == NULL) {
        HDF_LOGE("LinuxSdioRescan: cntlr is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    SdioRescan(cntlr->index);
    while (rescanFinish == false && count < SDIO_RESCAN_WAIT_TIME) {
        OsalMSleep(MS_50);
        count++;
        rescanFinish = LinuxSdioRescanFinish(cntlr);
    }

    if (rescanFinish == false) {
        HDF_LOGE("LinuxSdioRescan: fail!");
        return HDF_FAILURE;
    }

    OsalMSleep(MS_10);
    return HDF_SUCCESS;
}

struct MmcCntlrOps g_sdioCntlrOps = {
    .rescanSdioDev = LinuxSdioRescan,
};

static void LinuxSdioDeleteCntlr(struct MmcCntlr *cntlr)
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

static int32_t LinuxSdioCntlrParse(struct MmcCntlr *cntlr, struct HdfDeviceObject *obj)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (obj == NULL || cntlr == NULL) {
        HDF_LOGE("LinuxSdioCntlrParse: input para is NULL.");
        return HDF_FAILURE;
    }

    node = obj->property;
    if (node == NULL) {
        HDF_LOGE("LinuxSdioCntlrParse: drs node is NULL.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint16 == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("LinuxSdioCntlrParse: invalid drs ops fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint16(node, "hostId", &(cntlr->index), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxSdioCntlrParse: read hostIndex fail!");
        return ret;
    }
    ret = drsOps->GetUint32(node, "devType", &(cntlr->devType), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxSdioCntlrParse: read devType fail!");
        return ret;
    }
    HDF_LOGD("LinuxSdioCntlrParse: hostIndex = %d, devType = %d.", cntlr->index, cntlr->devType);
    return HDF_SUCCESS;
}

static int32_t LinuxSdioBind(struct HdfDeviceObject *obj)
{
    struct MmcCntlr *cntlr = NULL;
    int32_t ret;

    if (obj == NULL) {
        HDF_LOGE("LinuxSdioBind: Fail, obj is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct MmcCntlr *)OsalMemCalloc(sizeof(struct MmcCntlr));
    if (cntlr == NULL) {
        HDF_LOGE("LinuxSdioBind: no mem for MmcCntlr.");
        return HDF_ERR_MALLOC_FAIL;
    }

    cntlr->ops = &g_sdioCntlrOps;
    cntlr->hdfDevObj = obj;
    obj->service = &cntlr->service;
    /* init cntlr. */
    ret = LinuxSdioCntlrParse(cntlr, obj);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxSdioBind: cntlr parse fail.");
        goto _ERR;
    }

    ret = MmcCntlrAdd(cntlr, false);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxSdioBind: cntlr add fail.");
        goto _ERR;
    }

    ret = MmcCntlrAllocDev(cntlr, (enum MmcDevType)cntlr->devType);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("LinuxSdioBind: alloc dev fail.");
        goto _ERR;
    }
    MmcDeviceAddOps(cntlr->curDev, &g_sdioDeviceOps);
    HDF_LOGD("LinuxSdioBind: Success!");
    return HDF_SUCCESS;

_ERR:
    LinuxSdioDeleteCntlr(cntlr);
    HDF_LOGE("LinuxSdioBind: Fail!");
    return HDF_FAILURE;
}

static int32_t LinuxSdioInit(struct HdfDeviceObject *obj)
{
    (void)obj;
    HDF_LOGD("LinuxSdioInit: Success!");
    return HDF_SUCCESS;
}

static void LinuxSdioRelease(struct HdfDeviceObject *obj)
{
    if (obj == NULL) {
        return;
    }
    LinuxSdioDeleteCntlr((struct MmcCntlr *)obj->service);
}

struct HdfDriverEntry g_sdioDriverEntry = {
    .moduleVersion = 1,
    .Bind = LinuxSdioBind,
    .Init = LinuxSdioInit,
    .Release = LinuxSdioRelease,
    .moduleName = "HDF_PLATFORM_SDIO",
};
HDF_INIT(g_sdioDriverEntry);
