#
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
#
#

VIBRATOR_ROOT_DIR = ../../../../../../framework/model/misc/vibrator/driver

obj-$(CONFIG_DRIVERS_HDF_VIBRATOR) += \
               $(VIBRATOR_ROOT_DIR)/src/vibrator_driver.o \
               $(VIBRATOR_ROOT_DIR)/src/vibrator_haptic.o
obj-$(CONFIG_DRIVERS_HDF_VIBRATOR_LINEAR) += \
              $(VIBRATOR_ROOT_DIR)/chipset/vibrator_linear_driver.o


ccflags-y +=-I$(srctree)/drivers/hdf/framework/model/misc/vibrator/driver/include \
            -I$(srctree)/drivers/hdf/framework/model/misc/vibrator/driver/chipset \
            -I$(srctree)/drivers/hdf/framework/include/core \
            -I$(srctree)/drivers/hdf/framework/core/common/include/host \
            -I$(srctree)/drivers/hdf/framework/include/utils \
            -I$(srctree)/drivers/hdf/framework/include/osal \
            -I$(srctree)/drivers/hdf/framework/include/platform \
            -I$(srctree)/drivers/hdf/framework/include/config \
            -I$(srctree)/drivers/hdf/framework/core/host/include \
            -I$(srctree)/drivers/hdf/framework/core/shared/include \
            -I$(srctree)/drivers/hdf/framework/utils/include \
            -I$(srctree)/drivers/hdf/khdf/osal/include \
            -I$(srctree)/bounds_checking_function/include
