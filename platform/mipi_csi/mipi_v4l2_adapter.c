/*
 * mipi_v4l2_adapter.c
 *
 * Mipi v4l2 adapter driver.
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

#include <asm/unaligned.h>
#include <linux/acpi.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fwnode.h>
#include "hdf_log.h"
#include "mipi_csi_core.h"

#define HDF_LOG_TAG          mipi_v4l2_adapter
#define SENSOR_FLL_MAX       0xffff
#define PIXEL_RATE_DIVISOR   10
#define LINKS_COUNT          2
#define LANES_COUNT          4
#define CTRLS_COUNT          10

/* Mode : resolution and related config&values */
struct CameraSensorMode {
    /* V-timing */
    u32 fll_def;
    u32 fll_min;

    /* H-timing */
    u32 llp;
};

struct CameraDrvData {
    struct v4l2_subdev *sd;
    struct media_pad *pad;

    /* V4L2 Controls */
    struct v4l2_ctrl_handler *ctrl_handler;
    struct v4l2_ctrl *link_freq;
    struct v4l2_ctrl *pixel_rate;
    struct v4l2_ctrl *vblank;
    struct v4l2_ctrl *hblank;
    struct v4l2_ctrl *exposure;
    struct v4l2_ctrl *vflip;
    struct v4l2_ctrl *hflip;

    /* Current mode */
    const struct CameraSensorMode *cur_mode;
    s64 link_freqs; /* CSI-2 link frequencies, Application under v4l2 framework */

    /* Streaming on/off */
    bool streaming;
};

struct AdapterDrvData {
    struct CameraDrvData *camera;
    ComboDevAttr *attr;

    struct v4l2_subdev_format fmt;
    /*
     * Mutex for serialized access:
     * Protect sensor set pad format and start/stop streaming safely.
     * Protect access to sensor v4l2 controls.
     */
    struct mutex mutex;
};

static struct AdapterDrvData g_adapterDrvData;

/** 
 * Get bayer order based on flip setting.
 * ref. linuxtv.org/downloads/v4l-dvb-apis/userspace-api/v4l/subdev-formats.html?highlight=media_bus_fmt_uv8_1x8
 */
static u32 LinuxGetFormatCode(DataType dataType)
{
    u32 code = MEDIA_BUS_FMT_SGBRG12_1X12;

    switch (dataType) {
        case DATA_TYPE_RAW_8BIT:
            code = MEDIA_BUS_FMT_SBGGR8_1X8;
            break;
        case DATA_TYPE_RAW_10BIT:
            code = MEDIA_BUS_FMT_SBGGR10_1X10;
            break;
        case DATA_TYPE_RAW_12BIT:
            code = MEDIA_BUS_FMT_SBGGR12_1X12;
            break;
        case DATA_TYPE_RAW_14BIT:
            code = MEDIA_BUS_FMT_SBGGR14_1X14;
            break;
        case DATA_TYPE_RAW_16BIT:
            code = MEDIA_BUS_FMT_SBGGR16_1X16;
            break;
        case DATA_TYPE_YUV420_8BIT_NORMAL:
            code = MEDIA_BUS_FMT_Y8_1X8;
            break;
        case DATA_TYPE_YUV420_8BIT_LEGACY:
            code = MEDIA_BUS_FMT_Y8_1X8;
            break;
        case DATA_TYPE_YUV422_8BIT:
            code = MEDIA_BUS_FMT_YUYV8_1X16;
            break;
        case DATA_TYPE_YUV422_PACKED:
            code = MEDIA_BUS_FMT_YVYU8_1X16;
            break;
        default:
            break;
    }

    return code;
}

static int LinuxEnumMbusCode(struct v4l2_subdev *sd,
    struct v4l2_subdev_pad_config *cfg, struct v4l2_subdev_mbus_code_enum *code)
{
    struct AdapterDrvData *drvData = &g_adapterDrvData;
    (void)sd;
    (void)cfg;

    if (code == NULL) {
        HDF_LOGE("%s: code is NULL.", __func__);
        return -EINVAL;
    }
    if (code->index > 0) {
        HDF_LOGE("%s: code->index is invalid.", __func__);
        return -EINVAL;
    }

    mutex_lock(&drvData->mutex);
    code->code = drvData->fmt.format.code;
    mutex_unlock(&drvData->mutex);

    return 0;
}

