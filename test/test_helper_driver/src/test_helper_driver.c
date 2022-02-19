/*
 * test_helper_driver.c
 *
 * test helper driver on linux
 *
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

#include "devmgr_service.h"
#include "devsvc_manager_clnt.h"
#include "hdf_device_object.h"
#include "hdf_driver_module.h"
#include "hdf_log.h"
#include "hdf_pm.h"
#include "osal_file.h"
#include "osal_mem.h"
#include "sample_driver_test.h"

#define HDF_LOG_TAG test_help_driver

static int32_t HelperDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t num = 0;
    if (cmdId != 1) {
        HDF_LOGE("%s:unknown comd id %d", __func__, cmdId);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (!HdfSbufReadInt32(data, &num)) {
        HDF_LOGE("%s:failed to read parm", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteInt32(reply, num)) {
        HDF_LOGE("%s:failed to write parm", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t HdfHelperDriverBind(struct HdfDeviceObject *deviceObject)
{
    static struct IDeviceIoService testService = {
        .Open = NULL,
        .Dispatch = HelperDriverDispatch,
        .Release = NULL,
    };
    HDF_LOGI("%s: called", __func__);
    if (deviceObject == NULL) {
        return HDF_FAILURE;
    }

    deviceObject->service = &testService;
    return HDF_SUCCESS;
}

static int32_t HdfHelperDriverInit(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    HDF_LOGI("%s: called", __func__);
    return HDF_SUCCESS;
}

static void HdfHelperDriverRelease(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    HDF_LOGI("%s: called", __func__);
}

static struct HdfDriverEntry g_helperDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "hdf_test_helper",
    .Bind = HdfHelperDriverBind,
    .Init = HdfHelperDriverInit,
    .Release = HdfHelperDriverRelease,
};

HDF_DRIVER_MODULE(g_helperDriverEntry);
