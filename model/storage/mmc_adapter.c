/*
 * mmc_adapter.c
 *
 * linux mmc driver implement.
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

#define HDF_LOG_TAG mmc_adapter_c

struct mmc_host *GetMmcHost(int32_t slot)
{
    (void)slot;
    HDF_LOGE("Vendor need to adapter GetMmcHost");
    return NULL;
}

void SdioRescan(int32_t slot)
{
    (void)slot;
    HDF_LOGE("Vendor need to adapter SdioRescan");
    return;
}
