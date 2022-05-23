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

HDF_FRAMEWORK_TEST_ROOT = drivers/hdf/framework/test/unittest
HDF_FRAMEWORK_ROOT = drivers/hdf/framework
HDF_AUDIO_ADM_TEST_INC_DIR = drivers/hdf/framework/model/audio
HDF_AUDIO_DRIVER_TEST_ROOT = ../../../../../device/board/hisilicon/hispark_taurus/audio_drivers/unittest
HDF_AUDIO_DRIVER_TEST_INC_DIR = drivers/hdf/framework/../../device/board/hisilicon/hispark_taurus/audio_drivers/unittest
#$(error HDF_FRAMEWORK_ROOT is $(HDF_FRAMEWORK_ROOT))

ccflags-$(CONFIG_DRIVERS_HDF_TEST) += -I$(srctree)/drivers/hdf/framework/include/platform \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/support/platform/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/support/platform/include/fwk \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/support/platform/include/rtc \
    -I$(srctree)/include/hdf \
    -I$(srctree)/include/hdf/osal \
    -I$(srctree)/include/hdf/utils \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/common \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/manager \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/osal \
    -I$(srctree)/drivers/hdf/khdf/test/adapter/osal/include \
    -I$(srctree)/drivers/hdf/khdf/include/core \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/osal \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/wifi \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/netdevice \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/module \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/net \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/qos \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/message \
    -I$(srctree)/drivers/hdf/khdf/network/include \
    -I$(srctree)/drivers/hdf/khdf/osal/include \
    -I$(srctree)/drivers/hdf/khdf/test/osal/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/include/utils \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/include/config \
    -I$(srctree)/drivers/hdf/khdf/config/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/core/manager/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/core/host/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/core/shared/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/include/core \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/core/common/include/host \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/utils/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/include/wifi \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/include/net \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/model/network/wifi/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/model/network/common/netdevice \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/model/network/wifi/core/module \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/model/network/wifi/platfrom/src/qos \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/model/network/wifi/core/components/softap \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/model/network/wifi/core/components/sta \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/model/network/wifi/platform/include \
    -I$(srctree)/bounds_checking_function/include \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/platform \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/platform/entry \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/platform/common \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/wifi \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/netdevice \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/module \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/net \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/qos \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/model/network/wifi/unittest/message \
    -I$(srctree)/$(HDF_FRAMEWORK_TEST_ROOT)/sensor \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/model/sensor/driver/include \
    -I$(srctree)/$(HDF_FRAMEWORK_ROOT)/model/sensor/driver/common/include \
    -I$(srctree)/$(HDF_AUDIO_ADM_TEST_INC_DIR)/sapm/include \
    -I$(srctree)/$(HDF_AUDIO_ADM_TEST_INC_DIR)/dispatch/include \
    -I$(srctree)/$(HDF_AUDIO_ADM_TEST_INC_DIR)/core/include \
    -I$(srctree)/$(HDF_AUDIO_ADM_TEST_INC_DIR)/common/include \
    -I$(srctree)/$(HDF_AUDIO_ADM_TEST_INC_DIR)/../../include/audio \
    -I$(srctree)/$(HDF_AUDIO_ADM_TEST_INC_DIR)/../../test/unittest/model/audio/include \
    -I$(srctree)/$(HDF_AUDIO_DRIVER_TEST_INC_DIR)/include \
    -I$(srctree)/$(HDF_AUDIO_DRIVER_TEST_INC_DIR)/../codec/hi3516/include \
    -I$(srctree)/$(HDF_AUDIO_DRIVER_TEST_INC_DIR)/../soc/include \
    -I$(srctree)/$(HDF_AUDIO_DRIVER_TEST_INC_DIR)/../include
