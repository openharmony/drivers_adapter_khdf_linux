/*
 * mipi_csi_dev.c
 *
 * create vfs node for mipi-csi
 *
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "mipi_csi_dev.h"
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include "hdf_log.h"
#include "mipi_csi_core.h"
#include "osal_mem.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* macro definition */
#define HDF_LOG_TAG           mipi_csi_dev
#define MIPI_RX_DEV_NAME      "mipi_csi_dev"
#define MIPI_RX_PROC_NAME     "mipi_rx"
#define MAX_DEV_NAME_LEN      48
#define HIMEDIA_DYNAMIC_MINOR 255

#define COMBO_MAX_LANE_NUM    4

#define COMBO_MIN_WIDTH       32
#define COMBO_MIN_HEIGHT      32

#define MIPI_HEIGHT_ALIGN      2
#define MIPI_WIDTH_ALIGN       2
#define MIPI_RX_MAX_PHY_NUM    1
#define LANE_DATA_COUNT        12

#define ENABLE_INT_MASK

// SeqBuf and seq_file are used in different os.
typedef struct seq_file SysProcEntryTag;
struct MipiCsiVfsPara {
    struct MipiCsiCntlr *cntlr;
    struct miscdevice *miscDev;
    // mutex and OsalMutex are used in different os.
    struct OsalMutex lock;
    void *priv;
};

static struct MipiCsiVfsPara g_vfsPara;
void SysSeqPrintf(SysProcEntryTag *m, const char *f, ...)
{
    va_list args;

    va_start(args, f);
    // LosBufPrintf and seq_printf are used in different os.
    seq_printf(m, f, args);
    va_end(args);
}

static int32_t SysMutexLock(struct OsalMutex *mutex)
{
    // mutex_lock_interruptible and OsalMutexLock are used in different os.
    return OsalMutexLock(mutex);
}

