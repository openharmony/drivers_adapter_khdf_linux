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

include drivers/hdf/khdf/platform/platform.mk

STORAGE_ROOT_DIR = ../../../../../framework/model/storage

ccflags-y +=-I$(srctree)/drivers/hdf/framework/model/storage/include/mmc \
            -I$(srctree)/drivers/hdf/framework/model/storage/include/mtd \
            -I$(srctree)/drivers/hdf/framework/include/core \
            -I$(srctree)/drivers/hdf/framework/core/common/include/host \
            -I$(srctree)/drivers/hdf/framework/include/utils \
            -I$(srctree)/drivers/hdf/framework/utils/include \
            -I$(srctree)/drivers/hdf/framework/include/osal \
            -I$(srctree)/drivers/hdf/framework/include/platform \
            -I$(srctree)/drivers/hdf/framework/support/platform/include/fwk \
            -I$(srctree)/drivers/hdf/framework/model/storage/include/mmc \
            -I$(srctree)/drivers/hdf/framework/include/config \
            -I$(srctree)/drivers/hdf/khdf/osal/include \
            -I$(srctree)/bounds_checking_function/include

obj-$(CONFIG_DRIVERS_HDF_STORAGE) += \
             ./mmc_block_linux.o \
             $(STORAGE_ROOT_DIR)/src/mmc/mmc_block.o \
             $(STORAGE_ROOT_DIR)/src/mmc/mmc_core.o \
             $(STORAGE_ROOT_DIR)/src/mmc/mmc_emmc.o \
             $(STORAGE_ROOT_DIR)/src/mmc/mmc_sdio.o \
             $(STORAGE_ROOT_DIR)/src/mmc/mmc_dispatch.o \
             $(STORAGE_ROOT_DIR)/src/mmc/mmc_if.o \
             $(STORAGE_ROOT_DIR)/src/mmc/mmc_protocol.o

obj-y  += $(STORAGE_ROOT_DIR)/src/mmc/emmc_if.o \
          $(STORAGE_ROOT_DIR)/src/mmc/sdio_if.o \
          ./emmc_adapter.o \
		  ./sdio_adapter.o

ifeq ($(CONFIG_ARCH_HI3516DV300), y)
    obj-y  += \
	        ./vendor_adapter/hi35xx_mmc_adapter.o
else
    obj-y  += \
	        ./mmc_adapter.o
endif