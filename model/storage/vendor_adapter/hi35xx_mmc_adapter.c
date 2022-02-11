/*
 * hi35xx_mmc_adapter.c
 *
 * hi35xx linux mmc driver implement.
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

#include <linux/mmc/host.h>
#include "hdf_base.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hi35xx_mmc_adapter_c

struct mmc_host *himci_get_mmc_host(int32_t slot);
void hisi_sdio_rescan(int slot);

struct mmc_host *GetMmcHost(int32_t slot)
{
    HDF_LOGD("hi35xx GetMmcHost entry");
    return himci_get_mmc_host(slot);
}

void SdioRescan(int slot)
{
    HDF_LOGD("hi35xx SdioRescan entry");
    hisi_sdio_rescan(slot);
}
