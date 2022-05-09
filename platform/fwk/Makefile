#
# Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

include drivers/hdf/khdf/platform/platform.mk

obj-y  += $(HDF_PLATFORM_FRAMEWORKS_ROOT)/src/fwk/platform_common.o \
          $(HDF_PLATFORM_FRAMEWORKS_ROOT)/src/fwk/platform_device.o \
          $(HDF_PLATFORM_FRAMEWORKS_ROOT)/src/fwk/platform_event.o \
	  $(HDF_PLATFORM_FRAMEWORKS_ROOT)/src/fwk/platform_manager.o \
	  $(HDF_PLATFORM_FRAMEWORKS_ROOT)/src/fwk/platform_queue.o \
	  $(HDF_PLATFORM_FRAMEWORKS_ROOT)/src/fwk/platform_dumper.o

ifeq ($(CONFIG_DRIVERS_HDF_PLATFORM_TRACE), y)
obj-y  += $(HDF_PLATFORM_FRAMEWORKS_ROOT)/src/fwk/platform_trace_transfer.o \
	  ./platform_trace.o
endif

obj-y  += ./plat_common.o
