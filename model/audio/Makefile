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

KHDF_AUDIO_BASE_ROOT_DIR = ../../../../../..
KHDF_AUDIO_KHDF_ROOT_DIR = ../drivers/hdf/khdf
KHDF_FRAMEWORK_ROOT_DIR = ../drivers/hdf/framework
KHDF_AUDIO_ROOT_DIR = ../../../../../framework/model/audio
KHDF_AUDIO_HI3516DV300_DIR = $(KHDF_AUDIO_BASE_ROOT_DIR)/drivers/peripheral/audio/chipsets/hi3516dv300

obj-y += \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_core.o \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_host.o \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_parse.o \
          $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_accessory_base.o \
          $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_codec_base.o \
          $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_dsp_base.o \
          $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_platform_base.o \
          $(KHDF_AUDIO_ROOT_DIR)/sapm/src/audio_sapm.o \
          $(KHDF_AUDIO_ROOT_DIR)/dispatch/src/audio_stream_dispatch.o \
          $(KHDF_AUDIO_ROOT_DIR)/dispatch/src/audio_control_dispatch.o

obj-$(CONFIG_ARCH_HI3516DV300) += \
          $(KHDF_AUDIO_HI3516DV300_DIR)/../tfa9879/accessory/src/tfa9879_accessory_adapter.o \
          $(KHDF_AUDIO_HI3516DV300_DIR)/../tfa9879/accessory/src/tfa9879_accessory_impl.o \
          $(KHDF_AUDIO_HI3516DV300_DIR)/codec/src/hi3516_codec_adapter.o \
          $(KHDF_AUDIO_HI3516DV300_DIR)/codec/src/hi3516_codec_impl.o \
          $(KHDF_AUDIO_HI3516DV300_DIR)/codec/src/hi3516_codec_ops.o \
          $(KHDF_AUDIO_HI3516DV300_DIR)/dsp/src/dsp_adapter.o \
          $(KHDF_AUDIO_HI3516DV300_DIR)/soc/src/hi3516_dai_adapter.o \
          $(KHDF_AUDIO_HI3516DV300_DIR)/soc/src/hi3516_aiao_impl.o \
          $(KHDF_AUDIO_HI3516DV300_DIR)/soc/src/hi3516_platform_ops.o \
          $(KHDF_AUDIO_HI3516DV300_DIR)/soc/src/hi3516_platform_adapter.o

ccflags-y += -lm -lc -lgcc -std=gnu99 -Werror \
             -Iinclude/$(KHDF_AUDIO_BASE_ROOT_DIR)/third_party/bounds_checking_function/include \
	     -Iinclude/$(KHDF_AUDIO_BASE_ROOT_DIR)/utils/native/base/include \
             -Iinclude/$(KHDF_AUDIO_KHDF_ROOT_DIR)/osal/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/core \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/utils \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/osal \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/platform \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/config \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/audio \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/ability/sbuf/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/core/common/include/host \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/core/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/sapm/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/dispatch/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/common/include \
             -Iinclude/$(KHDF_AUDIO_HI3516DV300_DIR)/codec/include \
             -Iinclude/$(KHDF_AUDIO_HI3516DV300_DIR)/soc/include \
             -Iinclude/$(KHDF_AUDIO_HI3516DV300_DIR)/include \
             -Iinclude/$(KHDF_AUDIO_HI3516DV300_DIR)/../tfa9879/accessory/include
