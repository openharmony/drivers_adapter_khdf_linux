/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#include "platform_trace.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_file.h"
#include "osal_mem.h"
#include "securec.h"
#include "stdarg.h"
#include <asm/uaccess.h>
#include <linux/fs.h>

#define CREATE_TRACE_POINTS
#include <trace/events/platform_trace_event.h>

#define PLATFORM_TRACE_MSG_MAX_LEN 512
void PlatformTraceAddMsg(const char *module, const char *moduleFun, const char *fmt, ...)
{
    int length;
    va_list argList;
    char messages[PLATFORM_TRACE_MSG_MAX_LEN + 1] = {'\0'};
    if ((module == NULL) || (moduleFun == NULL)) {
        HDF_LOGE("PlatformTraceAddMsg params illegal", module, moduleFun);
        return;
    }

    length = snprintf_s(messages, PLATFORM_TRACE_MSG_MAX_LEN + 1, PLATFORM_TRACE_MSG_MAX_LEN,
        "module:%s function:%s params: ", module, moduleFun);
    if (length < 0) {
        HDF_LOGE("PlatformTraceAddMsg[%s][%s] generate messages fail[%d]!", module, moduleFun, length);
        return;
    }

    va_start(argList, fmt);
    length = vsprintf_s(messages + length - 1, PLATFORM_TRACE_MSG_MAX_LEN, fmt, argList);
    va_end(argList);
    if (length < 0) {
        HDF_LOGE("PlatformTraceAddMsg[%s][%s] generate messages fail[%d]!", module, moduleFun, length);
        return;
    }

    trace_platfrom_trace_record_messages(messages);
}

void PlatformTraceAddUintMsg(int module, int moduleFun, uint infos[], uint8_t size)
{
    int ret;
    int i;
    char messages[PLATFORM_TRACE_MSG_MAX_LEN + 1] = {0};
    const char *moduleMean = PlatformTraceModuleExplainGet(module);
    const char *moduleFunMean = PlatformTraceFunExplainGet(moduleFun);
    if ((moduleMean == NULL) || (moduleFunMean == NULL)) {
        HDF_LOGE("PlatformTraceAddUintMsg module[%d, %d] info illegal", module, moduleFun);
        return;
    }

    if ((size <= 0) || (size > PLATFORM_TRACE_UINT_PARAM_SIZE_MAX)) {
        HDF_LOGE("PlatformTraceAddUintMsg %hhu size illegal", size);
        return;
    }
    if ((module < PLATFORM_TRACE_MODULE_I2S) || (module >= PLATFORM_TRACE_MODULE_MAX)) {
        HDF_LOGE("PlatformTraceAddUintMsg %d module illegal", module);
        return;
    }
    if ((moduleFun < PLATFORM_TRACE_MODULE_I2S_FUN) || (moduleFun >= PLATFORM_TRACE_MODULE_MAX_FUN)) {
        HDF_LOGE("PlatformTraceAddUintMsg %d moduleFun illegal", moduleFun);
        return;
    }

    ret = snprintf_s(messages, PLATFORM_TRACE_MSG_MAX_LEN + 1, PLATFORM_TRACE_MSG_MAX_LEN,
        "module:%s function:%s params:", moduleMean, moduleFunMean);
    if (ret < 0) {
        HDF_LOGE("PlatformTraceAddUintMsg[%s][%s] generate messages fail[%d]!", moduleMean, moduleFunMean, ret);
        return;
    }
    for (i = 0; i < size; i++) {
        snprintf_s(messages + strlen(messages), PLATFORM_TRACE_MSG_MAX_LEN + 1 - strlen(messages),
            PLATFORM_TRACE_MSG_MAX_LEN - strlen(messages),
            "%d, ", infos[i]);
        if ((ret < 0) || (strlen(messages) >= PLATFORM_TRACE_MSG_MAX_LEN)) {
            HDF_LOGE("PlatformTraceAddUintMsg[%s][%s] generate messages fail[%d][%d]!", moduleMean, moduleFunMean, ret, strlen(messages));
            return;
        }
    }

    trace_platfrom_trace_record_messages(messages);
    return;
}

static bool TraceIsOpen()
{
#ifndef CONFIG_TRACING
    return false;
#else
    return true;
#endif
}

