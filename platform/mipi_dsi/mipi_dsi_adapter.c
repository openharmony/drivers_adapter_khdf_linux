/*
 * mipi_dsi_adapter.c
 *
 * Mipi dsi adapter driver.
 *
 * Copyright (C) Huawei Device Co., Ltd. 2020-2021. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "hdf_base.h"
#include "osal_mem.h"
#include "mipi_dsi_core.h"
#include "mipi_dsi_adapter.h"
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include "securec.h"


#define NAME_LEN 20

static struct miscdevice *g_mipiDsiMiscdev[MAX_CNTLR_CNT];

int MipiDsiRegisterDevice(const char *name, uint32_t id, unsigned short mode, struct file_operations *ops)
{
    int error;
    struct miscdevice *dev = NULL;

    if (name == NULL || ops == NULL || id >= MAX_CNTLR_CNT) {
        return -1;
    }
    dev = OsalMemCalloc(sizeof(struct miscdevice));
    if (dev == NULL) {
        return -1;
    }
    dev->fops = ops;
    dev->name = OsalMemCalloc(NAME_LEN + 1);
    if (dev->name == NULL) {
        OsalMemFree(dev);
        return -1;
    }
    if (id != 0) { /* 0 : id */
        if (snprintf_s((char *)dev->name, NAME_LEN + 1, NAME_LEN, "%s%u", name, id) < 0) {
            OsalMemFree((char *)dev->name);
            OsalMemFree(dev);
            return -1;
        }
    } else {
        if (memcpy_s((char *)dev->name, NAME_LEN, name, strlen(name)) != EOK) {
            OsalMemFree((char *)dev->name);
            OsalMemFree(dev);
            return -1;
        }
    }
    ops->owner = THIS_MODULE;
    dev->minor = MISC_DYNAMIC_MINOR;
    dev->mode = mode;
    error = misc_register(dev);
    if (error < 0) {
        printk("%s: id %u cannot register miscdev on minor=%d (err=%d)\n",
            __func__, id, MISC_DYNAMIC_MINOR, error);
        OsalMemFree((char *)dev->name);
        OsalMemFree(dev);
        return error;
    }
    g_mipiDsiMiscdev[id] = dev;
    printk("mipi_dsi:create inode ok %s %d\n", dev->name, dev->minor);
    return 0;
}

int MipiDsiProcRegister(const char *name, uint32_t id, unsigned short mode, const struct file_operations *ops)
{
    char procName[NAME_LEN + 1];
    struct proc_dir_entry* err = NULL;
    int ret;

    if (name == NULL || ops == NULL || id >= MAX_CNTLR_CNT) {
        return -1;
    }
    if (memset_s(procName, NAME_LEN + 1, 0, NAME_LEN + 1) != EOK) {
        return -1;
    }
    if (id != 0) {
        ret = snprintf_s(procName, NAME_LEN + 1, NAME_LEN, "%s%u", name, id);
    } else {
        ret = snprintf_s(procName, NAME_LEN + 1, NAME_LEN, "%s", name);
    }
    if (ret < 0) {
        printk(KERN_ERR "%s: procName %s snprintf_s fail\n", __func__, procName);
        return -1;
    }
    err = proc_create(procName, mode, NULL, ops);
    if (err == NULL) {
        printk(KERN_ERR "%s: proc_create name %s fail\n", __func__, procName);
        return -1;
    }
    return 0;
}

void MipiDsiUnregisterDevice(uint32_t id)
{
    if (id >= MAX_CNTLR_CNT) {
        return;
    }
    misc_deregister(g_mipiDsiMiscdev[id]);
    OsalMemFree((void *)g_mipiDsiMiscdev[id]->name);
    g_mipiDsiMiscdev[id]->name = NULL;
    OsalMemFree(g_mipiDsiMiscdev[id]);
    g_mipiDsiMiscdev[id] = NULL;
}

void MipiDsiProcUnregister(const char *name, uint32_t id)
{
    char procName[NAME_LEN + 1];
    int ret;

    if (id >= MAX_CNTLR_CNT) {
        return;
    }
    memset_s(procName, NAME_LEN + 1, 0, NAME_LEN + 1);
    if (id != 0) {
        ret = snprintf_s(procName, NAME_LEN + 1, NAME_LEN, "%s%u", name, id);
    } else {
        ret = snprintf_s(procName, NAME_LEN + 1, NAME_LEN, "%s", name);
    }
    if (ret < 0) {
        printk(KERN_ERR "%s: procName %s snprintf_s fail\n", __func__, procName);
        return;
    }
    remove_proc_entry(procName, NULL);
}
