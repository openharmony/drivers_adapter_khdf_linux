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
CURRENT_DIR := $(abspath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))

obj-$(CONFIG_DRIVERS_HDF) += osal/
obj-$(CONFIG_DRIVERS_HDF) += network/
obj-$(CONFIG_DRIVERS_HDF) += utils/

$(warning PRODUCT_PATH=$(PRODUCT_PATH))
ifeq ($(PRODUCT_PATH),)
$(error PRODUCT_PATH is not set)
endif

HCS_DIR := ../../../../$(PRODUCT_PATH)/hdf_config/khdf
ifeq ($(wildcard $(CURRENT_DIR)/$(HCS_DIR)),)
HCS_DIR := ../../../../$(PRODUCT_PATH)/hdf_config
endif

ifeq ($(CONFIG_DRIVERS_HDF), y)
ifeq ($(wildcard $(CURRENT_DIR)/$(HCS_DIR)),)
HCS_ABS_DIR := $(abspath $(CURRENT_DIR)/$(HCS_DIR)
$(error miss hcs config in $(HCS_ABS_DIR) for small system\
 or $(HCS_ABS_DIR)/khdf for standrad system)
endif

ifeq ($(CONFIG_DRIVERS_HDF_TEST), y)
obj-$(CONFIG_DRIVERS_HDF_TEST) += test/
obj-$(CONFIG_DRIVERS_HDF_TEST) += $(HCS_DIR)/hdf_test/
else
obj-$(CONFIG_DRIVERS_HDF) += $(HCS_DIR)/
endif
endif

obj-$(CONFIG_DRIVERS_HDF) += manager/
obj-$(CONFIG_DRIVERS_HDF_PLATFORM) += platform/
obj-$(CONFIG_DRIVERS_HDF_DISP) += model/display/
obj-$(CONFIG_DRIVERS_HDF_INPUT) += model/input/
obj-$(CONFIG_DRIVERS_HDF_WIFI) += model/network/wifi/
obj-$(CONFIG_DRIVERS_HDF_USB_PNP_NOTIFY) += model/usb/host/
obj-$(CONFIG_DRIVERS_HDF_USB_F_GENERIC) += model/usb/device/
obj-$(CONFIG_DRIVERS_HDF_SENSOR) += model/sensor/
obj-$(CONFIG_DRIVERS_HDF_STORAGE) += model/storage/
obj-$(CONFIG_DRIVERS_HDF_BT) += model/network/bluetooth/
obj-$(CONFIG_DRIVERS_HDF_VIBRATOR) += model/misc/vibrator/
obj-$(CONFIG_DRIVERS_HDF_AUDIO) += model/audio/
obj-$(CONFIG_DRIVERS_HDF_DSOFTBUS) += model/misc/dsoftbus/