static int32_t RegisterDevice(const char *name, uint8_t id, unsigned short mode, struct file_operations *ops)
{
    int32_t error;
    struct miscdevice *dev = NULL;

    if ((name == NULL) || (ops == NULL) || (id >= MAX_CNTLR_CNT)) {
        HDF_LOGE("%s: name, ops or id is error.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    dev = OsalMemCalloc(sizeof(struct miscdevice));
    if (dev == NULL) {
        HDF_LOGE("%s: [OsalMemCalloc] failed.", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    dev->fops = ops;
    dev->name = OsalMemCalloc(MAX_DEV_NAME_LEN + 1);
    if (dev->name == NULL) {
        OsalMemFree(dev);
        HDF_LOGE("%s: [OsalMemCalloc] failed.", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (id != 0) { /* 0 : id */
        if (snprintf_s((char *)dev->name, MAX_DEV_NAME_LEN + 1, MAX_DEV_NAME_LEN, "%s%u", name, id) < 0) {
            OsalMemFree((char *)dev->name);
            OsalMemFree(dev);
            HDF_LOGE("%s: [snprintf_s] failed.", __func__);
            return HDF_FAILURE;
        }
    } else {
        if (memcpy_s((char *)dev->name, MAX_DEV_NAME_LEN, name, strlen(name)) != EOK) {
            OsalMemFree((char *)dev->name);
            OsalMemFree(dev);
            HDF_LOGE("%s: [memcpy_s] failed.", __func__);
            return HDF_ERR_IO;
        }
    }
    ops->owner = THIS_MODULE;
    dev->minor = MISC_DYNAMIC_MINOR;
    dev->mode = mode;

    // register_driver and misc_register are used in different os.
    error = misc_register(dev);
    if (error < 0) {
        HDF_LOGE("%s: id %u cannot register miscDev on minor=%d (err=%d)",
            __func__, id, MISC_DYNAMIC_MINOR, error);
        OsalMemFree((char *)dev->name);
        OsalMemFree(dev);
        return error;
    }

    g_vfsPara.miscDev = dev;
    HDF_LOGI("%s: create inode ok, name = %s, minor = %d", __func__, dev->name, dev->minor);

    return HDF_SUCCESS;
}

static void UnregisterDevice(uint8_t id)
{
    struct miscdevice *dev = NULL;

    if (id >= MAX_CNTLR_CNT) {
        HDF_LOGE("%s: id error.", __func__);
        return;
    }
    dev = g_vfsPara.miscDev;
    if (dev == NULL) {
        HDF_LOGE("%s: dev is NULL.", __func__);
        return;
    }

    // unregister_driver <---> misc_deregister
    misc_deregister(dev);
    OsalMemFree((void *)dev->name);
    dev->name = NULL;
    OsalMemFree(dev);
    dev = NULL;
    HDF_LOGI("%s: success.", __func__);
}

static struct MipiCsiCntlr *GetCntlr(const SysProcEntryTag *s)
{
    if (s == NULL) {
        HDF_LOGE("%s: s is NULL.", __func__);
        return NULL;
    }
    return g_vfsPara.cntlr;
}

static int MipiSetComboDevAttr(struct MipiCsiVfsPara *vfsPara, ComboDevAttr *pAttr)
{
    int ret;

    if (vfsPara == NULL) {
        HDF_LOGE("%s: vfsPara is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (SysMutexLock(&(vfsPara->lock))) {
        HDF_LOGE("%s: [OsalMutexLock] failed.", __func__);
        return HDF_FAILURE;
    }
    ret = MipiCsiCntlrSetComboDevAttr(vfsPara->cntlr, pAttr);
    OsalMutexUnlock(&(vfsPara->lock));

    return ret;
}

static int MipiSetExtDataType(struct MipiCsiVfsPara *vfsPara, ExtDataType *dataType)
{
    if (vfsPara == NULL) {
        HDF_LOGE("%s: vfsPara is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dataType == NULL) {
        HDF_LOGE("%s: dataType is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (SysMutexLock(&(vfsPara->lock))) {
        HDF_LOGE("%s: [OsalMutexLock] failed.", __func__);
        return HDF_FAILURE;
    }
    MipiCsiCntlrSetExtDataType(vfsPara->cntlr, dataType);
    OsalMutexUnlock(&(vfsPara->lock));

    return HDF_SUCCESS;
}

static int MipiSetPhyCmvmode(struct MipiCsiVfsPara *vfsPara, uint8_t devno, PhyCmvMode cmvMode)
{
    if (vfsPara == NULL) {
        HDF_LOGE("%s: vfsPara is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (SysMutexLock(&(vfsPara->lock))) {
        HDF_LOGE("%s: [OsalMutexLock] failed.", __func__);
        return HDF_FAILURE;
    }
    MipiCsiCntlrSetPhyCmvmode(vfsPara->cntlr, devno, cmvMode);
    OsalMutexUnlock(&(vfsPara->lock));

    return HDF_SUCCESS;
}

static long MipiRxIoctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    unsigned long *argp = (unsigned long *)(uintptr_t)arg;
    struct MipiCsiCntlr *cntlr = g_vfsPara.cntlr;
    int ret = HDF_FAILURE;
    if ((argp == NULL) || (cntlr == NULL)) {
        HDF_LOGE("%s: argp or cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    (void)filep;
    switch (cmd) {
        case HI_MIPI_SET_DEV_ATTR:
            ret = MipiSetComboDevAttr(&g_vfsPara, (ComboDevAttr *)argp);
            break;
        case HI_MIPI_SET_PHY_CMVMODE: {
            PhyCmv *pPhyCmv = (PhyCmv *)argp;
            ret = MipiSetPhyCmvmode(&g_vfsPara, pPhyCmv->devno, pPhyCmv->cmvMode);
            break;
        }
        case HI_MIPI_RESET_SENSOR:
            ret = MipiCsiCntlrResetSensor(cntlr, (uint8_t)argp);
            break;
        case HI_MIPI_UNRESET_SENSOR:
            ret = MipiCsiCntlrUnresetSensor(cntlr, (uint8_t)argp);
            break;
        case HI_MIPI_RESET_MIPI:
            ret = MipiCsiCntlrResetRx(cntlr, (uint8_t)argp);
            break;
        case HI_MIPI_UNRESET_MIPI:
            ret = MipiCsiCntlrUnresetRx(cntlr, (uint8_t)argp);
            break;
        case HI_MIPI_RESET_SLVS:
            break;
        case HI_MIPI_UNRESET_SLVS:
            break;
        case HI_MIPI_SET_HS_MODE:
            ret = MipiCsiCntlrSetHsMode(cntlr, (LaneDivideMode)argp);
            break;
        case HI_MIPI_ENABLE_MIPI_CLOCK:
            ret = MipiCsiCntlrEnableClock(cntlr, (uint8_t)argp);
            break;
        case HI_MIPI_DISABLE_MIPI_CLOCK:
            ret = MipiCsiCntlrDisableClock(cntlr, (uint8_t)argp);
            break;
        case HI_MIPI_ENABLE_SLVS_CLOCK:
            break;
        case HI_MIPI_DISABLE_SLVS_CLOCK:
            break;
        case HI_MIPI_ENABLE_SENSOR_CLOCK:
            ret = MipiCsiCntlrEnableSensorClock(cntlr, (uint8_t)argp);
            break;
        case HI_MIPI_DISABLE_SENSOR_CLOCK:
            ret = MipiCsiCntlrDisableSensorClock(cntlr, (uint8_t)argp);
            break;
        case HI_MIPI_SET_EXT_DATA_TYPE:
            ret = MipiSetExtDataType(&g_vfsPara, (ExtDataType *)argp);
            break;
        default:
            break;
    }
    HDF_LOGI("%s: cmd = %d %s.", __func__, cmd, (ret == HDF_SUCCESS) ? "success" : "failed");
    return ret;
}

#ifdef CONFIG_COMPAT
static long MipiRxCompatIoctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    return MipiRxIoctl(filep, cmd, arg);
}
#endif

#ifdef CONFIG_HI_PROC_SHOW_SUPPORT
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static int32_t ProcRegister(const char *name, uint8_t id, unsigned short mode, const struct proc_ops *ops)
#else
static int32_t ProcRegister(const char *name, uint8_t id, unsigned short mode, const struct file_operations *ops)
#endif
{
    char procName[MAX_DEV_NAME_LEN + 1];
    struct proc_dir_entry* entry = NULL;
    int32_t ret;

    if ((name == NULL) || (ops == NULL) || (id >= MAX_CNTLR_CNT)) {
        HDF_LOGE("%s: name, ops or id is error.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (memset_s(procName, MAX_DEV_NAME_LEN + 1, 0, MAX_DEV_NAME_LEN + 1) != EOK) {
        HDF_LOGE("%s: [memcpy_s] failed.", __func__);
        return HDF_ERR_IO;
    }
    if (id != 0) {
        ret = snprintf_s(procName, MAX_DEV_NAME_LEN + 1, MAX_DEV_NAME_LEN, "%s%u", name, id);
    } else {
        ret = snprintf_s(procName, MAX_DEV_NAME_LEN + 1, MAX_DEV_NAME_LEN, "%s", name);
    }
    if (ret < 0) {
        HDF_LOGE("%s: procName %s [snprintf_s] fail", __func__, procName);
        return HDF_FAILURE;
    }

    // proc_create and CreateProcEntry are used in different os.
    entry = proc_create(procName, mode, NULL, ops);
    if (entry == NULL) {
        HDF_LOGE("%s: [proc_create] name %s fail", __func__, procName);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success.", __func__);
    return HDF_SUCCESS;
}

static void ProcUnregister(const char *name, uint8_t id)
{
    char procName[MAX_DEV_NAME_LEN + 1];
    int32_t ret;

    if (id >= MAX_CNTLR_CNT) {
        HDF_LOGE("%s: id error.", __func__);
        return;
    }
    if (memset_s(procName, MAX_DEV_NAME_LEN + 1, 0, MAX_DEV_NAME_LEN + 1) != EOK) {
        HDF_LOGE("%s: [memcpy_s] failed.", __func__);
        return;
    }
    if (id != 0) {
        ret = snprintf_s(procName, MAX_DEV_NAME_LEN + 1, MAX_DEV_NAME_LEN, "%s%u", name, id);
    } else {
        ret = snprintf_s(procName, MAX_DEV_NAME_LEN + 1, MAX_DEV_NAME_LEN, "%s", name);
    }
    if (ret < 0) {
        HDF_LOGE("%s: [snprintf_s] failed.", __func__);
        return;
    }

    // remove_proc_entry and ProcFreeEntry are used in different os.
    remove_proc_entry(procName, NULL);
    HDF_LOGI("%s: success.", __func__);
}

static const char *MipiGetIntputModeName(InputMode inputMode)
{
    switch (inputMode) {
        case INPUT_MODE_SUBLVDS:
        case INPUT_MODE_LVDS:
        case INPUT_MODE_HISPI:
            return "LVDS";

        case INPUT_MODE_MIPI:
            return "MIPI";

        case INPUT_MODE_CMOS:
        case INPUT_MODE_BT1120:
        case INPUT_MODE_BT656:
        case INPUT_MODE_BYPASS:
            return "CMOS";

        default:
            break;
    }

    return "N/A";
}

static const char *MipiGetRawDataTypeName(DataType type)
{
    switch (type) {
        case DATA_TYPE_RAW_8BIT:
            return "RAW8";

        case DATA_TYPE_RAW_10BIT:
            return "RAW10";

        case DATA_TYPE_RAW_12BIT:
            return "RAW12";

        case DATA_TYPE_RAW_14BIT:
            return "RAW14";

        case DATA_TYPE_RAW_16BIT:
            return "RAW16";

        case DATA_TYPE_YUV420_8BIT_NORMAL:
            return "yuv420_8bit_normal";

        case DATA_TYPE_YUV420_8BIT_LEGACY:
            return "yuv420_8bit_legacy";

        case DATA_TYPE_YUV422_8BIT:
            return "yuv422_8bit";

        case DATA_TYPE_YUV422_PACKED:
            return "yuv422_packed";

        default:
            break;
    }

    return "N/A";
}

static const char *MipiGetDataRateName(MipiDataRate dataRate)
{
    switch (dataRate) {
        case MIPI_DATA_RATE_X1:
            return "X1";

        case MIPI_DATA_RATE_X2:
            return "X2";

        default:
            break;
    }

    return "N/A";
}

static const char *MipiPrintMipiWdrMode(MipiWdrMode wdrMode)
{
    switch (wdrMode) {
        case HI_MIPI_WDR_MODE_NONE:
            return "None";

        case HI_MIPI_WDR_MODE_VC:
            return "VC";

        case HI_MIPI_WDR_MODE_DT:
            return "DT";

        case HI_MIPI_WDR_MODE_DOL:
            return "DOL";

        default:
            break;
    }

    return "N/A";
}

static const char *MipiPrintLvdsWdrMode(WdrMode wdrMode)
{
    switch (wdrMode) {
        case HI_WDR_MODE_NONE:
            return "None";

        case HI_WDR_MODE_2F:
            return "2F";

        case HI_WDR_MODE_3F:
            return "3F";

        case HI_WDR_MODE_4F:
            return "4F";

        case HI_WDR_MODE_DOL_2F:
            return "DOL_2F";

        case HI_WDR_MODE_DOL_3F:
            return "DOL_3F";

        case HI_WDR_MODE_DOL_4F:
            return "DOL_4F";

        default:
            break;
    }

    return "N/A";
}

static const char *MipiPrintLaneDivideMode(LaneDivideMode mode)
{
    if (mode == LANE_DIVIDE_MODE_0) {
        return "4";
    } else {
        return "2+2";
    }
}

static void ProcShowMipiDevice(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr)
{
    int32_t ret;
    const char *wdrMode = NULL;
    uint8_t devno;
    InputMode inputMode;
    DataType dataType;
    MipiDevCtx tag;

    ret = MipiCsiDebugGetMipiDevCtx(cntlr, &tag);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [MipiCsiDebugGetMipiDevCtx] failed.", __func__);
        return;
    }

    SysSeqPrintf(s, "\nMIPI DEV ATTR\n");
    SysSeqPrintf(s,
        "%8s"  "%10s"      "%10s"      "%20s"      "%10s"     "%8s"   "%8s"   "%8s"    "%8s"    "\n",
        "Devno", "WorkMode", "DataRate", "DataType", "WDRMode", "ImgX", "ImgY", "ImgW",  "ImgH");

    for (devno = 0; devno < MIPI_RX_MAX_DEV_NUM; devno++) {
        ComboDevAttr *pdevAttr = &tag.comboDevAttr[devno];
        inputMode = pdevAttr->inputMode;
        if (!tag.devCfged[devno]) {
            continue;
        }

        if (inputMode == INPUT_MODE_MIPI) {
            dataType = pdevAttr->mipiAttr.inputDataType;
            wdrMode = MipiPrintMipiWdrMode(pdevAttr->mipiAttr.wdrMode);
        } else {
            dataType = pdevAttr->lvdsAttr.inputDataType;
            wdrMode = MipiPrintLvdsWdrMode(pdevAttr->lvdsAttr.wdrMode);
        }

        SysSeqPrintf(s, "%8d" "%10s" "%10s" "%20s" "%10s" "%8d" "%8d" "%8d" "%8d" "\n",
            devno,
            MipiGetIntputModeName(inputMode),
            MipiGetDataRateName(pdevAttr->dataRate),
            MipiGetRawDataTypeName(dataType),
            wdrMode,
            pdevAttr->imgRect.x,
            pdevAttr->imgRect.y,
            pdevAttr->imgRect.width,
            pdevAttr->imgRect.height);
    }
}

static void ProcShowMipiLane(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr)
{
    int32_t ret;
    uint8_t devno;
    InputMode inputMode;
    MipiDevCtx tag;

    ret = MipiCsiDebugGetMipiDevCtx(cntlr, &tag);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [MipiCsiDebugGetMipiDevCtx] failed.", __func__);
        return;
    }

    SysSeqPrintf(s, "\nMIPI LANE INFO\n");
    SysSeqPrintf(s, "%8s"   "%24s" "\n", "Devno",  "LaneID");

    for (devno = 0; devno < MIPI_RX_MAX_DEV_NUM; devno++) {
        ComboDevAttr *pdevAttr = &tag.comboDevAttr[devno];

        inputMode = pdevAttr->inputMode;
        if (!tag.devCfged[devno]) {
            continue;
        }

        if (inputMode == INPUT_MODE_MIPI) {
            SysSeqPrintf(s, "%8d" "%10d,%3d,%3d,%3d" "\n",
                devno,
                pdevAttr->mipiAttr.laneId[0],  /* 0 -- laneId 0 */
                pdevAttr->mipiAttr.laneId[1],  /* 1 -- laneId 1 */
                pdevAttr->mipiAttr.laneId[2],  /* 2 -- laneId 2 */
                pdevAttr->mipiAttr.laneId[3]); /* 3 -- laneId 3 */
        } else if (inputMode == INPUT_MODE_LVDS ||
            inputMode == INPUT_MODE_SUBLVDS ||
            inputMode == INPUT_MODE_HISPI) {
            SysSeqPrintf(s, "%8d" "%10d,%3d,%3d,%3d" "\n",
                devno,
                pdevAttr->lvdsAttr.laneId[0],
                pdevAttr->lvdsAttr.laneId[1],
                pdevAttr->lvdsAttr.laneId[2],  /* 2 -- laneId 2 */
                pdevAttr->lvdsAttr.laneId[3]); /* 3 -- laneId 3 */
        }
    }
}

static unsigned int GetPhyData(struct MipiCsiCntlr *cntlr, int phyId, int laneId)
{
    int32_t ret;
    unsigned int laneData;

    ret = MipiCsiDebugGetPhyData(cntlr, phyId, laneId, &laneData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [MipiCsiDebugGetPhyData] failed.", __func__);
        return 0;
    }

    return laneData;
}

static unsigned int GetPhyMipiLinkData(struct MipiCsiCntlr *cntlr, int phyId, int laneId)
{
    int32_t ret;
    unsigned int laneData;

    ret = MipiCsiDebugGetPhyMipiLinkData(cntlr, phyId, laneId, &laneData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [MipiCsiDebugGetPhyMipiLinkData] failed.", __func__);
        return 0;
    }

    return laneData;
}

static unsigned int GetPhyLvdsLinkData(struct MipiCsiCntlr *cntlr, int phyId, int laneId)
{
    int32_t ret;
    unsigned int laneData;

    ret = MipiCsiDebugGetPhyLvdsLinkData(cntlr, phyId, laneId, &laneData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [MipiCsiDebugGetPhyLvdsLinkData] failed.", __func__);
        return 0;
    }

    return laneData;
}

static void ProcShowMipiPhyData(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr)
{
    int i;

    SysSeqPrintf(s, "\nMIPI PHY DATA INFO\n");
    SysSeqPrintf(s, "%8s"  "%15s"       "%19s"     "%24s"    "%22s"  "\n",
        "PhyId", "LaneId", "PhyData",  "MipiData", "LvdsData");

    for (i = 0; i < MIPI_RX_MAX_PHY_NUM; i++) {
        SysSeqPrintf(s,
            "%8d%8d,%2d,%2d,%2d,   0x%02x,0x%02x,0x%02x,0x%02x    0x%02x,0x%02x,0x%02x,0x%02x"
            "    0x%02x,0x%02x,0x%02x,0x%02x\n",
            i,
            4 * i,      /* 4 * i + 0 -- phyId laneId 0 */
            4 * i + 1,  /* 4 * i + 1 -- phyId laneId 1 */
            4 * i + 2,  /* 4 * i + 2 -- phyId laneId 2 */
            4 * i + 3,  /* 4 * i + 3 -- phyId laneId 3 */
            GetPhyData(cntlr, i, 0),          /* 0 -- laneId 0 */
            GetPhyData(cntlr, i, 1),          /* 1 -- laneId 1 */
            GetPhyData(cntlr, i, 2),          /* 2 -- laneId 2 */
            GetPhyData(cntlr, i, 3),          /* 3 -- laneId 3 */
            GetPhyMipiLinkData(cntlr, i, 0),  /* 0 -- laneId 0 */
            GetPhyMipiLinkData(cntlr, i, 1),  /* 1 -- laneId 1 */
            GetPhyMipiLinkData(cntlr, i, 2),  /* 2 -- laneId 2 */
            GetPhyMipiLinkData(cntlr, i, 3),  /* 3 -- laneId 3 */
            GetPhyLvdsLinkData(cntlr, i, 0),  /* 0 -- laneId 0 */
            GetPhyLvdsLinkData(cntlr, i, 1),  /* 1 -- laneId 1 */
            GetPhyLvdsLinkData(cntlr, i, 2),  /* 2 -- laneId 2 */
            GetPhyLvdsLinkData(cntlr, i, 3)); /* 3 -- laneId 3 */
    }
}

static void ProcShowMipiDetectInfo(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr,
    uint8_t devnoArray[], int mipiCnt)
{
    int32_t ret;
    ImgSize imageSize;
    short vcNum;
    int devnoIdx;

    SysSeqPrintf(s, "\nMIPI DETECT INFO\n");
    SysSeqPrintf(s, "%6s%3s%8s%8s\n", "Devno", "VC", "width", "height");

    for (devnoIdx = 0; devnoIdx < mipiCnt; devnoIdx++) {
        for (vcNum = 0; vcNum < WDR_VC_NUM; vcNum++) {
            ret = MipiCsiDebugGetMipiImgsizeStatis(cntlr, devnoArray[devnoIdx], vcNum, &imageSize);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s: [MipiCsiDebugGetMipiImgsizeStatis] failed.", __func__);
                return;
            }
            SysSeqPrintf(s, "%6d%3d%8u%8u\n",
                devnoArray[devnoIdx], vcNum, imageSize.width, imageSize.height);
        }
    }
}

static void ProcShowLvdsDetectInfo(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr,
    uint8_t devnoArray[], int mipiCnt)
{
    int32_t ret;
    ImgSize imageSize;
    short vcNum;
    int devnoIdx;

    SysSeqPrintf(s, "\nLVDS DETECT INFO\n");
    SysSeqPrintf(s, "%6s%3s%8s%8s\n", "Devno", "VC", "width", "height");

    for (devnoIdx = 0; devnoIdx < mipiCnt; devnoIdx++) {
        for (vcNum = 0; vcNum < WDR_VC_NUM; vcNum++) {
            ret = MipiCsiDebugGetLvdsImgsizeStatis(cntlr, devnoArray[devnoIdx], vcNum, &imageSize);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s: [MipiCsiDebugGetLvdsImgsizeStatis] failed.", __func__);
                return;
            }
            SysSeqPrintf(s, "%6d%3d%8d%8d\n",
                devnoArray[devnoIdx], vcNum, imageSize.width, imageSize.height);
        }
    }
}

static void ProcShowLvdsLaneDetectInfo(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr,
    uint8_t devnoArray[], int mipiCnt)
{
    int32_t ret;
    ImgSize imageSize;
    short lane;
    int devnoIdx;
    ComboDevAttr *pstcomboDevAttr = NULL;
    MipiDevCtx tag;

    ret = MipiCsiDebugGetMipiDevCtx(cntlr, &tag);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [MipiCsiDebugGetMipiDevCtx] failed.", __func__);
        return;
    }

    SysSeqPrintf(s, "\nLVDS LANE DETECT INFO\n");
    SysSeqPrintf(s, "%6s%6s%8s%8s\n", "Devno", "Lane", "width", "height");

    for (devnoIdx = 0; devnoIdx < mipiCnt; devnoIdx++) {
        pstcomboDevAttr = &tag.comboDevAttr[devnoArray[devnoIdx]];
        for (lane = 0; lane < LVDS_LANE_NUM; lane++) {
            if (pstcomboDevAttr->lvdsAttr.laneId[lane] == -1) {
                continue;
            }
            ret = MipiCsiDebugGetLvdsLaneImgsizeStatis(cntlr, devnoArray[devnoIdx], lane, &imageSize);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s: [MipiCsiDebugGetLvdsLaneImgsizeStatis] failed.", __func__);
                return;
            }

            SysSeqPrintf(s, "%6d%6d%8d%8d\n",
                devnoArray[devnoIdx], pstcomboDevAttr->lvdsAttr.laneId[lane],
                imageSize.width, imageSize.height);
        }
    }
}

static void ProcShowMipiHsMode(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr)
{
    int32_t ret;
    MipiDevCtx tag;
    LaneDivideMode laneDivideMode;

    ret = MipiCsiDebugGetMipiDevCtx(cntlr, &tag);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [MipiCsiDebugGetMipiDevCtx] failed.", __func__);
        return;
    }

    laneDivideMode = tag.laneDivideMode;
    SysSeqPrintf(s, "\nMIPI LANE DIVIDE MODE\n");
    SysSeqPrintf(s, "%6s" "%20s" "\n", "MODE", "LANE DIVIDE");
    SysSeqPrintf(s, "%6d" "%20s" "\n", laneDivideMode, MipiPrintLaneDivideMode(laneDivideMode));
}

static void ProcShowPhyCilIntErrCnt(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr)
{
    int32_t ret;
    PhyErrIntCnt tag;
    int phyId;

    SysSeqPrintf(s, "\nPHY CIL ERR INT INFO\n");
    SysSeqPrintf(s, "%8s%11s%10s%12s%12s%12s%12s%9s%8s%10s%10s%10s%10s\n",
        "PhyId", "Clk2TmOut", "ClkTmOut", "Lane0TmOut", "Lane1TmOut", "Lane2TmOut", "Lane3TmOut",
        "Clk2Esc", "ClkEsc", "Lane0Esc", "Lane1Esc", "Lane2Esc", "Lane3Esc");

    for (phyId = 0; phyId < MIPI_RX_MAX_PHY_NUM; phyId++) {
        ret = MipiCsiDebugGetPhyErrIntCnt(cntlr, phyId, &tag);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: [MipiCsiDebugGetPhyErrIntCnt] failed.", __func__);
            return;
        }

        SysSeqPrintf(s, "%8d%11d%10d%12d%12d%12d%12d%9d%8d%10d%10d%10d%10d\n",
            phyId,
            tag.clk1FsmTimeoutErrCnt,
            tag.clk0FsmTimeoutErrCnt,
            tag.d0FsmTimeoutErrCnt,
            tag.d1FsmTimeoutErrCnt,
            tag.d2FsmTimeoutErrCnt,
            tag.d3FsmTimeoutErrCnt,
            tag.clk1FsmEscapeErrCnt,
            tag.clk0FsmEscapeErrCnt,
            tag.d0FsmEscapeErrCnt,
            tag.d1FsmEscapeErrCnt,
            tag.d2FsmEscapeErrCnt,
            tag.d3FsmEscapeErrCnt);
    }
}

static void ProcShowMipirxCrcErr(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr,
    const uint8_t devnoArray[], int mipiCnt)
{
    int32_t ret;
    MipiErrIntCnt tag;
    int devnoIdx;
    uint8_t devno;

    SysSeqPrintf(s, "\nMIPI ERROR INT INFO 1\n");
    SysSeqPrintf(s, "%8s%6s%8s%8s%8s%8s%14s%14s%14s%14s\n",
        "Devno", "Ecc2", "Vc0CRC", "Vc1CRC", "Vc2CRC", "Vc3CRC",
        "Vc0EccCorrct", "Vc1EccCorrct", "Vc2EccCorrct", "Vc3EccCorrct");

    for (devnoIdx = 0; devnoIdx < mipiCnt; devnoIdx++) {
        devno = devnoArray[devnoIdx];
        ret = MipiCsiDebugGetMipiErrInt(cntlr, devno, &tag);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: [MipiCsiDebugGetMipiErrInt] failed.", __func__);
            return;
        }

        SysSeqPrintf(s, "%8d%6d%8d%8d%8d%8d%14d%14d%14d%14d\n",
            devno,
            tag.errEccDoubleCnt,
            tag.vc0ErrCrcCnt,
            tag.vc1ErrCrcCnt,
            tag.vc2ErrCrcCnt,
            tag.vc3ErrCrcCnt,
            tag.vc0ErrEccCorrectedCnt,
            tag.vc1ErrEccCorrectedCnt,
            tag.vc2ErrEccCorrectedCnt,
            tag.vc3ErrEccCorrectedCnt);
    }
}

static void ProcShowMipirxVcErr(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr,
    const uint8_t devnoArray[], int mipiCnt)
{
    int32_t ret;
    MipiErrIntCnt tag;
    int devnoIdx;
    uint8_t devno;

    SysSeqPrintf(s, "\nMIPI ERROR INT INFO 2\n");
    SysSeqPrintf(s, "%8s%7s%7s%7s%7s%11s%11s%11s%11s\n",
        "Devno", "Vc0Dt", "Vc1Dt", "Vc2Dt", "Vc3Dt",
        "Vc0FrmCrc", "Vc1FrmCrc", "Vc2FrmCrc", "Vc3FrmCrc");

    for (devnoIdx = 0; devnoIdx < mipiCnt; devnoIdx++) {
        devno = devnoArray[devnoIdx];
        ret = MipiCsiDebugGetMipiErrInt(cntlr, devno, &tag);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: [MipiCsiDebugGetMipiErrInt] failed.", __func__);
            return;
        }
        SysSeqPrintf(s, "%8d%7d%7d%7d%7d%11d%11d%11d%11d\n",
            devno,
            tag.errIdVc0Cnt,
            tag.errIdVc1Cnt,
            tag.errIdVc2Cnt,
            tag.errIdVc3Cnt,
            tag.errFrameDataVc0Cnt,
            tag.errFrameDataVc1Cnt,
            tag.errFrameDataVc2Cnt,
            tag.errFrameDataVc3Cnt);
    }
    SysSeqPrintf(s, "\nMIPI ERROR INT INFO 3\n");
    SysSeqPrintf(s, "%8s%11s%11s%11s%11s%12s%12s%12s%12s\n",
        "Devno", "Vc0FrmSeq", "Vc1FrmSeq", "Vc2FrmSeq", "Vc3FrmSeq",
        "Vc0BndryMt", "Vc1BndryMt", "Vc2BndryMt", "Vc3BndryMt");
    for (devnoIdx = 0; devnoIdx < mipiCnt; devnoIdx++) {
        devno = devnoArray[devnoIdx];
        ret = MipiCsiDebugGetMipiErrInt(cntlr, devno, &tag);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: [MipiCsiDebugGetMipiErrInt] failed.", __func__);
            return;
        }
        SysSeqPrintf(s, "%8d%11d%11d%11d%11d%12d%12d%12d%12d\n",
            devno,
            tag.errFSeqVc0Cnt,
            tag.errFSeqVc1Cnt,
            tag.errFSeqVc2Cnt,
            tag.errFSeqVc3Cnt,
            tag.errFBndryMatchVc0Cnt,
            tag.errFBndryMatchVc1Cnt,
            tag.errFBndryMatchVc2Cnt,
            tag.errFBndryMatchVc3Cnt);
    }
}

static void ProcShowMipirxFifoErr(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr,
    const uint8_t devnoArray[], int mipiCnt)
{
    int32_t ret;
    MipiErrIntCnt tag;
    int devnoIdx;
    uint8_t devno;

    SysSeqPrintf(s, "\nMIPI ERROR INT INFO 4\n");
    SysSeqPrintf(s, "%8s%15s%14s%14s%15s\n",
        "Devno", "DataFifoRdErr", "CmdFifoRdErr", "CmdFifoWrErr", "DataFifoWrErr");

    for (devnoIdx = 0; devnoIdx < mipiCnt; devnoIdx++) {
        devno = devnoArray[devnoIdx];
        ret = MipiCsiDebugGetMipiErrInt(cntlr, devno, &tag);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: [MipiCsiDebugGetMipiErrInt] failed.", __func__);
            return;
        }

        SysSeqPrintf(s, "%8d%15d%14d%14d%15d\n",
            devno,
            tag.dataFifoRderrCnt,
            tag.cmdFifoRderrCnt,
            tag.dataFifoWrerrCnt,
            tag.cmdFifoWrerrCnt);
    }
}

