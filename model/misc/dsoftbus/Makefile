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

DSOFTBUS_ROOT_DIR = ../../../../../../framework/model/misc/dsoftbus

obj-$(CONFIG_DRIVERS_HDF_DSOFTBUS) += \
               $(DSOFTBUS_ROOT_DIR)/src/hdf_dsoftbus_driver.o \
               $(DSOFTBUS_ROOT_DIR)/src/module_manager.o
ifeq ($(CONFIG_DSOFTBUS_WLAN_PARAM_MONITOR), y)
obj-$(CONFIG_DRIVERS_HDF_DSOFTBUS) += $(DSOFTBUS_ROOT_DIR)/src/wlan_param_monitor.o
ccflags-$(CONFIG_DRIVERS_HDF_DSOFTBUS) +=-DENABLE_WLAN_PARAM_MONITOR
endif

ccflags-y += -Idrivers/hdf/framework/model/misc/dsoftbus/include \
             -Idrivers/hdf/framework/core/common/include/host \
             -Idrivers/hdf/framework/include/core \
             -Idrivers/hdf/framework/include/osal \
             -Idrivers/hdf/framework/include/utils \
             -Idrivers/hdf/framework/ability/sbuf/include \
             -Idrivers/hdf/framework/include/platform \
             -Idrivers/hdf/framework/include/config \
             -Idrivers/hdf/framework/core/host/include \
             -Idrivers/hdf/framework/core/shared/include \
             -Idrivers/hdf/framework/utils/include \
             -Idrivers/hdf/khdf/osal/include \
             -Ibounds_checking_function/include
