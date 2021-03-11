/*
 * sdio_adatper.c
 *
 * hi35xx linux sdio driver implement.
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
#include "plat_log.h"
#include "sdio_core.h"

#define HDF_LOG_TAG sdio_adapter_c

#define DATA_LEN_ONE_BYTE 1
#define DATA_LEN_TWO_BYTES 2
#define DATA_LEN_FOUR_BYTES 4
#define MMC_SLOT_NUM 3

struct mmc_host *himci_get_mmc_host(int slot);

static struct sdio_func *Hi35xxLinuxSdioGetFunc(struct SdioCntlr *cntlr)
{
    if (cntlr == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioGetFunc: cntlr is null.");
        return NULL;
    }
    return (struct sdio_func *)cntlr->priv;
}

static int32_t Hi35xxLinuxSdioIncrAddrReadBytes(struct SdioCntlr *cntlr,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t timeOut)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);
    int32_t ret = HDF_SUCCESS;
    uint16_t *output16 = NULL;
    uint32_t *output32 = NULL;

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioIncrAddrReadBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("Hi35xxLinuxSdioIncrAddrReadBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (timeOut > 0) {
        PLAT_LOGV("Hi35xxLinuxSdioIncrAddrReadBytes: use current timeOut value.");
        func->enable_timeout = timeOut;
    }
    if (size == DATA_LEN_ONE_BYTE) {
        PLAT_LOGV("Hi35xxLinuxSdioIncrAddrReadBytes: Read one byte Success!");
        *data = sdio_readb(func, addr, &ret);
        return ret;
    }
    if (size == DATA_LEN_TWO_BYTES) {
        PLAT_LOGV("Hi35xxLinuxSdioIncrAddrReadBytes: Read two bytes Success!");
        output16 = (uint16_t *)data;
        *output16 = sdio_readw(func, addr, &ret);
        return ret;
    }
    if (size == DATA_LEN_FOUR_BYTES) {
        PLAT_LOGV("Hi35xxLinuxSdioIncrAddrReadBytes: Read four bytes Success!");
        output32 = (uint32_t *)data;
        *output32 = sdio_readl(func, addr, &ret);
        return ret;
    }
    PLAT_LOGV("Hi35xxLinuxSdioIncrAddrReadBytes: Success!");
    return sdio_memcpy_fromio(func, data, addr, size);
}

static int32_t Hi35xxLinuxSdioIncrAddrWriteBytes(struct SdioCntlr *cntlr,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t timeOut)
{
    int32_t ret = HDF_SUCCESS;
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioIncrAddrWriteBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("Hi35xxLinuxSdioIncrAddrWriteBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (timeOut > 0) {
        PLAT_LOGV("Hi35xxLinuxSdioIncrAddrWriteBytes: use current timeOut value.");
        func->enable_timeout = timeOut;
    }
    if (size == DATA_LEN_ONE_BYTE) {
        PLAT_LOGV("Hi35xxLinuxSdioIncrAddrWriteBytes: write one byte Success!");
        sdio_writeb(func, *data, addr, &ret);
        return ret;
    }
    if (size == DATA_LEN_TWO_BYTES) {
        PLAT_LOGV("Hi35xxLinuxSdioIncrAddrWriteBytes: write two bytes Success!");
        sdio_writew(func, *(uint16_t *)data, addr, &ret);
        return ret;
    }
    if (size == DATA_LEN_FOUR_BYTES) {
        PLAT_LOGV("Hi35xxLinuxSdioIncrAddrWriteBytes: write four bytes Success!");
        sdio_writel(func, *(uint32_t *)data, addr, &ret);
        return ret;
    }
    PLAT_LOGV("Hi35xxLinuxSdioIncrAddrWriteBytes: Success!");
    return sdio_memcpy_toio(func, addr, data, size);
}

static int32_t Hi35xxLinuxSdioFixedAddrReadBytes(struct SdioCntlr *cntlr,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t timeOut)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioFixedAddrReadBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("Hi35xxLinuxSdioFixedAddrReadBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (timeOut > 0) {
        PLAT_LOGV("Hi35xxLinuxSdioFixedAddrReadBytes: use current timeOut value.");
        func->enable_timeout = timeOut;
    }
    PLAT_LOGV("Hi35xxLinuxSdioFixedAddrReadBytes: Success!");
    return sdio_readsb(func, data, addr, size);
}

static int32_t Hi35xxLinuxSdioFixedAddrWriteBytes(struct SdioCntlr *cntlr,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t timeOut)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioFixedAddrWriteBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("Hi35xxLinuxSdioFixedAddrReadBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (timeOut > 0) {
        PLAT_LOGV("Hi35xxLinuxSdioFixedAddrWriteBytes: use current timeOut value.");
        func->enable_timeout = timeOut;
    }
    PLAT_LOGV("Hi35xxLinuxSdioFixedAddrWriteBytes: Success!");
    return sdio_writesb(func, addr, data, size);
}

static int32_t Hi35xxLinuxSdioFunc0ReadBytes(struct SdioCntlr *cntlr,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t timeOut)
{
    int32_t ret = HDF_SUCCESS;
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioFunc0ReadBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("Hi35xxLinuxSdioFunc0ReadBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (timeOut > 0) {
        PLAT_LOGV("Hi35xxLinuxSdioFunc0ReadBytes: use current timeOut value.");
        func->enable_timeout = timeOut;
    }

    *data = sdio_f0_readb(func, addr, &ret);
    PLAT_LOGV("Hi35xxLinuxSdioFunc0ReadBytes: Success!");
    return ret;
}

static int32_t Hi35xxLinuxSdioFunc0WriteBytes(struct SdioCntlr *cntlr,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t timeOut)
{
    int32_t ret = HDF_SUCCESS;
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioFunc0WriteBytes: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if ((data == NULL) || (size == 0)) {
        HDF_LOGE("Hi35xxLinuxSdioFunc0WriteBytes: data or size is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    if (timeOut > 0) {
        PLAT_LOGV("Hi35xxLinuxSdioFunc0WriteBytes: use current timeOut value.");
        func->enable_timeout = timeOut;
    }

    sdio_f0_writeb(func, *data, addr, &ret);
    PLAT_LOGV("Hi35xxLinuxSdioFunc0WriteBytes: Success!");
    return ret;
}

static int32_t Hi35xxLinuxSdioSetBlockSize(struct SdioCntlr *cntlr, uint32_t blockSize)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioSetBlockSize, func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    PLAT_LOGV("Hi35xxLinuxSdioSetBlockSize: Success!");
    return sdio_set_block_size(func, blockSize);
}

static int32_t Hi35xxLinuxSdioGetCommonInfo(struct SdioCntlr *cntlr,
    SdioCommonInfo *info, uint32_t infoType)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioGetCommonInfo: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (info == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioGetCommonInfo: info is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (infoType != SDIO_FUNC_INFO) {
        HDF_LOGE("Hi35xxLinuxSdioGetCommonInfo: cur type %d is not support.", infoType);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (func->card == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioGetCommonInfo fail, card is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (func->card->host == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioGetCommonInfo fail, host is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    info->funcInfo.enTimeout = func->enable_timeout;
    info->funcInfo.maxBlockNum = func->card->host->max_blk_count;
    info->funcInfo.maxBlockSize = func->card->host->max_blk_size;
    info->funcInfo.maxRequestSize = func->card->host->max_req_size;
    info->funcInfo.funcNum = func->num;
    info->funcInfo.irqCap = func->card->host->caps & MMC_CAP_SDIO_IRQ;
    info->funcInfo.data = func;
    PLAT_LOGV("Hi35xxLinuxSdioGetCommonInfo: get %d info Success!", infoType);
    return HDF_SUCCESS;
}

static int32_t Hi35xxLinuxSdioSetCommonInfo(struct SdioCntlr *cntlr,
    SdioCommonInfo *info, uint32_t infoType)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioSetCommonInfo: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (info == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioSetCommonInfo: info is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (infoType != SDIO_FUNC_INFO) {
        HDF_LOGE("Hi35xxLinuxSdioSetCommonInfo: cur type %d is not support.", infoType);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (func->card == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioSetCommonInfo fail, card is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (func->card->host == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioSetCommonInfo fail, host is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    func->enable_timeout = info->funcInfo.enTimeout;
    func->card->host->max_blk_count = info->funcInfo.maxBlockNum;
    func->card->host->max_blk_size = info->funcInfo.maxBlockSize;
    func->card->host->max_req_size = info->funcInfo.maxRequestSize;
    func->num = info->funcInfo.funcNum;
    PLAT_LOGV("Hi35xxLinuxSdioSetCommonInfo: set %d info Success!", infoType);
    return HDF_SUCCESS;
}

static int32_t Hi35xxLinuxSdioFlushData(struct SdioCntlr *cntlr)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioFlushData: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (func->card == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioFlushData: card is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    return mmc_sw_reset(func->card->host);
}

static void Hi35xxLinuxSdioClaimHost(struct SdioCntlr *cntlr)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioClaimHost: func is NULL.");
        return;
    }
    PLAT_LOGV("Hi35xxLinuxSdioClaimHost: Success!");
    sdio_claim_host(func);
}

static void Hi35xxLinuxSdioReleaseHost(struct SdioCntlr *cntlr)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioReleaseHost: func is NULL.");
        return;
    }
    PLAT_LOGV("Hi35xxLinuxSdioReleaseHost: Success!");
    sdio_release_host(func);
}

static int32_t Hi35xxLinuxSdioEnableFunc(struct SdioCntlr *cntlr)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioEnableFunc: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    PLAT_LOGV("Hi35xxLinuxSdioEnableFunc: Success!");
    return sdio_enable_func(func);
}

static int32_t Hi35xxLinuxSdioDisableFunc(struct SdioCntlr *cntlr)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioDisableFunc: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    PLAT_LOGV("Hi35xxLinuxSdioDisableFunc: Success!");
    return sdio_disable_func(func);
}

static int32_t Hi35xxLinuxSdioClaimIrq(struct SdioCntlr *cntlr, SdioIrqHandler *handler)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioClaimIrq: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (handler == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioClaimIrq: handler is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    PLAT_LOGV("Hi35xxLinuxSdioClaimIrq: Success!");
    return sdio_claim_irq(func, (sdio_irq_handler_t *)handler);
}

static int32_t Hi35xxLinuxSdioReleaseIrq(struct SdioCntlr *cntlr)
{
    struct sdio_func *func = Hi35xxLinuxSdioGetFunc(cntlr);

    if (func == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioReleaseIrq: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    PLAT_LOGV("Hi35xxLinuxSdioReleaseIrq: Success!");
    return sdio_release_irq(func);
}

static struct sdio_func *Hi35xxLinuxSdioSearchFunc(uint32_t funcNum, uint32_t vendorId, uint32_t deviceId)
{
    struct mmc_card *card = NULL;
    struct mmc_host *host = NULL;
    struct sdio_func *func = NULL;
    uint32_t i, j;

    for (i = 0; i < MMC_SLOT_NUM; i++) {
        host = himci_get_mmc_host(i);
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
                return func;
            }
        }
    }

    HDF_LOGE("Hi35xxLinuxSdioSearchFunc: get sdio func fail!");
    return NULL;
}

static int32_t Hi35xxLinuxSdioFindFunc(struct SdioCntlr *cntlr, struct SdioConfigData *configData)
{
    if (cntlr == NULL || configData == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioFindFunc: cntlr or configData is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr->priv = Hi35xxLinuxSdioSearchFunc(configData->funcNum, configData->vendorId, configData->deviceId);
    if (cntlr->priv == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioFindFunc: Hi35xxLinuxSdioSearchFunc fail.");
        return HDF_ERR_NOT_SUPPORT;
    }
    PLAT_LOGV("Hi35xxLinuxSdioFindFunc: Success.");
    return HDF_SUCCESS;
}

static struct SdioMethod g_method = {
    .incrAddrReadBytes = Hi35xxLinuxSdioIncrAddrReadBytes,
    .incrAddrWriteBytes = Hi35xxLinuxSdioIncrAddrWriteBytes,
    .fixedAddrReadBytes = Hi35xxLinuxSdioFixedAddrReadBytes,
    .fixedAddrWriteBytes = Hi35xxLinuxSdioFixedAddrWriteBytes,
    .func0ReadBytes = Hi35xxLinuxSdioFunc0ReadBytes,
    .func0WriteBytes = Hi35xxLinuxSdioFunc0WriteBytes,
    .setBlockSize = Hi35xxLinuxSdioSetBlockSize,
    .getCommonInfo = Hi35xxLinuxSdioGetCommonInfo,
    .setCommonInfo = Hi35xxLinuxSdioSetCommonInfo,
    .flushData = Hi35xxLinuxSdioFlushData,
    .claimHost = Hi35xxLinuxSdioClaimHost,
    .releaseHost = Hi35xxLinuxSdioReleaseHost,
    .enableFunc = Hi35xxLinuxSdioEnableFunc,
    .disableFunc = Hi35xxLinuxSdioDisableFunc,
    .claimIrq = Hi35xxLinuxSdioClaimIrq,
    .releaseIrq = Hi35xxLinuxSdioReleaseIrq,
    .findFunc = Hi35xxLinuxSdioFindFunc,
};

static int32_t Hi35xxLinuxSdioBind(struct HdfDeviceObject *device)
{
    PLAT_LOGV("Hi35xxLinuxSdioBind: entry.");
    if (device == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioBind: Fail, device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    PLAT_LOGV("Hi35xxLinuxSdioBind: Success.");
    return (SdioCntlrCreateAndBind(device) == NULL) ? HDF_FAILURE : HDF_SUCCESS;
}

static int32_t Hi35xxLinuxSdioInit(struct HdfDeviceObject *device)
{
    struct SdioCntlr *cntlr = NULL;
    int32_t ret;

    PLAT_LOGV("Hi35xxLinuxSdioInit: entry!");
    if (device == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioInit: device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = SdioCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioInit: SdioHostFromDevice fail.");
        return HDF_ERR_IO;
    }

    ret = SdioFillConfigData(device, &(cntlr->configData));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Hi35xxLinuxSdioInit: SdioFillConfigData fail.");
        return HDF_ERR_IO;
    }
    cntlr->method = &g_method;
    PLAT_LOGV("Hi35xxLinuxSdioInit: Success!");
    return HDF_SUCCESS;
}

static void Hi35xxLinuxSdioRelease(struct HdfDeviceObject *device)
{
    struct SdioCntlr *cntlr = NULL;

    PLAT_LOGV("Hi35xxLinuxSdioRelease: entry!");
    cntlr = SdioCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("Hi35xxLinuxSdioRelease: Fail, cntlr is NULL.");
        return;
    }
    SdioCntlrDestroy(cntlr);
    PLAT_LOGV("Hi35xxLinuxSdioRelease: Success.");
}

struct HdfDriverEntry g_sdioDriverEntry = {
    .moduleVersion = 1,
    .Bind = Hi35xxLinuxSdioBind,
    .Init = Hi35xxLinuxSdioInit,
    .Release = Hi35xxLinuxSdioRelease,
    .moduleName = "HDF_PLATFORM_SDIO",
};
HDF_INIT(g_sdioDriverEntry);
