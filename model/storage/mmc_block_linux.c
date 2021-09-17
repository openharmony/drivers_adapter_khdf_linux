/*
 * storage_block_linux.c
 *
 * storage block adapter of linux
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

#include "mmc_block.h"

int32_t MmcBlockOsInit(struct MmcDevice *mmcDevice)
{
    (void)mmcDevice;
    return HDF_SUCCESS;
}

void MmcBlockOsUninit(struct MmcDevice *mmcDevice)
{
    (void)mmcDevice;
}
