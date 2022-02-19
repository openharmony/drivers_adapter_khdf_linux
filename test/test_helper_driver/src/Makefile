# Copyright (c) 2022 Huawei Device Co., Ltd.
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

CROSS := arm-linux-gnueabi-
COMPILER_PATH := $(COMPILER_PATH_DIR)
CROSS_TOOLCHAIN_PATH := $(OHOS_ROOT_PATH)/prebuilts/gcc/linux-x86/arm/gcc-linaro-7.5.0-arm-linux-gnueabi/bin
CLANG_HOST_TOOLCHAIN := $(OHOS_ROOT_PATH)/prebuilts/clang/ohos/linux-x86_64/llvm/bin
HOST_CC := $(CLANG_HOST_TOOLCHAIN)/clang
AR := $(CROSS_PATH)/$(CROSS)ar

export KERNEL_ROOT?=$(OHOS_ROOT_PATH)/out/KERNEL_OBJ/kernel/src_tmp/$(KERNEL_VERSION)
export KBUILD_OUTPUT=$(OHOS_ROOT_PATH)/out/KERNEL_OBJ/kernel/OBJ/$(KERNEL_VERSION)

COMPILER_TRIPLE :=
COMPILER_TRIPLE += "CROSS_COMPILE=$(CROSS_TOOLCHAIN_PATH)/$(CROSS)"
COMPILER_TRIPLE += "CC=$(HOST_CC)"

include $(OHOS_ROOT_PATH)/drivers/adapter/khdf/linux/test/test_khdf.mk

$(waring build sample_driver module)

obj-m += hdf_test_helper.o
hdf_test_helper-y := test_helper_driver.o


.PHONY: default clean
default:
	@$(MAKE) -C $(KERNEL_ROOT) M=$(PWD) ARCH=arm $(COMPILER_TRIPLE) $(COMPILER_TRIPLE) V=10 modules -j 1

clean:
	@rm -f .*.cmd
	@rm -f *.mod.c
	@rm -f *.o
	@rm -f *.mod
	@rm -f *.symvers
	@rm -f *.order