static void LinuxUpdatePadFormat(struct AdapterDrvData *drvData,
    const struct CameraSensorMode *mode, struct v4l2_subdev_format *fmt)
{
    (void)mode;

    if ((drvData == NULL) || (fmt == NULL)) {
        HDF_LOGE("%s: drvData or fmt is NULL.", __func__);
        return;
    }
    fmt->format = drvData->fmt.format;
}

static int LinuxSetPadFormat(struct v4l2_subdev *sd, struct v4l2_subdev_pad_config *cfg,
    struct v4l2_subdev_format *fmt)
{
    s32 vblank_def;
    s32 vblank_min;
    s64 h_blank;
    u64 pixel_rate;
    u32 height;
    struct AdapterDrvData *drvData = &g_adapterDrvData;
    struct CameraDrvData *camera = drvData->camera;
    const struct CameraSensorMode *mode = camera->cur_mode;
    ImgRect *rect = &drvData->attr->imgRect;

    (void)sd;
    (void)cfg;
    mutex_lock(&drvData->mutex);
    /*
     * Only one bayer order is supported.
     * It depends on the flip settings.
     */
    fmt->format.code = drvData->fmt.format.code;
    LinuxUpdatePadFormat(drvData, mode, fmt);

    pixel_rate = camera->link_freqs * LINKS_COUNT * LANES_COUNT;
    do_div(pixel_rate, PIXEL_RATE_DIVISOR);
    __v4l2_ctrl_s_ctrl_int64(camera->pixel_rate, pixel_rate);
    /* Update limits and set FPS to default */
    height = rect->height;
    vblank_def = camera->cur_mode->fll_def - height;
    vblank_min = camera->cur_mode->fll_min - height;
    height = SENSOR_FLL_MAX - height;
    __v4l2_ctrl_modify_range(camera->vblank, vblank_min, height, 1, vblank_def);
    __v4l2_ctrl_s_ctrl(camera->vblank, vblank_def);
    h_blank = mode->llp - rect->width;
    /*
     * Currently hblank is not changeable.
     * So FPS control is done only by vblank.
     */
    __v4l2_ctrl_modify_range(camera->hblank, h_blank, h_blank, 1, h_blank);
    mutex_unlock(&drvData->mutex);

    return 0;
}

static int SetStream(int enable)
{
    struct AdapterDrvData *drvData = &g_adapterDrvData;
    struct CameraDrvData *camera = drvData->camera;

    mutex_lock(&drvData->mutex);
    if (camera->streaming == enable) {
        mutex_unlock(&drvData->mutex);
        HDF_LOGE("%s: streaming-flag is not change.", __func__);
        return 0;
    }

    camera->streaming = enable;

    /* vflip and hflip cannot change during streaming */
    __v4l2_ctrl_grab(camera->vflip, enable);
    __v4l2_ctrl_grab(camera->hflip, enable);
    mutex_unlock(&drvData->mutex);

    return 0;
}

static const struct v4l2_subdev_video_ops g_cameraVideoOps = {
    .s_stream = NULL,
};

static const struct v4l2_subdev_pad_ops g_cameraPadOps = {
    .enum_mbus_code = LinuxEnumMbusCode,
    .set_fmt = LinuxSetPadFormat,
};

static const struct v4l2_subdev_ops g_cameraSubdevOps = {
    .video = &g_cameraVideoOps,
    .pad = &g_cameraPadOps,
};