static void ProcShowMipiIntErr(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr, uint8_t devnoArray[], int mipiCnt)
{
    ProcShowMipirxCrcErr(s, cntlr, devnoArray, mipiCnt);
    ProcShowMipirxVcErr(s, cntlr, devnoArray, mipiCnt);
    ProcShowMipirxFifoErr(s, cntlr, devnoArray, mipiCnt);
}

static void ProcShowLvdsIntErr(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr,
    const uint8_t devnoArray[], int lvdsCnt)
{
    int32_t ret;
    LvdsErrIntCnt tag;
    uint8_t devno;
    int devnoIdx;

    SysSeqPrintf(s, "\nLVDS ERROR INT INFO\n");
    SysSeqPrintf(s, "%8s%10s%10s%8s%9s%12s%12s\n",
        "Devno", "CmdRdErr", "CmdWrErr", "PopErr", "StatErr", "Link0WrErr", "Link0RdErr");

    for (devnoIdx = 0; devnoIdx < lvdsCnt; devnoIdx++) {
        devno = devnoArray[devnoIdx];
        ret = MipiCsiDebugGetLvdsErrIntCnt(cntlr, devno, &tag);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: [MipiCsiDebugGetLvdsErrIntCnt] failed.", __func__);
            return;
        }

        SysSeqPrintf(s, "%8d%10d%10d%8d%9d%12d%12d\n",
            devno,
            tag.cmdRdErrCnt,
            tag.cmdWrErrCnt,
            tag.popErrCnt,
            tag.lvdsStateErrCnt,
            tag.link0RdErrCnt,
            tag.link0WrErrCnt);
    }
}

