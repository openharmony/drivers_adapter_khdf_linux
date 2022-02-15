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

MODULE_NAME := hdf_vendor_wifi

HDF_VENDOR_PREFIX := ../../../../../../../../

include drivers/hdf/khdf/model/network/wifi/hdfwifi.mk

obj-$(CONFIG_DRIVERS_HDF_WIFI) += $(MODULE_NAME).o
$(MODULE_NAME)-objs := $(HDF_WIFI_VENDOR_ROOT)/adapter/hdf_wlan_sdio_adapt.o

ccflags-$(CONFIG_DRIVERS_HDF_WIFI) += \
    $(HDF_FRAMEWORKS_INC) \
    $(HDF_WIFI_FRAMEWORKS_INC) \
    $(HDF_WIFI_ADAPTER_INC) \
    $(HDF_WIFI_VENDOR_INC) \
    $(SECURE_LIB_INC)

obj-$(CONFIG_DRIVERS_HI3881) += hi3881/

ifneq ($(CONFIG_AP6XXX_WIFI6_HDF),)
RKWIFI_PATH := $(HDF_VENDOR_PREFIX)/device/hihope/drivers/rkwifi
obj-$(CONFIG_AP6XXX_WIFI6_HDF) += $(RKWIFI_PATH)/
endif
