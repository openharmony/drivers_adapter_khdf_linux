/*
 * hdf_driver_module.h
 *
 * osal driver
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

#ifndef HDF_DRIVER_MODULE_H
#define HDF_DRIVER_MODULE_H

#include <linux/kernel.h>
#include <linux/module.h>

#include "hdf_driver.h"

#define HDF_DRIVER_MODULE(module)                      \
    static int __init hdf_driver_init(void)            \
    {                                                  \
        HDF_LOGI("hdf driver " #module " register");   \
        return HdfRegisterDriverEntry(&module);        \
    }                                                  \
    static void __exit hdf_driver_exit(void)           \
    {                                                  \
        HDF_LOGI("hdf driver " #module " unregister"); \
        HdfUnregisterDriverEntry(&module);             \
    }                                                  \
    module_init(hdf_driver_init);                      \
    module_exit(hdf_driver_exit);

#endif // HDF_DRIVER_MODULE_H