static void ProcShowAlignIntErr(SysProcEntryTag *s, struct MipiCsiCntlr *cntlr)
{
    int32_t ret;
    AlignErrIntCnt tag;
    uint8_t devno;

    SysSeqPrintf(s, "\nALIGN ERROR INT INFO\n");
    for (devno = 0; devno < MIPI_RX_MAX_DEV_NUM; devno++) {
        ret = MipiCsiDebugGetAlignErrIntCnt(cntlr, devno, &tag);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: [MipiCsiDebugGetAlignErrIntCnt] failed.", __func__);
            continue;
        }
        SysSeqPrintf(s, "%8s%14s%10s%10s%10s%10s\n",
            "Devno", "FIFO_FullErr", "Lane0Err", "Lane1Err", "Lane2Err", "Lane3Err");
        SysSeqPrintf(s, "%8d%14d%10d%10d%10d%10d\n",
            devno,
            tag.fifoFullErrCnt,
            tag.lane0AlignErrCnt,
            tag.lane1AlignErrCnt,
            tag.lane2AlignErrCnt,
            tag.lane3AlignErrCnt);
    }
}

static void MipiLvdsProcShow(int mipiCnt, int lvdsCnt, uint8_t devnoMipi[MIPI_RX_MAX_DEV_NUM],
    uint8_t devnoLvds[MIPI_RX_MAX_DEV_NUM], SysProcEntryTag *s)
{
    struct MipiCsiCntlr *cntlr = GetCntlr(s);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return;
    }

    if (mipiCnt > 0 || lvdsCnt > 0) {
        ProcShowMipiHsMode(s, cntlr);
        ProcShowMipiDevice(s, cntlr);
        ProcShowMipiLane(s, cntlr);
        ProcShowMipiPhyData(s, cntlr);
    }

    if (mipiCnt > 0) {
        ProcShowMipiDetectInfo(s, cntlr, devnoMipi, mipiCnt);
    }

    if (lvdsCnt > 0) {
        ProcShowLvdsDetectInfo(s, cntlr, devnoLvds, lvdsCnt);
        ProcShowLvdsLaneDetectInfo(s, cntlr, devnoLvds, lvdsCnt);
    }

    if (mipiCnt > 0 || lvdsCnt > 0) {
        ProcShowPhyCilIntErrCnt(s, cntlr);
    }

    if (mipiCnt > 0) {
        ProcShowMipiIntErr(s, cntlr, devnoMipi, mipiCnt);
    }

    if (lvdsCnt > 0) {
        ProcShowLvdsIntErr(s, cntlr, devnoLvds, lvdsCnt);
    }

    if (mipiCnt > 0 || lvdsCnt > 0) {
        ProcShowAlignIntErr(s, cntlr);
    }
}

