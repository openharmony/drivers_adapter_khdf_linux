/*
 * mipi_tx_dev.c
 *
 * create vfs node for mipi
 *
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/seq_file.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "mipi_dsi_adapter.h"
#include "mipi_tx_dev.h"
#include "mipi_tx_hi35xx.h"
#include "mipi_tx_reg.h"
#include "osal_io.h"
#include "osal_mem.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * macro definition                                                         *
 ****************************************************************************/
#define MIPI_TX_DEV_NAME           "hi_mipi_tx"
#define MIPI_TX_PROC_NAME          "mipi_tx"

struct semaphore g_mipiTxSem;
static int32_t MipiTxSemaInit(struct semaphore *sem, uint16_t val)
{
    if (sem == NULL) {
        return -1;
    }
    sema_init(sem, val);
    return 0;
}

static void MipiTxSemaDestroy(const struct semaphore *sem)
{
    (void *)sem;
    return;
}

static int32_t MipiTxSemaDownInterruptable(const struct semaphore *sem)
{
    return down_interruptible((struct semaphore *)sem);
}

static void MipiTxUp(const struct semaphore *sem)
{
    up((struct semaphore *)sem);
}

static int g_enDev;
static int g_enDevCfg;
MipiTxDevCtxTag g_mipiTxDevCtx;

static int MipiTxCheckCombDevCfg(const ComboDevCfgTag *devCfg)
{
    int i;
    int validLaneId[LANE_MAX_NUM] = {0, 1, 2, 3};

    if (g_enDev == TRUE) {
        HDF_LOGE("mipi_tx dev has enable!\n");
        return -1;
    }
    if (devCfg->devno != 0) {
        HDF_LOGE("mipi_tx dev devno err!\n");
        return -1;
    }
    for (i = 0; i < LANE_MAX_NUM; i++) {
        if ((devCfg->laneId[i] != validLaneId[i]) && (devCfg->laneId[i] != MIPI_TX_DISABLE_LANE_ID)) {
            HDF_LOGE("mipi_tx dev laneId %d err!\n", devCfg->laneId[i]);
            return -1;
        }
    }
    if ((devCfg->outputMode != OUTPUT_MODE_CSI) && (devCfg->outputMode != OUTPUT_MODE_DSI_VIDEO) &&
        (devCfg->outputMode != OUTPUT_MODE_DSI_CMD)) {
        HDF_LOGE("mipi_tx dev outputMode %d err!\n", devCfg->outputMode);
        return -1;
    }
    if ((devCfg->videoMode != BURST_MODE) && (devCfg->videoMode != NON_BURST_MODE_SYNC_PULSES) &&
        (devCfg->videoMode != NON_BURST_MODE_SYNC_EVENTS)) {
        HDF_LOGE("mipi_tx dev videoMode %d err!\n", devCfg->videoMode);
        return -1;
    }
    if ((devCfg->outputFormat != OUT_FORMAT_RGB_16_BIT) && (devCfg->outputFormat != OUT_FORMAT_RGB_18_BIT) &&
        (devCfg->outputFormat != OUT_FORMAT_RGB_24_BIT) && (devCfg->outputFormat !=
        OUT_FORMAT_YUV420_8_BIT_NORMAL) && (devCfg->outputFormat != OUT_FORMAT_YUV420_8_BIT_LEGACY) &&
        (devCfg->outputFormat != OUT_FORMAT_YUV422_8_BIT)) {
        HDF_LOGE("mipi_tx dev outputFormat %d err!\n", devCfg->outputFormat);
        return -1;
    }
    return 0;
}

int MipiTxSetComboDevCfg(const ComboDevCfgTag *devCfg)
{
    int ret;

    ret = MipiTxCheckCombDevCfg(devCfg);
    if (ret < 0) {
        HDF_LOGE("mipi_tx check combo_dev config failed!\n");
        return ret;
    }
    /* set controller config */
    MipiTxDrvSetControllerCfg(devCfg);
    /* set phy config */
    MipiTxDrvSetPhyCfg(devCfg);
    ret = memcpy_s(&g_mipiTxDevCtx.devCfg, sizeof(ComboDevCfgTag), devCfg, sizeof(ComboDevCfgTag));
    if (ret != EOK) {
        return ret;
    }
    g_enDevCfg = TRUE;
    return ret;
}

static int MipiTxCheckSetCmdInfo(const CmdInfoTag *cmdInfo)
{
    if (g_enDev == TRUE) {
        HDF_LOGE("mipi_tx dev has enable!\n");
        return -1;
    }
    if (g_enDevCfg != TRUE) {
        HDF_LOGE("mipi_tx dev has not config!\n");
        return -1;
    }
    if (cmdInfo->devno != 0) {
        HDF_LOGE("mipi_tx devno %d err!\n", cmdInfo->devno);
        return -1;
    }
    /* When cmd is not NULL, cmd_size means the length of cmd or it means cmd and addr */
    if (cmdInfo->cmd != NULL) {
        if (cmdInfo->cmdSize > MIPI_TX_SET_DATA_SIZE) {
            HDF_LOGE("mipi_tx dev cmd_size %d err!\n", cmdInfo->cmdSize);
            return -1;
        }
    }
    return 0;
}

