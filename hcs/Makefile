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

ifneq ($(findstring $(shell uname),Linux),)
HCGEN_PATH := linux-x86/bin/hc-gen
else
HCGEN_PATH := win-x86/bin/hc-gen.exe
endif

SOURCE_ROOT:=$(HDF_PROJECT_ROOT)
HC_GEN := hc-gen
BUILD_IN_HC_GEN := $(SOURCE_ROOT)/prebuilts/build-tools/$(HCGEN_PATH)
ifneq ($(wildcard $(BUILD_IN_HC_GEN)),)
HC_GEN := $(BUILD_IN_HC_GEN)
endif

LOCAL_HCS_ROOT := $(abspath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))

HCS_DIR := $(SOURCE_ROOT)/$(product_path)

HCB_FLAGS := -b -i -a

HCS_OBJ := hdf_hcs_hex.o
HCS_OBJ_SRC := $(subst .o,.c,$(notdir $(HCS_OBJ)))

CONFIG_GEN_HEX_SRC := $(addprefix $(LOCAL_HCS_ROOT)/, $(HCS_OBJ_SRC))
CONFIG_HCS_SRC := $(subst _hcs_hex.o,.hcs,$(addprefix $(HCS_DIR)/, $(HCS_OBJ)))

$(obj)/$(HCS_OBJ): $(CONFIG_GEN_HEX_SRC)
	$(Q)$(CC) $(c_flags) -c -o $@ $<
	$(Q)rm -f $<

$(CONFIG_GEN_HEX_SRC):  $(LOCAL_HCS_ROOT)/%_hcs_hex.c: $(HCS_DIR)/%.hcs
	$(Q)echo gen hdf built-in config
	$(Q)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(Q)$(HC_GEN) $(HCB_FLAGS) -o $(subst _hex.c,,$(@)) $<

$(CONFIG_GEN_SRCS): $(CONFIG_OUT_DIR)%.c: $(HCS_DIR)/%.hcs
	$(Q)echo gen hdf driver config
	$(Q)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(Q)$(HC_GEN) -t -o $@ $<


obj-$(CONFIG_DRIVERS_HDF) += $(HCS_OBJ)