static int32_t GetMipiOrLvdsDeviceCount(SysProcEntryTag *s, int *pmipiCnt, int *plvdsCnt, uint8_t devnoMipi[],
    uint8_t devnoLvds[])
{
    int32_t ret;
    uint8_t devno;
    int mipiCnt = 0;
    int lvdsCnt = 0;
    InputMode inputMode;
    MipiDevCtx tag;
    struct MipiCsiCntlr *cntlr = NULL;

    if ((pmipiCnt == NULL) || (plvdsCnt == NULL) || (devnoMipi == NULL) || (devnoLvds == NULL)) {
        HDF_LOGE("%s: pmipiCnt, plvdsCnt and other parameters are invalid.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = GetCntlr(s);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = MipiCsiDebugGetMipiDevCtx(cntlr, &tag);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [MipiCsiDebugGetMipiDevCtx] failed.", __func__);
        return ret;
    }

    for (devno = 0; devno < MIPI_RX_MAX_DEV_NUM; devno++) {
        if (!tag.devCfged[devno]) {
            continue;
        }

        inputMode = tag.comboDevAttr[devno].inputMode;
        if (inputMode == INPUT_MODE_MIPI) {
            devnoMipi[mipiCnt] = devno;
            mipiCnt++;
        } else if ((inputMode == INPUT_MODE_LVDS) || (inputMode == INPUT_MODE_SUBLVDS) ||
            (inputMode == INPUT_MODE_HISPI)) {
            devnoLvds[lvdsCnt] = devno;
            lvdsCnt++;
        }
    }
    *pmipiCnt = mipiCnt;
    *plvdsCnt = lvdsCnt;

    return ret;
}