/* Initialize control handlers */
static int LinuxInitControls(struct AdapterDrvData *drvData)
{
    s64 vblank_def;
    s64 vblank_min;
    s64 hblank;
    u64 pixel_rate;
    int ret;

    struct CameraDrvData *camera = drvData->camera;
    struct v4l2_ctrl_handler *ctrl_hdlr = camera->ctrl_handler;
    const struct CameraSensorMode *mode = camera->cur_mode;
    ImgRect *rect = &(drvData->attr->imgRect);

    if (ctrl_hdlr == NULL) {
        ret = v4l2_ctrl_handler_init(ctrl_hdlr, CTRLS_COUNT);
        if (ret) {
            HDF_LOGE("%s: [v4l2_ctrl_handler_init] failed.", __func__);
            return ret;
        }
    }
    ctrl_hdlr->lock = &drvData->mutex;
    if (camera->link_freq == NULL) {
        camera->link_freq = v4l2_ctrl_new_std(ctrl_hdlr, NULL,
            V4L2_CID_LINK_FREQ, camera->link_freqs, camera->link_freqs, 1, camera->link_freqs);
        if (camera->link_freq) {
            camera->link_freq->flags |= V4L2_CTRL_FLAG_READ_ONLY;
        }
    }

    if (camera->pixel_rate == NULL) {
        /* pixel_rate = link_freq * 2 * nr_of_lanes / bits_per_sample */
        pixel_rate = camera->link_freqs * LINKS_COUNT * LANES_COUNT;
        do_div(pixel_rate, PIXEL_RATE_DIVISOR);
        /* By default, PIXEL_RATE is read only */
        camera->pixel_rate = v4l2_ctrl_new_std(ctrl_hdlr, NULL,
            V4L2_CID_PIXEL_RATE, pixel_rate, pixel_rate, 1, pixel_rate);
    }

    /* Initialize vblank/hblank/exposure parameters based on current mode */
    if (camera->vblank == NULL) {
        vblank_def = mode->fll_def - rect->height;
        vblank_min = mode->fll_min - rect->height;
        camera->vblank = v4l2_ctrl_new_std(ctrl_hdlr, NULL,
            V4L2_CID_VBLANK, vblank_min, SENSOR_FLL_MAX - rect->height, 1, vblank_def);
    }

    if (camera->hblank == NULL) {
        hblank = mode->llp - rect->width;
        camera->hblank = v4l2_ctrl_new_std(ctrl_hdlr, NULL,
            V4L2_CID_HBLANK, hblank, hblank, 1, hblank);
        if (camera->hblank) {
            camera->hblank->flags |= V4L2_CTRL_FLAG_READ_ONLY;
        }
    }
    if (ctrl_hdlr->error) {
        ret = ctrl_hdlr->error;
        HDF_LOGE("%s: control init failed: %d", __func__, ret);
        v4l2_ctrl_handler_free(ctrl_hdlr);
        return ret;
    }
    camera->sd->ctrl_handler = ctrl_hdlr;

    return 0;
}

