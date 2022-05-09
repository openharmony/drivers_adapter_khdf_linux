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

obj-$(CONFIG_DRIVERS_HI3881) += hi3881/

ifneq ($(CONFIG_AP6XXX_WIFI6_HDF),)
RKWIFI_PATH := $(HDF_VENDOR_PREFIX)/device/board/$(PRODUCT_NAME)/$(DEVICE_NAME)/wifi
obj-$(CONFIG_AP6XXX_WIFI6_HDF) += $(RKWIFI_PATH)/
endif