int MipiTxSetCmd(const CmdInfoTag *cmdInfo)
{
    int ret;

    ret = MipiTxCheckSetCmdInfo(cmdInfo);
    if (ret < 0) {
        HDF_LOGE("mipi_tx check combo_dev config failed!\n");
        return ret;
    }
    return MipiTxDrvSetCmdInfo(cmdInfo);
}

static int MipiTxCheckGetCmdInfo(const GetCmdInfoTag *getCmdInfo)
{
    if (g_enDev == TRUE) {
        HDF_LOGE("mipi_tx dev has enable!\n");
        return -1;
    }
    if (g_enDevCfg != TRUE) {
        HDF_LOGE("mipi_tx dev has not config!\n");
        return -1;
    }
    if (getCmdInfo->devno != 0) {
        HDF_LOGE("mipi_tx dev devno %d err!\n", getCmdInfo->devno);
        return -1;
    }
    if ((getCmdInfo->getDataSize == 0) || (getCmdInfo->getDataSize > MIPI_TX_GET_DATA_SIZE)) {
        HDF_LOGE("mipi_tx dev getDataSize %d err!\n", getCmdInfo->getDataSize);
        return -1;
    }
    if (getCmdInfo->getData == NULL) {
        HDF_LOGE("mipi_tx dev getData is null!\n");
        return -1;
    }
    return 0;
}

int MipiTxGetCmd(GetCmdInfoTag *getCmdInfo)
{
    int ret;

    ret = MipiTxCheckGetCmdInfo(getCmdInfo);
    if (ret < 0) {
        HDF_LOGE("mipi_tx check combo_dev config failed!\n");
        return ret;
    }
    return MipiTxDrvGetCmdInfo(getCmdInfo);
}

static void MipiTxEnable(void)
{
    OutPutModeTag mode;

    mode = g_mipiTxDevCtx.devCfg.outputMode;
    MipiTxDrvEnableInput(mode);
    g_enDev = TRUE;
}

static void MipiTxDisable(void)
{
    MipiTxDrvDisableInput();
    g_enDev = FALSE;
    g_enDevCfg = FALSE;
}

static long MipiTxIoctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    void *pArg = (void *)arg;

    if (filep == NULL || pArg == NULL) {
        HDF_LOGE("invalid input param");
        return -1;
    }
    if (MipiTxSemaDownInterruptable(&g_mipiTxSem)) {
        return -1;
    }
    switch (cmd) {
        case HI_MIPI_TX_SET_DEV_CFG:
            ret = MipiTxSetComboDevCfg((ComboDevCfgTag *)pArg);
            if (ret < 0) {
                HDF_LOGE("mipi_tx set combo_dev config failed!\n");
            }
            break;
        case HI_MIPI_TX_SET_CMD:
            ret = MipiTxSetCmd((CmdInfoTag *)pArg);
            if (ret < 0) {
                HDF_LOGE("mipi_tx set cmd failed!\n");
            }
            break;
        case HI_MIPI_TX_GET_CMD:
            ret = MipiTxGetCmd((GetCmdInfoTag *)pArg);
            if (ret < 0) {
                HDF_LOGE("mipi_tx get cmd failed!\n");
            }
            break;
        case HI_MIPI_TX_ENABLE:
            MipiTxEnable();
            break;
        case HI_MIPI_TX_DISABLE:
            MipiTxDisable();
            break;
        default:
            HDF_LOGE("invalid mipi_tx ioctl cmd");
            ret = -1;
            break;
    }
    MipiTxUp(&g_mipiTxSem);
    return ret;
}

static int MipiTxInit(int smooth)
{
    return MipiTxDrvInit(smooth);
}

static void MipiTxExit(void)
{
    MipiTxDrvExit();
}

static int MipiTxOpen(struct inode *inode, struct file *filep)
{
    (void)inode;
    (void)filep;
    return 0;
}

static int MipiTxRelease(struct inode *inode, struct file *filep)
{
    (void)inode;
    (void)filep;
    return 0;
}