static void MipiProcShow(SysProcEntryTag *s)
{
    int32_t ret;
    int mipiCnt;
    int lvdsCnt;
    uint8_t devnoMipi[MIPI_RX_MAX_DEV_NUM] = {0};
    uint8_t devnoLvds[MIPI_RX_MAX_DEV_NUM] = {0};

    ret = GetMipiOrLvdsDeviceCount(s, &mipiCnt, &lvdsCnt, devnoMipi, devnoLvds);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [GetMipiOrLvdsDeviceCount] failed.", __func__);
        return;
    }
    MipiLvdsProcShow(mipiCnt, lvdsCnt, devnoMipi, devnoLvds, s);
}

static int MipiRxProcShow(SysProcEntryTag *s, void *v)
{
    SysSeqPrintf(s, "\nModule: [MIPI_RX], Build Time["__DATE__", "__TIME__"]\n");
    MipiProcShow(s);
    HDF_LOGI("%s: v %p", __func__, v);
    HDF_LOGI("%s: success.", __func__);
    return HDF_SUCCESS;
}

static int MipiCsiDevProcOpen(struct inode *inode, struct file *file)
{
    (void)inode;
    HDF_LOGI("%s: enter.", __func__);
    return single_open(file, MipiRxProcShow, NULL);
}

