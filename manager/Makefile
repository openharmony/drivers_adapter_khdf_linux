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

OHOS_SOURCE_ROOT := ../../../../../
HDF_FRAMEWORK_SRC_DIR := ../../../../

SEC_LIB_SOURCES = memcpy_s.c memmove_s.c memset_s.c securecutil.c secureinput_a.c secureprintoutput_a.c snprintf_s.c \
                  sprintf_s.c strcat_s.c strcpy_s.c strncat_s.c strncpy_s.c strtok_s.c  vsnprintf_s.c vsprintf_s.c
SEC_FUNCTION_OBJECTS := $(patsubst %.c,%.o,$(SEC_LIB_SOURCES))
SEC_FUNCTION_OBJECTS := $(addprefix $(OHOS_SOURCE_ROOT)/third_party/bounds_checking_function/src/,${SEC_FUNCTION_OBJECTS})

obj-y += \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/utils/src/hdf_cstring.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/utils/src/hdf_slist.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/utils/src/hdf_sref.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/utils/src/hdf_map.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/utils/src/hdf_sbuf.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/utils/src/hdf_sbuf_impl_raw.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/utils/src/hdf_task_queue.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/devhost_service.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/devmgr_service_clnt.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/devsvc_manager_clnt.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/hdf_device.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/hdf_device_node.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/hdf_device_token.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/hdf_driver_loader.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/hdf_observer_record.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/hdf_service_subscriber.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/hdf_device_object.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/hdf_power_manager.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/hdf_service_observer.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/host/src/power_state_token.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/devhost_service_clnt.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/device_token_clnt.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/devmgr_service.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/devsvc_manager.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/devsvc_manager_ext.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/servstat_listener_holder.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/hdf_driver_installer.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/hdf_host_info.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/power_state_token_clnt.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/manager/src/driver_manager.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/shared/src/hdf_service_record.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/shared/src/hdf_device_info.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/shared/src/hdf_object_manager.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/shared/src/hdf_io_service.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/shared/src/service_status.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/common/src/devlite_object_config.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/common/src/hdf_attribute.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/common/src/devmgr_service_start.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/common/src/hdf_device_node_ext.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/common/src/buildin_config_entry.o \
        $(HDF_FRAMEWORK_SRC_DIR)/framework/core/adapter/vnode/src/hdf_vnode_adapter.o \
        ./src/devmgr_load.o \
        ./src/devmgr_pm_reg.o \
        ./src/sym_export.o \
        ./src/hdf_kevent.o \
        $(SEC_FUNCTION_OBJECTS)

ccflags-y += \
       -I$(srctree)/drivers/hdf/framework/include \
       -I$(srctree)/drivers/hdf/framework/include/osal \
       -I$(srctree)/drivers/hdf/framework/include/utils \
       -I$(srctree)/drivers/hdf/framework/include \
       -I$(srctree)/drivers/hdf/framework/utils/include \
       -I$(srctree)/drivers/hdf/framework/include/core \
       -I$(srctree)/drivers/hdf/framework/core/host/include \
       -I$(srctree)/drivers/hdf/framework/core/manager/include \
       -I$(srctree)/drivers/hdf/framework/core/shared/include \
       -I$(srctree)/drivers/hdf/framework/core/common/include/host \
       -I$(srctree)/drivers/hdf/framework/core/common/include/manager \
       -I$(srctree)/drivers/hdf/framework/core/adapter/vnode/include \
       -I$(srctree)/drivers/hdf/framework/include/config \
       -I$(srctree)/drivers/hdf/khdf/osal/include \
       -I$(srctree)/include/uapi/linux \
       -I$(srctree)/bounds_checking_function/include