static int32_t MipiCsiAdapterTraceMipiCfg(ComboDevAttr *attr)
{
    unsigned int i;
    MipiDevAttr *cfg = NULL;

    if (attr == NULL) {
        HDF_LOGE("%s: attr is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    cfg = &(attr->mipiAttr);
    HDF_LOGD("%s: inputDataType = %d, wdrMode = %d", __func__, cfg->inputDataType, cfg->wdrMode);
    for (i = 0; i < MIPI_LANE_NUM; i++) {
        HDF_LOGD("laneId[%d] = %d", i, cfg->laneId[i]);
    }

    HDF_LOGD("%s: inputMode = %d, dataRate = %d, imgRect(x = %d, y = %d, width = %d, height = %d).",
        __func__, attr->inputMode, attr->dataRate, attr->imgRect.x, attr->imgRect.y,
        attr->imgRect.width, attr->imgRect.height);

    return HDF_SUCCESS;
}

static int32_t MipiCsiAdapterTraceCameraCfg(struct CameraDrvData *camera)
{
    const struct CameraSensorMode *mode = NULL;
    if (camera == NULL) {
        HDF_LOGE("%s: camera is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    mode = camera->cur_mode;
    HDF_LOGD("%s: link_freq = %lld", __func__, camera->link_freqs);
    HDF_LOGD("%s: fll_def = %d, fll_min = %d, llp = %d.", __func__, mode->fll_def, mode->fll_min, mode->llp);

    return HDF_SUCCESS;
}

static int32_t MipiCsiAdapterSetComboDevAttr(struct MipiCsiCntlr *cntlr, ComboDevAttr *pAttr)
{
    int32_t ret;
    struct AdapterDrvData *drvData = (struct AdapterDrvData *)cntlr->priv;
    struct v4l2_mbus_framefmt *fmt = NULL;

    if ((drvData == NULL) || (pAttr == NULL)) {
        HDF_LOGE("%s: drvData or pAttr is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    drvData->attr = pAttr;
    fmt = &drvData->fmt.format;
    fmt->width = pAttr->imgRect.width;
    fmt->height = pAttr->imgRect.height;
    fmt->code = LinuxGetFormatCode(pAttr->mipiAttr.inputDataType);
    fmt->field = V4L2_FIELD_NONE;

    ret = MipiCsiAdapterTraceMipiCfg(pAttr);

    return ret;
}

static int32_t MipiCsiAdapterResetRx(struct MipiCsiCntlr *cntlr, uint8_t comboDev)
{
    int32_t ret;

    (void)cntlr;
    ret = SetStream(1);
    return (ret == 0) ? HDF_SUCCESS : HDF_FAILURE;
}

static int32_t MipiCsiAdapterUnresetRx(struct MipiCsiCntlr *cntlr, uint8_t comboDev)
{
    int32_t ret;

    (void)cntlr;
    ret = SetStream(0);
    return (ret == 0) ? HDF_SUCCESS : HDF_FAILURE;
}

static int32_t MipiCsiAdapterProbeV4l2(struct MipiCsiCntlr *cntlr)
{
    int32_t ret;
    struct AdapterDrvData *drvData = (struct AdapterDrvData *)cntlr->priv;
    struct CameraDrvData *camera = NULL;

    if ((drvData == NULL) || (drvData->camera == NULL)) {
        HDF_LOGE("%s: drvData or drvData->camera is NULL.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    camera = drvData->camera;
    /* Initialize subdev */
    v4l2_subdev_init(camera->sd, &g_cameraSubdevOps);
    ret = LinuxInitControls(drvData);
    if (ret) {
        HDF_LOGE("%s: failed to init controls: %d", __func__, ret);
        v4l2_ctrl_handler_free(camera->sd->ctrl_handler);
        return HDF_FAILURE;
    }

    mutex_init(&drvData->mutex);
    return HDF_SUCCESS;
}

static int32_t MipiCsiAdapterSetDrvData(struct MipiCsiCntlr *cntlr, void *cameraData)
{
    int32_t ret;
    struct AdapterDrvData *drvData = (struct AdapterDrvData *)cntlr->priv;

    if ((drvData == NULL) || (cameraData == NULL)) {
        HDF_LOGE("%s: drvData or cameraData is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drvData->camera = (struct CameraDrvData *)cameraData;
    ret = MipiCsiAdapterTraceCameraCfg(drvData->camera);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: trace Camera Cfg failed!", __func__);
        return HDF_FAILURE;
    }
    ret = MipiCsiAdapterProbeV4l2(cntlr);

    return ret;
}

static struct MipiCsiCntlr g_mipiRx = {
    .devNo = 0
};

static struct MipiCsiCntlrMethod g_method = {
    .setComboDevAttr = MipiCsiAdapterSetComboDevAttr,
    .resetRx = MipiCsiAdapterResetRx,
    .unresetRx = MipiCsiAdapterUnresetRx,
    .setDrvData = MipiCsiAdapterSetDrvData,
};

static void MipiCsiAdapterRemoveV4l2(struct MipiCsiCntlr *cntlr)
{
    struct AdapterDrvData *drvData = (struct AdapterDrvData *)cntlr->priv;
    struct CameraDrvData *camera = NULL;

    if ((drvData == NULL) || (drvData->camera == NULL)) {
        HDF_LOGE("%s: drvData or drvData->camera is NULL!", __func__);
        return;
    }

    camera = drvData->camera;
    if ((camera->sd != NULL) && (camera->sd->ctrl_handler != NULL)) {
        v4l2_ctrl_handler_free(camera->sd->ctrl_handler);
        camera->sd->ctrl_handler = NULL;
    }
    mutex_destroy(&drvData->mutex);
    drvData->attr = NULL;
    drvData->camera = NULL;
}

static int32_t MipiCsiAdapterBind(struct HdfDeviceObject *device)
{
    int32_t ret;

    HDF_LOGI("%s: enter.", __func__);
    g_mipiRx.priv = &g_adapterDrvData;
    g_mipiRx.ops = &g_method;
    ret = MipiCsiRegisterCntlr(&g_mipiRx, device);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HDF_LOGI("%s: success.", __func__);

    return HDF_SUCCESS;
}

static int32_t MipiCsiAdapterInit(struct HdfDeviceObject *device)
{
    (void)device;

    HDF_LOGI("%s: success.", __func__);
    return HDF_SUCCESS;
}

static void MipiCsiAdapterRelease(struct HdfDeviceObject *device)
{
    struct MipiCsiCntlr *cntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL.", __func__);
        return;
    }
    cntlr = MipiCsiCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is NULL.", __func__);
        return;
    }
    MipiCsiAdapterRemoveV4l2(cntlr);
    MipiCsiUnregisterCntlr(cntlr);
    cntlr->priv = NULL;

    HDF_LOGI("%s: success.", __func__);
}

struct HdfDriverEntry g_mipiCsiLinuxDriverEntry = {
    .moduleVersion = 1,
    .Bind = MipiCsiAdapterBind,
    .Init = MipiCsiAdapterInit,
    .Release = MipiCsiAdapterRelease,
    .moduleName = "linux_mipi_csi_adapter",
};
HDF_INIT(g_mipiCsiLinuxDriverEntry);

