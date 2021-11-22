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

obj-y  += ../../../../framework/utils/src/hcs_parser/device_resource_if.o \
          ../../../../framework/utils/src/hcs_parser/hcs_blob_if.o \
          ../../../../framework/utils/src/hcs_parser/hcs_generate_tree.o \
          ../../../../framework/utils/src/hcs_parser/hcs_buildin_parser.o \
          ../../../../framework/utils/src/hcs_parser/hcs_parser.o \
          ../../../../framework/utils/src/hcs_parser/hcs_tree_if.o
ccflags-y +=-I$(srctree)/include/hdf \
            -I$(srctree)/include/hdf/osal \
            -I$(srctree)/include/hdf/utils \
            -I$(srctree)/drivers/hdf/khdf/osal/include \
            -I$(srctree)/drivers/hdf/framework/include \
            -I$(srctree)/drivers/hdf/framework/include/utils \
            -I$(srctree)/drivers/hdf/framework/include/config \
            -I$(srctree)/drivers/hdf/framework/utils/include