// ProcFileOperations and file_operations are used in different os.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops g_procMipiCsiDevOps = {
    .proc_open = MipiCsiDevProcOpen,
    .proc_read = seq_read,
};
#else
static const struct file_operations g_procMipiCsiDevOps = {
    .open = MipiCsiDevProcOpen,
    .read = seq_read,
};
#endif
#endif

static int MipiRxInit(void)
{
    OsalMutexInit(&g_vfsPara.lock);

    return HDF_SUCCESS;
}

static void MipiRxExit(void)
{
    OsalMutexDestroy(&g_vfsPara.lock);
}

static int MipiRxOpen(struct inode *inode, struct file *filep)
{
    (void)inode;
    (void)filep;

    g_vfsPara.cntlr = MipiCsiCntlrGet(0);
    HDF_LOGI("%s: success.", __func__);

    return HDF_SUCCESS;
}

static int MipiRxRelease(struct inode *inode, struct file *filep)
{
    (void)inode;
    (void)filep;

    if (g_vfsPara.cntlr != NULL) {
        MipiCsiCntlrPut(g_vfsPara.cntlr);
    }
    HDF_LOGI("%s: success.", __func__);

    return HDF_SUCCESS;
}

// file_operations_vfs and file_operations are used in different os.
static const struct file_operations g_mipiRxFops = {
    .open = MipiRxOpen,
    .release = MipiRxRelease,
    .unlocked_ioctl = MipiRxIoctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = MipiRxCompatIoctl,
#endif
};