static void MipiTxProcDevShow(struct seq_file *s)
{
    ComboDevCfgTag *devCfg = NULL;
    SyncInfoTag *syncInfo = NULL;
    devCfg = (ComboDevCfgTag *)&g_mipiTxDevCtx.devCfg;
    syncInfo = (SyncInfoTag *)&g_mipiTxDevCtx.devCfg.syncInfo;

    /* mipi tx device config */
    seq_printf(s, "----------MIPI_Tx DEV CONFIG---------------------------\n");
    seq_printf(s, "%8s%8s%8s%8s%8s%15s%15s%15s%15s%15s\n",
        "devno", "lane0", "lane1", "lane2", "lane3", "output_mode", "phy_data_rate", "pixel_clk(KHz)",
        "video_mode", "output_fmt");
    seq_printf(s, "%8d%8d%8d%8d%8d%15d%15d%15d%15d%15d\n",
        devCfg->devno,
        devCfg->laneId[0],
        devCfg->laneId[1],
        devCfg->laneId[2], /* lina id 2 */
        devCfg->laneId[3], /* lina id 3 */
        devCfg->outputMode,
        devCfg->phyDataRate,
        devCfg->pixelClk,
        devCfg->videoMode,
        devCfg->outputFormat);
    seq_printf(s, "\r\n");
    /* mipi tx device sync config */
    seq_printf(s, "----------MIPI_Tx SYNC CONFIG---------------------------\n");
    seq_printf(s, "%14s%14s%14s%14s%14s%14s%14s%14s%14s\n",
        "pkt_size", "hsa_pixels", "hbp_pixels", "hline_pixels", "vsa_lines", "vbp_lines",
        "vfp_lines", "active_lines", "edpi_cmd_size");
    seq_printf(s, "%14d%14d%14d%14d%14d%14d%14d%14d%14d\n",
        syncInfo->vidPktSize,
        syncInfo->vidHsaPixels,
        syncInfo->vidHbpPixels,
        syncInfo->vidHlinePixels,
        syncInfo->vidVsaLines,
        syncInfo->vidVbpLines,
        syncInfo->vidVfpLines,
        syncInfo->vidActiveLines,
        syncInfo->edpiCmdSize);
    seq_printf(s, "\r\n");
}

static void MipiTxProcDevStatusShow(struct seq_file *s)
{
    MipiTxDevPhyTag phyCtx;

    MipiTxDrvGetDevStatus(&phyCtx);
    /* mipi tx phy status */
    seq_printf(s, "----------MIPI_Tx DEV STATUS---------------------------\n");
    seq_printf(s, "%8s%8s%8s%8s%8s%8s%8s\n",
        "width", "height", "HoriAll", "VertAll", "hbp", "hsa", "vsa");
    seq_printf(s, "%8u%8u%8u%8u%8u%8u%8u\n",
        phyCtx.hactDet,
        phyCtx.vactDet,
        phyCtx.hallDet,
        phyCtx.vallDet,
        phyCtx.hbpDet,
        phyCtx.hsaDet,
        phyCtx.vsaDet);
    seq_printf(s, "\r\n");
}

static int MipiTxProcShow(struct seq_file *m, void *v)
{
    seq_printf(m, "\nModule: [MIPI_TX], Build Time["__DATE__", "__TIME__"]\n");
    MipiTxProcDevShow(m);
    MipiTxProcDevStatusShow(m);
    HDF_LOGI("%s: v %p", __func__, v);
    return 0;
}

static int MipiTxProcOpen(struct inode *inode, struct file *file)
{
    return single_open(file, MipiTxProcShow, NULL);
}

static const struct file_operations g_procMipiTxOps = {
    .open = MipiTxProcOpen,
    .read = seq_read,
};

static const struct file_operations g_mipiTxfOps = {
    .open = MipiTxOpen,
    .release = MipiTxRelease,
    .unlocked_ioctl = MipiTxIoctl,
};

static int MipiVfsInit(int smooth)
{
    int ret;

    (void)smooth;
    /* 0660 : node mode */
    ret = MipiDsiRegisterDevice(MIPI_TX_DEV_NAME, 0, 0660, (struct file_operations *)&g_mipiTxfOps);
    if (ret < 0) {
        HDF_LOGE("%s: mipi dsi reg dev fail:%d", __func__, ret);
        return ret;
    }
    ret = MipiDsiProcRegister(MIPI_TX_PROC_NAME, 0, 0440, &g_procMipiTxOps); /* 0440 : proc file mode */
    if (ret < 0) {
        MipiDsiUnregisterDevice(0);
        HDF_LOGE("%s: mipi dsi reg proc fail:%d", __func__, ret);
        return ret;
    }

    return 0;
}

static void MipiVfsRemove(void)
{
    MipiDsiUnregisterDevice(0);
    MipiDsiProcUnregister(MIPI_TX_PROC_NAME, 0);
}

int MipiTxModuleInit(int smooth)
{
    int ret;

    (void)MipiVfsInit;

    ret = MipiTxInit(smooth);
    if (ret != 0) {
        HDF_LOGE("hi_mipi_init failed!\n");
        goto fail1;
    }
    ret = MipiTxSemaInit(&g_mipiTxSem, 1);
    if (ret != 0) {
        HDF_LOGE("init sema error!\n");
        goto fail2;
    }
    HDF_LOGE("load mipi_tx driver successful!\n");
    return 0;

fail2:
    MipiTxExit();
fail1:
    (void)MipiVfsRemove;

    HDF_LOGE("load mipi_tx driver failed!\n");
    return ret;
}

void MipiTxModuleExit(void)
{
    MipiTxSemaDestroy(&g_mipiTxSem);
    MipiTxExit();
    (void)MipiVfsRemove;
    HDF_LOGI("unload mipi_tx driver ok!\n");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
