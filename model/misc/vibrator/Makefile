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


ccflags-y += -Idrivers/hdf/framework/model/misc/vibrator/driver/include \
             -Idrivers/hdf/framework/model/misc/vibrator/driver/chipset \
             -Idrivers/hdf/framework/include/core \
             -Idrivers/hdf/framework/core/common/include/host \
             -Idrivers/hdf/framework/include/utils \
             -Idrivers/hdf/framework/include/osal \
             -Idrivers/hdf/framework/ability/sbuf/include \
             -Idrivers/hdf/framework/include/platform \
             -Idrivers/hdf/framework/include/config \
             -Idrivers/hdf/framework/core/host/include \
             -Idrivers/hdf/framework/core/shared/include \
             -Idrivers/hdf/framework/utils/include \
             -Idrivers/hdf/khdf/osal/include \
             -Ibounds_checking_function/include
