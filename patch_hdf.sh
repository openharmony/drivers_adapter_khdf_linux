#!/bin/bash
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

set -e

OHOS_SOURCE_ROOT=$1
KERNEL_BUILD_ROOT=$2
HDF_PATCH_FILE=$3

ln_list=(
    $OHOS_SOURCE_ROOT/drivers/adapter/khdf/linux    drivers/hdf/khdf
    $OHOS_SOURCE_ROOT/drivers/framework             drivers/hdf/framework
    $OHOS_SOURCE_ROOT/drivers/framework/include     include/hdf
)

cp_list=(
    $OHOS_SOURCE_ROOT/third_party/bounds_checking_function  ./
    $OHOS_SOURCE_ROOT/device/hisilicon/drivers/wifi         drivers/hdf/
    $OHOS_SOURCE_ROOT/third_party/FreeBSD/sys/dev/evdev     drivers/hdf/
)

function copy_external_compents
{
    for ((i=0; i<${#cp_list[*]}; i+=2))
    do
        dst_dir=${cp_list[`expr $i + 1`]}/${cp_list[$i]##*/}
        mkdir -p $dst_dir
        cp -arfL ${cp_list[$i]}/* $dst_dir/
    done
}

function ln_hdf_repos
{
    for ((i=0; i<${#ln_list[*]}; i+=2))
    do
        ln -sf ${ln_list[$i]} ${ln_list[`expr $i + 1`]}
    done
}

function main
{
    cd $KERNEL_BUILD_ROOT
    patch -p1 < $HDF_PATCH_FILE
    ln_hdf_repos
    copy_external_compents
    cd -
}

main
