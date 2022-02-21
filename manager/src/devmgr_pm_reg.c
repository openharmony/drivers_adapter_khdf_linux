/*
 * devmgr_pm.c
 *
 * HDF power manager of linux
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

#include <linux/notifier.h>
#include <linux/suspend.h>

#include "devmgr_service.h"
#include "hdf_base.h"
#include "hdf_log.h"

#define HDF_LOG_TAG devmgr_pm

static int DevmgrPmSuspend(void)
{
    HDF_LOGD("%s enter", __func__);
    struct IDevmgrService *devmgrService = DevmgrServiceGetInstance();
    if (devmgrService == NULL) {
        return HDF_FAILURE;
    }

    if (devmgrService->PowerStateChange(devmgrService, POWER_STATE_SUSPEND) != HDF_SUCCESS) {
        HDF_LOGE("%s drivers suspend failed", __func__);
        devmgrService->PowerStateChange(devmgrService, POWER_STATE_RESUME);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int DevmgrPmResume(void)
{
    HDF_LOGD("%s enter", __func__);
    struct IDevmgrService *devmgrService = DevmgrServiceGetInstance();
    if (devmgrService == NULL) {
        return HDF_FAILURE;
    }

    devmgrService->PowerStateChange(devmgrService, POWER_STATE_RESUME);
    HDF_LOGD("%s resume done", __func__);
    return HDF_SUCCESS;
}

static int DevmgrPmNotifier(struct notifier_block *nb, unsigned long mode, void *data)
{
    int ret = HDF_SUCCESS;
    switch (mode) {
        case PM_SUSPEND_PREPARE:
            ret = DevmgrPmSuspend();
            break;
        case PM_POST_SUSPEND:
            ret = DevmgrPmResume();
            break;
        default:
            break;
    }
    return ret;
}
static struct notifier_block PmNotifier = {
    .notifier_call = DevmgrPmNotifier,
};

int DevMgrPmRegister(void)
{
    int ret;

    HDF_LOGD("%s enter", __func__);
    ret = register_pm_notifier(&PmNotifier);
    if (ret) {
        HDF_LOGE("%s register_pm_notifier failed", __func__);
    }

    return ret;
}
EXPORT_SYMBOL(DevMgrPmRegister);