static ssize_t TraceFileWrite(OsalFile *file, const void *string, uint32_t length)
{
    ssize_t ret;
    loff_t pos;
    mm_segment_t org_fs;
    struct file *fp = NULL;

    if (file == NULL || IS_ERR_OR_NULL(file->realFile) || string == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    fp = (struct file *)file->realFile;
    pos = fp->f_pos;
    org_fs = get_fs();
    set_fs(KERNEL_DS);

    ret = vfs_write(fp, string, length, &pos);
    set_fs(org_fs);
    if (ret < 0) {
        HDF_LOGE("%s write file length %d fail %d", __func__, length, ret);
        return HDF_FAILURE;
    }

    return ret;
}

static ssize_t TraceFileRead(OsalFile *file, void *buf, uint32_t length)
{
    ssize_t ret;
    mm_segment_t org_fs;
    loff_t pos;
    struct file *fp = NULL;

    if (file == NULL || IS_ERR_OR_NULL(file->realFile) || buf == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    fp = (struct file *)file->realFile;
    org_fs = get_fs();
    set_fs(KERNEL_DS);
    pos = fp->f_pos;

    ret = vfs_read(fp, buf, length, &pos);
    set_fs(org_fs);
    if (ret < 0) {
        HDF_LOGE("%s read file length %d fail %d", __func__, length, ret);
        return HDF_FAILURE;
    }

    return ret;
}

#define TRACE_EVENT_ENABLE_PATH "/sys/kernel/debug/tracing/events/platform_trace_event/enable"
static int32_t TraceEventFileWrite(const char *enable)
{
    int32_t ret;
    OsalFile file;
    ret = OsalFileOpen(&file, TRACE_EVENT_ENABLE_PATH, OSAL_O_RDWR, OSAL_S_IWRITE);
    if (ret < 0) {
        HDF_LOGE("%s %d open err:%d %s", __func__, __LINE__, ret, TRACE_EVENT_ENABLE_PATH);
        return ret;
    }

    ret = TraceFileWrite(&file, enable, 1);
    if (ret < 0) {
        HDF_LOGE("%s %d write err:%d %s", __func__, __LINE__, ret, TRACE_EVENT_ENABLE_PATH);
        OsalFileClose(&file);
        return ret;
    }

    OsalFileClose(&file);
    return HDF_SUCCESS;
}

int32_t PlatformTraceStart(void)
{
    if (!TraceIsOpen()) {
        return HDF_SUCCESS;
    }
    TraceEventFileWrite("1");
    return HDF_SUCCESS;
}

int32_t PlatformTraceStop(void)
{
    if (!TraceIsOpen()) {
        return HDF_SUCCESS;
    }
    TraceEventFileWrite("0");
    return HDF_SUCCESS;
}

#define TRACE_EVENT_READ_PATH          "/sys/kernel/debug/tracing/trace"
#define TRACE_EVENT_READ_MATCH         "platfrom_trace_record_messages"
#define TRACE_INFO_DUMPER_LEN          512
#define TRACE_INFO_DUMPER_FILE_MAX_LEN 102400
void PlatformTraceInfoDump(void)
{
    int32_t ret;
    char buf[TRACE_INFO_DUMPER_LEN];
    OsalFile file;
    int32_t len = 0;
    off_t off;
    if (!TraceIsOpen()) {
        return;
    }
    ret = OsalFileOpen(&file, TRACE_EVENT_READ_PATH, OSAL_O_RDWR, OSAL_S_IREAD);
    if (ret < 0) {
        HDF_LOGE("PlatformTraceInfoDump open %s err:%d", TRACE_EVENT_READ_PATH, ret);
        return;
    }

    do {
        if (memset_s(buf, TRACE_INFO_DUMPER_LEN, 0, TRACE_INFO_DUMPER_LEN) != EOK) {
            break;
        }
        ret = TraceFileRead(&file, buf, TRACE_INFO_DUMPER_LEN - 1);
        if (ret > 0) {
            len += ret;
            off = OsalFileLseek(&file, len, 0);
            printk("%s", buf);
        }
        if ((len >= TRACE_INFO_DUMPER_FILE_MAX_LEN) || (off < 0)) {
            HDF_LOGE("PlatformTraceInfoDump trace info too big[%d] or lseek error[%ld]", len, off);
            break;
        }
    } while (ret > 0);

    OsalFileClose(&file);
}

void PlatformTraceReset(void)
{
    return;
}
