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

HDF_PLATFORM_FRAMEWORKS_ROOT = ../../../../../framework/support/platform
ccflags-$(CONFIG_DRIVERS_HDF_PLATFORM) +=-I$(srctree)/drivers/hdf/framework/include/platform \
   -I$(srctree)/drivers/hdf/framework/support/platform/include \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/fwk \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/adc \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/dma \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/gpio \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/hdmi \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/i2c \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/i2s \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/i3c \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/mipi \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/pwm \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/rtc \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/spi \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/uart \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/watchdog \
   -I$(srctree)/drivers/hdf/framework/support/platform/include/regulator \
   -I$(srctree)/drivers/hdf/framework/model/storage/include \
   -I$(srctree)/drivers/hdf/framework/model/storage/include/mmc \
   -I$(srctree)/drivers/hdf/framework/model/storage/include/mtd \
   -I$(srctree)/include/hdf \
   -I$(srctree)/include/hdf/osal \
   -I$(srctree)/include/hdf/utils \
   -I$(srctree)/drivers/hdf/khdf/osal/include \
   -I$(srctree)/drivers/hdf/framework/include \
   -I$(srctree)/drivers/hdf/framework/include/utils \
   -I$(srctree)/drivers/hdf/framework/include/config \
   -I$(srctree)/drivers/hdf/khdf/config/include \
   -I$(srctree)/drivers/hdf/framework/core/manager/include \
   -I$(srctree)/drivers/hdf/framework/core/host/include \
   -I$(srctree)/drivers/hdf/framework/core/shared/include \
   -I$(srctree)/drivers/hdf/framework/include/core \
   -I$(srctree)/drivers/hdf/framework/core/common/include/host \
   -I$(srctree)/drivers/hdf/framework/utils/include \
   -I$(srctree)/bounds_checking_function/include
