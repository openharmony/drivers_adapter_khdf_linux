/*
 * sym_export.c
 *
 * HDF symbol export file
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
#include <linux/kernel.h>
#include <linux/module.h>

#include <devmgr_service.h>
#include <hdf_base.h>
#include <hdf_device_desc.h>
#include <hdf_device_object.h>
#include <hdf_driver.h>
#include <hdf_pm.h>
#include <hdf_sbuf.h>

EXPORT_SYMBOL(HdfDeviceSendEvent);
EXPORT_SYMBOL(HdfDeviceSendEventToClient);
EXPORT_SYMBOL(HdfDeviceGetServiceName);
EXPORT_SYMBOL(HdfPmRegisterPowerListener);
EXPORT_SYMBOL(HdfDeviceObjectAlloc);
EXPORT_SYMBOL(HdfDeviceObjectRelease);
EXPORT_SYMBOL(HdfDeviceObjectRegister);
EXPORT_SYMBOL(HdfDeviceObjectPublishService);
EXPORT_SYMBOL(HdfDeviceObjectSetServInfo);
EXPORT_SYMBOL(HdfDeviceObjectUpdate);
EXPORT_SYMBOL(DevmgrServiceGetInstance);
EXPORT_SYMBOL(HdfSbufWriteInt32);
EXPORT_SYMBOL(HdfSbufReadUint32);
EXPORT_SYMBOL(HdfSbufReadString);
EXPORT_SYMBOL(HdfUnregisterDriverEntry);
EXPORT_SYMBOL(HdfRegisterDriverEntry);
EXPORT_SYMBOL(HdfSbufReadInt32);