int MipiCsiDevModuleInit(uint8_t id)
{
    int32_t ret;

    /* 0660 : node mode */
    ret = RegisterDevice(MIPI_RX_DEV_NAME, id, 0660, (struct file_operations *)&g_mipiRxFops);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [RegisterDevice] fail: %d.", __func__, ret);
        return ret;
    }
#ifdef CONFIG_HI_PROC_SHOW_SUPPORT
    ret = ProcRegister(MIPI_RX_PROC_NAME, id, 0440, &g_procMipiCsiDevOps); /* 0440 : proc file mode */
    if (ret != HDF_SUCCESS) {
        UnregisterDevice(id);
        HDF_LOGE("%s: [ProcRegister] fail: %d.", __func__, ret);
        return ret;
    }
#endif

    ret = MipiRxInit();
    if (ret != HDF_SUCCESS) {
        UnregisterDevice(id);
#ifdef CONFIG_HI_PROC_SHOW_SUPPORT
        ProcUnregister(MIPI_RX_PROC_NAME, id);
#endif
        HDF_LOGE("%s: [MipiRxInit] failed.", __func__);
        return ret;
    }

    HDF_LOGI("%s: success!", __func__);
    return ret;
}

void MipiCsiDevModuleExit(uint8_t id)
{
    MipiRxExit();
#ifdef CONFIG_HI_PROC_SHOW_SUPPORT
    ProcUnregister(MIPI_RX_PROC_NAME, id);
#endif
    UnregisterDevice(id);

    HDF_LOGI("%s: success!", __func__);
}

#ifdef __cplusplus
#if __cplusplus
}

#endif
#endif /* End of #ifdef __cplusplus */
