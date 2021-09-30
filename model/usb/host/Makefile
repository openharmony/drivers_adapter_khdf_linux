# Copyright (c) 2021 Huawei Device Co., Ltd.
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

USB_PNP_NOTIFY_ROOT_DIR = ./
obj-$(CONFIG_DRIVERS_HDF_USB_PNP_NOTIFY) += \
         $(USB_PNP_NOTIFY_ROOT_DIR)/src/usb_pnp_notify.o

ccflags-y += -lm -lc -lgcc \
            -I$(srctree)/drivers/hdf/khdf/model/usb/host/include \
            -I$(srctree)/drivers/hdf/framework/include/utils \
            -I$(srctree)/drivers/hdf/framework/utils/include \
            -I$(srctree)/drivers/hdf/khdf/osal/include \
            -I$(srctree)/drivers/hdf/framework/include/core \
            -I$(srctree)/drivers/hdf/framework/core/common/include/host \
            -I$(srctree)/drivers/hdf/framework/core/shared/include \
            -I$(srctree)/drivers/hdf/framework/include \
            -I$(srctree)/drivers/hdf/framework/include/osal \
            -I$(srctree)/drivers/hdf/framework/include/platform \
            -I$(srctree)/drivers/hdf/framework/include/config \
            -I$(srctree)/bounds_checking_function/include
