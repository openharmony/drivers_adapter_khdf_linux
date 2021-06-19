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

export HDF_ROOT := drivers/hdf
obj-$(CONFIG_DRIVERS_HDF)  += osal/
obj-$(CONFIG_DRIVERS_HDF)  += network/
obj-$(CONFIG_DRIVERS_HDF)  += config/


SUPPORT_LEVEL_STD_H := $(shell [[ "$(CONFIG_HDF_SUPPORT_LEVEL)" -ge 2 ]] && echo true)

ifneq ($(SUPPORT_LEVEL_STD_H), true) # for L1
ifeq ($(CONFIG_DRIVERS_HDF_TEST), y)
obj-y  += ../../../../$(PRODUCT_PATH)/config/hdf_test/
obj-y  += test/
else
obj-$(CONFIG_DRIVERS_HDF)  += ../../../../$(PRODUCT_PATH)/config/
endif
else # for L2+
ifeq ($(CONFIG_DRIVERS_HDF_TEST), y)
obj-y  += hcs/hdf_test/
obj-y  += test/
else
obj-$(CONFIG_DRIVERS_HDF)  += hcs/
endif
endif

obj-$(CONFIG_DRIVERS_HDF)  += manager/
obj-$(CONFIG_DRIVERS_HDF_PLATFORM) += platform/
obj-$(CONFIG_DRIVERS_HDF_DISP) += model/display/
obj-$(CONFIG_DRIVERS_HDF_INPUT)  += model/input/
obj-$(CONFIG_DRIVERS_HDF_WIFI) += model/network/wifi/
obj-$(CONFIG_DRIVERS_HDF_SENSOR)  += model/sensor/
obj-$(CONFIG_DRIVERS_HDF_STORAGE) += model/storage/
obj-$(CONFIG_DRIVERS_HDF_BT) += model/network/bluetooth/
