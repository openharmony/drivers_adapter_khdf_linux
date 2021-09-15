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

STORAGE_ROOT_DIR = ../../../../../framework/model/storage

obj-$(CONFIG_DRIVERS_HDF_STORAGE) += \
               $(STORAGE_ROOT_DIR)/src/mmc/mmc_block.o \
               ./mmc_block_linux.o

ccflags-y += -Idrivers/hdf/framework/model/storage/include/ \
             -Idrivers/hdf/framework/include/core \
             -Idrivers/hdf/framework/core/common/include/host \
             -Idrivers/hdf/framework/include/utils \
             -Idrivers/hdf/framework/utils/include \
             -Idrivers/hdf/framework/include/osal \
             -Idrivers/hdf/framework/ability/sbuf/include \
             -Idrivers/hdf/framework/include/platform \
             -Idrivers/hdf/framework/support/platform/include/common \
             -Idrivers/hdf/framework/support/platform/include/mmc \
             -Idrivers/hdf/framework/include/config \
             -Idrivers/hdf/khdf/osal/include \
             -Ibounds_checking_function/include
