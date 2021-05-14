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


DISPLAY_ROOT_DIR = ../../../../../framework/model/display/driver
obj-y += \
         $(DISPLAY_ROOT_DIR)/hdf_disp.o \
         $(DISPLAY_ROOT_DIR)/lcd_abs_if.o
obj-$(CONFIG_ARCH_HI3516DV300) += \
         $(DISPLAY_ROOT_DIR)/hi35xx_disp.o
obj-$(CONFIG_DRIVERS_HDF_LCDKIT) += \
         $(DISPLAY_ROOT_DIR)/lcdkit/lite_lcdkit.o \
         $(DISPLAY_ROOT_DIR)/lcdkit/lcdkit_parse_config.o
obj-$(CONFIG_DRIVERS_HDF_LCD_ICN9700) += \
         $(DISPLAY_ROOT_DIR)/panel/mipi_icn9700.o
obj-$(CONFIG_DRIVERS_HDF_LCD_ST7789) += \
         $(DISPLAY_ROOT_DIR)/panel/ssp_st7789.o

ccflags-y += -lm -lc -lgcc \
             -Idrivers/hdf/framework/model/display/driver \
             -Idrivers/hdf/framework/include/utils \
             -Idrivers/hdf/khdf/osal/include \
             -Idrivers/hdf/framework/include/core \
             -Idrivers/hdf/framework/core/common/include/host \
             -Idrivers/hdf/framework/ability/sbuf/include \
             -Idrivers/hdf/../../../../../../../../../../../../../third_party/bounds_checking_function/include \
             -Idrivers/hdf/framework/include \
             -Idrivers/hdf/framework/include/osal \
             -Idrivers/hdf/framework/include/platform \
             -Idrivers/hdf/framework/include/config
