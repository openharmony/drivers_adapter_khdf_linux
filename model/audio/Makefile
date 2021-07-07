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

KHDF_AUDIO_BASE_ROOT_DIR = ../../../../../..
KHDF_AUDIO_KHDF_ROOT_DIR = ../drivers/hdf/khdf
KHDF_FRAMEWORK_ROOT_DIR = ../drivers/hdf/framework
KHDF_AUDIO_ROOT_DIR = ../../../../../framework/model/audio
KHDF_AUDIO_PERIPHERAL_ROOT_DIR = $(KHDF_AUDIO_BASE_ROOT_DIR)/drivers/peripheral
KHDF_AUDIO_CODEC_DIR = $(KHDF_AUDIO_PERIPHERAL_ROOT_DIR)/audio/chipsets

obj-y += \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_core.o \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_host.o \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_parse.o \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/codec_core.o \
          $(KHDF_AUDIO_ROOT_DIR)/sapm/src/audio_sapm.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/soc/src/platform.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/soc/src/dai.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/soc/src/dsp.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/codec/src/codec.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/accessory/src/accessory.o \
          $(KHDF_AUDIO_ROOT_DIR)/dispatch/src/audio_stream_dispatch.o \
          $(KHDF_AUDIO_ROOT_DIR)/dispatch/src/audio_control_dispatch.o \
          $(KHDF_AUDIO_CODEC_DIR)/codec/hi3516_codec/src/hi3516_codec.o \
          $(KHDF_AUDIO_CODEC_DIR)/codec/hi3516_codec/src/hi3516_aiao.o \
          $(KHDF_AUDIO_CODEC_DIR)/accessory/tfa9879_smartPA/src/tfa9879_codec.o

obj-$(CONFIG_ARCH_HI3516DV300) += \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_core.o \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_host.o \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_parse.o \
          $(KHDF_AUDIO_ROOT_DIR)/core/src/codec_core.o \
          $(KHDF_AUDIO_ROOT_DIR)/sapm/src/audio_sapm.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/soc/src/platform.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/soc/src/dai.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/soc/src/dsp.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/codec/src/codec.o \
          $(KHDF_AUDIO_ROOT_DIR)/device/accessory/src/accessory.o \
          $(KHDF_AUDIO_ROOT_DIR)/dispatch/src/audio_stream_dispatch.o \
          $(KHDF_AUDIO_ROOT_DIR)/dispatch/src/audio_control_dispatch.o \
          $(KHDF_AUDIO_CODEC_DIR)/codec/hi3516_codec/src/hi3516_codec.o \
          $(KHDF_AUDIO_CODEC_DIR)/codec/hi3516_codec/src/hi3516_aiao.o \
          $(KHDF_AUDIO_CODEC_DIR)/accessory/tfa9879_smartPA/src/tfa9879_codec.o

ccflags-y += -lm -lc  -lgcc -std=gnu99 -Werror \
             -Iinclude/$(KHDF_AUDIO_BASE_ROOT_DIR)/drivers/framework/include/platform \
             -Iinclude/$(KHDF_AUDIO_BASE_ROOT_DIR)/utils/native/base/include \
             -Iinclude/$(KHDF_AUDIO_BASE_ROOT_DIR)/third_party/bounds_checking_function/include \
             -Iinclude/$(KHDF_AUDIO_BASE_ROOT_DIR)/third_party/FreeBSD/sys/dev/evdev \
             -Iinclude/$(KHDF_AUDIO_KHDF_ROOT_DIR)/osal/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/support/platform/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/utils/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/core \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/core/common/include/host \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/utils \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/utils \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/osal \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/ability/sbuf/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/platform \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/include/config \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/core/host/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/core/shared/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/core/manager/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/core/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/sapm/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/dispatch/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/device/accessory/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/device/codec/include \
             -Iinclude/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/device/soc/include \
             -Iinclude/$(KHDF_AUDIO_CODEC_DIR)/codec/hi3516_codec/include \
             -Iinclude/$(KHDF_AUDIO_CODEC_DIR)/accessory/tfa9879_smartPA/include \
             -Iinclude/$(KHDF_AUDIO_PERIPHERAL_ROOT_DIR)/audio/interfaces/include \
             -Iinclude/$(KHDF_AUDIO_CODEC_DIR)/include
