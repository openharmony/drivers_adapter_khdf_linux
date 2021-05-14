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

SEC_LIB_SOURCES = memcpy_s.c memmove_s.c memset_s.c securecutil.c secureinput_a.c secureprintoutput_a.c snprintf_s.c sprintf_s.c  strcat_s.c strcpy_s.c strncat_s.c strncpy_s.c strtok_s.c  vsnprintf_s.c vsprintf_s.c 

PWD:=$(shell pwd)
SEC_OBJECTS:=$(patsubst %.c,%.o,$(SEC_LIB_SOURCES))

INPUT_ROOT_DIR = ../../../../../framework/model/input/driver

obj-$(CONFIG_DRIVERS_HDF_INPUT) += \
               $(INPUT_ROOT_DIR)/input_bus_ops/input_i2c_ops.o \
               $(INPUT_ROOT_DIR)/hdf_input_device_manager.o \
               $(INPUT_ROOT_DIR)/input_config_parser.o \
               $(INPUT_ROOT_DIR)/event_hub.o \
               $(INPUT_ROOT_DIR)/hdf_touch.o \
               $(INPUT_ROOT_DIR)/hdf_key.o \
               $(INPUT_ROOT_DIR)/hdf_hid.o

obj-$(CONFIG_DRIVERS_HDF_TP_5P5_GT911) += \
              $(INPUT_ROOT_DIR)/touchscreen/touch_gt911.o
obj-$(CONFIG_DRIVERS_HDF_TP_2P35_FT6236) += \
              $(INPUT_ROOT_DIR)/touchscreen/touch_ft6336.o

ccflags-y += -Iinclude/../drivers/hdf/framework/model/input/driver \
             -Iinclude/../drivers/hdf/framework/model/input/driver/input_bus_ops \
             -Iinclude/../drivers/hdf/framework/include/core \
             -Iinclude/../drivers/hdf/framework/core/common/include/host \
             -Iinclude/../drivers/hdf/framework/include/utils \
             -Iinclude/../drivers/hdf/framework/include/osal \
             -Iinclude/../drivers/hdf/framework/ability/sbuf/include \
             -Iinclude/../drivers/hdf/framework/include/platform \
             -Iinclude/../drivers/hdf/framework/include/config \
             -Iinclude/../drivers/hdf/framework/core/host/include \
             -Iinclude/../drivers/hdf/framework/core/shared/include \
             -Iinclude/../drivers/hdf/khdf/osal/include \
             -Iinclude/../drivers/hdf/../../../../../../../../../../../../../third_party/bounds_checking_function/include \
             -Iinclude/../drivers/hdf/../../../../../../../../../../../../../third_party/FreeBSD/sys/dev/evdev
