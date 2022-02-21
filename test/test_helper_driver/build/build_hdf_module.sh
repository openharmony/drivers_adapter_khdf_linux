#!/bin/bash
# Copyright (c) 2022 Huawei Device Co., Ltd.
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

set -e

OHOS_ROOT_PATH=$1
SRCDIR=$2
OUTDIR=$3
COMPILER_DIR=$4

export KERNEL_VERSION="$5"
export OHOS_ROOT_PATH
export COMPILER_PATH_DIR=${COMPILER_DIR}/bin


echo "KERNEL_VERSION=${KERNEL_VERSION}"
echo "OHOS_ROOT_PATH=${OHOS_ROOT_PATH}"
echo "COMPILER_PATH_DIR=${COMPILER_PATH_DIR}"
echo "SRCDIR=${SRCDIR}"
echo "OUTDIR=${OUTDIR}"


pushd ${SRCDIR} && make clean && make -j && popd
mkdir -p ${OUTDIR}; mv ${SRCDIR}/*.ko ${OUTDIR}/;
pushd ${SRCDIR} && make clean && popd
