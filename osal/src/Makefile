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

obj-y  += osal_workqueue.o osal_thread.o osal_firmware.o osal_irq.o osal_mem.o osal_mutex.o osal_sem.o osal_spinlock.o \
			osal_time.o osal_file.o osal_timer.o osal_cdev.o osal_deal_log_format.o
ccflags-y +=-I$(srctree)/include/hdf \
			-I$(srctree)/include/hdf/osal \
			-I$(srctree)/include/hdf/utils \
			-I$(srctree)/drivers/hdf/framework/utils/include \
			-I$(srctree)/drivers/hdf/khdf/osal/include \
			-I$(srctree)/bounds_checking_function/include
