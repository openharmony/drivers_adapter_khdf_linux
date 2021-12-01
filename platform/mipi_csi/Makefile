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

include drivers/hdf/khdf/platform/platform.mk

obj-y  += $(HDF_PLATFORM_FRAMEWORKS_ROOT)/src/mipi/mipi_csi_core.o \
          $(HDF_PLATFORM_FRAMEWORKS_ROOT)/src/mipi/mipi_csi_if.o
obj-$(CONFIG_VIDEO_V4L2) += mipi_v4l2_adapter.o

obj-y  += mipi_csi_dev.o \

EXTRA_CFLAGS += -Wno-error=date-time
