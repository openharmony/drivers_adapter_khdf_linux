#
# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
#

HDF_WIFI_FRAMEWORKS_ROOT = $(HDF_DIR_PREFIX)/framework/model/network/wifi
HDF_WIFI_KHDF_FRAMEWORKS_ROOT = $(HDF_DIR_PREFIX)/adapter/khdf/linux/model/network/wifi
HDF_FRAMEWORKS_INC := \
   -I$(srctree)/drivers/hdf/framework/core/common/include/host \
   -I$(srctree)/drivers/hdf/framework/core/host/include \
   -I$(srctree)/drivers/hdf/framework/core/manager/include \
   -I$(srctree)/drivers/hdf/framework/core/shared/include \
   -I$(srctree)/drivers/hdf/framework/include \
   -I$(srctree)/drivers/hdf/framework/include/config \
   -I$(srctree)/drivers/hdf/framework/include/core \
   -I$(srctree)/drivers/hdf/framework/include/platform \
   -I$(srctree)/drivers/hdf/framework/include/utils \
   -I$(srctree)/drivers/hdf/framework/support/platform/include \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/platform \
   -I$(srctree)/drivers/hdf/framework/utils/include \
   -I$(srctree)/drivers/hdf/khdf/osal/include \
   -I$(srctree)/drivers/hdf/khdf/config/include \
   -I$(srctree)/include/hdf \
   -I$(srctree)/include/hdf/osal \
   -I$(srctree)/include/hdf/utils

HDF_WIFI_FRAMEWORKS_INC := \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/core/components/eapol \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/core/components/softap \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/core/components/sta \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/core/components/p2p \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/include \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/core \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/core/module \
   -I$(srctree)/drivers/hdf/framework/model/network/common/netdevice \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/platform/include \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/platform/include/message \
   -I$(srctree)/drivers/hdf/framework/model/network/wifi/client/include \
   -I$(srctree)/drivers/hdf/framework/include/wifi \
   -I$(srctree)/drivers/hdf/framework/include/net \
   -I$(srctree)/drivers/hdf/frameworks/model/network/wifi/bus

HDF_WIFI_ADAPTER_INC := \
   -I$(srctree)/drivers/hdf/khdf/network/include

HDF_WIFI_VENDOR_INC := \
   -I$(srctree)/drivers/hdf/wifi

SECURE_LIB_INC := \
   -I$(srctree)/bounds_checking_function/include
