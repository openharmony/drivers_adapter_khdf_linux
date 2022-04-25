/*
 *
 * hdf_kevent.c
 *
 * HDF kevent implement for linux
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

#include <linux/completion.h>
#include <linux/fb.h>
#include <linux/notifier.h>
#include <linux/suspend.h>

#include "devmgr_service_clnt.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "hdf_log.h"
#include "hdf_power_state.h"
#include "hdf_sbuf.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "osal_sysevent.h"

#define HDF_LOG_TAG kevent

#define EVENT_TIMEOUT_JIFFIES 5
#define EVENT_DEFAULT_SIZE    64
#define KEVENT_COMPLETE_EVENT 1

struct HdfKeventWait {
    struct completion comp;
    struct DListHead listNode;
    uint32_t waitCount;
};

struct HdfKeventClient {
    struct HdfDeviceIoClient *ioClient;
    struct DListHead listNode;
};

struct HdfKeventModule {
    struct HdfDeviceObject *devObj;
    struct notifier_block keventPmNotifier;
    struct notifier_block fbNotifier;
    struct DListHead waitList;
    struct OsalMutex mutex;
    struct OsalMutex clientMutex;
    struct DListHead clientList;
    int32_t clientCount;
};

static struct HdfKeventModule *g_keventModule = NULL;

static struct HdfKeventWait *HdfKeventWaitAlloc(void)
{
    struct HdfKeventWait *wait = OsalMemAlloc(sizeof(struct HdfKeventWait));
    if (wait != NULL) {
        init_completion(&wait->comp);
    }

    return wait;
}

static void HdfKeventWaitFree(struct HdfKeventWait *wait)
{
    if (wait != NULL) {
        OsalMemFree(wait);
    }
}

static struct HdfSBuf *PrepareEvent(
    struct HdfKeventWait **wait, uint64_t class, int32_t eventId, const char *content, bool sync)
{
    struct HdfSBuf *eventBuf = NULL;
    struct HdfSysEvent sysEvent;
    struct HdfKeventWait *eventWait = NULL;

    eventBuf = HdfSbufObtain(EVENT_DEFAULT_SIZE);
    if (eventBuf == NULL) {
        HDF_LOGE("%s: failed to obtain sbuf", __func__);
        return NULL;
    }

    sysEvent.eventClass = class;
    sysEvent.eventid = eventId;
    sysEvent.syncToken = 0;

    if (sync) {
        eventWait = HdfKeventWaitAlloc();
        if (eventWait == NULL) {
            HdfSbufRecycle(eventBuf);
            return NULL;
        }
        sysEvent.syncToken = (uint64_t)eventWait;
    }

    if (!HdfSbufWriteBuffer(eventBuf, &sysEvent, sizeof(sysEvent))) {
        HdfSbufRecycle(eventBuf);
        return NULL;
    }

    if (!HdfSbufWriteString(eventBuf, content)) {
        HdfSbufRecycle(eventBuf);
        return NULL;
    }

    *wait = eventWait;
    return eventBuf;
}

static int SendKevent(
    struct HdfKeventModule *keventModule, uint16_t class, int32_t event, const char *content, bool sync)
{
    struct HdfSBuf *eventBuf = NULL;
    struct HdfKeventWait *wait = NULL;
    struct HdfKeventClient *client = NULL;
    int ret;

    if (keventModule->clientCount <= 0) {
        return 0;
    }

    eventBuf = PrepareEvent(&wait, class, event, content, sync);
    if (eventBuf == NULL) {
        return HDF_FAILURE;
    }

    OsalMutexLock(&keventModule->mutex);
    if (sync) {
        DListInsertTail(&wait->listNode, &keventModule->waitList);
    }

    OsalMutexLock(&keventModule->clientMutex);
    if (sync) {
        wait->waitCount = keventModule->clientCount;
    }

    DLIST_FOR_EACH_ENTRY(client, &keventModule->clientList, struct HdfKeventClient, listNode) {
        ret = HdfDeviceSendEventToClient(client->ioClient, HDF_SYSEVENT, eventBuf);
        if (ret) {
            HDF_LOGE("%s: failed to send device event, %d", __func__, ret);
        }
    }
    OsalMutexUnlock(&keventModule->clientMutex);

    if (sync) {
        OsalMutexUnlock(&keventModule->mutex);
        if (wait_for_completion_timeout(&wait->comp, EVENT_TIMEOUT_JIFFIES * HZ) == 0) {
            HDF_LOGE("%s:send kevent timeout, class=%d, event=%d", __func__, class, event);
            ret = HDF_ERR_TIMEOUT;
        }

        OsalMutexLock(&keventModule->mutex);
        DListRemove(&wait->listNode);
        HdfKeventWaitFree(wait);
    }

    OsalMutexUnlock(&keventModule->mutex);
    HdfSbufRecycle(eventBuf);
    return ret;
}

int32_t HdfSysEventSend(uint64_t eventClass, uint32_t event, const char *content, bool sync)
{
    struct HdfKeventModule *keventmodule = g_keventModule;
    if (keventmodule == NULL) {
        return HDF_DEV_ERR_OP;
    }

    return SendKevent(keventmodule, eventClass, event, content, sync);
}

static int32_t KernalSpacePmNotify(int32_t powerEvent)
{
    uint32_t pmStatus = POWER_STATE_MAX;
    struct DevmgrServiceClnt *devmgrClnt = NULL;

    switch (powerEvent) {
        case KEVENT_POWER_SUSPEND:
            pmStatus = POWER_STATE_SUSPEND;
            break;
        case KEVENT_POWER_DISPLAY_OFF:
            pmStatus = POWER_STATE_DOZE_SUSPEND;
            break;
        case KEVENT_POWER_RESUME:
            pmStatus = POWER_STATE_RESUME;
            break;
        case KEVENT_POWER_DISPLAY_ON:
            pmStatus = POWER_STATE_DOZE_RESUME;
            break;
        default:
            return HDF_ERR_INVALID_PARAM;
    }

    devmgrClnt = DevmgrServiceClntGetInstance();
    if (devmgrClnt == NULL || devmgrClnt->devMgrSvcIf == NULL) {
        return HDF_FAILURE;
    }

    return devmgrClnt->devMgrSvcIf->PowerStateChange(devmgrClnt->devMgrSvcIf, pmStatus);
}

static int32_t KeventPmNotifierFn(struct notifier_block *nb, unsigned long action, void *dummy)
{
    struct HdfKeventModule *keventModule = NULL;
    int32_t powerEvent;
    bool sync = false;
    int ret = 0;

    keventModule = CONTAINER_OF(nb, struct HdfKeventModule, keventPmNotifier);
    HDF_LOGI("%s:action=%d", __func__, action);
    switch (action) {
        case PM_SUSPEND_PREPARE:
            HDF_LOGI("%s:receive suspend event", __func__);
            powerEvent = KEVENT_POWER_SUSPEND;
            sync = true;
            break;
        case PM_POST_SUSPEND:
            HDF_LOGI("%s:receive resume event", __func__);
            powerEvent = KEVENT_POWER_RESUME;
            break;
        default:
            return 0;
    }

    ret = SendKevent(keventModule, HDF_SYSEVENT_CLASS_POWER, powerEvent, NULL, sync);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to notify userspace pm status");
    }

    return KernalSpacePmNotify(powerEvent);
}

static int32_t KeventFbNotifierFn(struct notifier_block *nb, unsigned long event, void *data)
{
    int *blank = NULL;
    struct fb_event *fbEvent = data;
    struct HdfKeventModule *keventModule = NULL;
    int32_t powerEvent;
    bool sync = false;
    int ret = 0;

    if (event != FB_EVENT_BLANK) {
        return 0;
    }

    if (fbEvent == NULL || fbEvent->data == NULL) {
        return 0;
    }

    keventModule = CONTAINER_OF(nb, struct HdfKeventModule, fbNotifier);
    blank = fbEvent->data;

    HDF_LOGI("%s:blank=%d", __func__, *blank);
    switch (*blank) {
        case FB_BLANK_UNBLANK:
            HDF_LOGI("%s:receive display on event", __func__);
            powerEvent = KEVENT_POWER_DISPLAY_ON;
            break;
        default:
            HDF_LOGI("%s:receive display off event", __func__);
            powerEvent = KEVENT_POWER_DISPLAY_OFF;
            sync = true;
            break;
    }

    ret = SendKevent(keventModule, HDF_SYSEVENT_CLASS_POWER, powerEvent, NULL, sync);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to notify userspace pm status");
    }

    return KernalSpacePmNotify(powerEvent);
}

void CompleteKevent(struct HdfKeventModule *keventModule, struct HdfSBuf *tokenBuffer)
{
    uint64_t token = 0;
    struct HdfKeventWait *wait = NULL;

    if (tokenBuffer == NULL || !HdfSbufReadUint64(tokenBuffer, &token)) {
        return;
    }

    OsalMutexLock(&keventModule->mutex);
    DLIST_FOR_EACH_ENTRY(wait, &keventModule->waitList, struct HdfKeventWait, listNode) {
        if (token == (uint64_t)wait) {
            wait->waitCount--;
            if (wait->waitCount == 0) {
                complete(&wait->comp);
            }
        }
    }
    OsalMutexUnlock(&keventModule->mutex);
}

static int32_t HdfKeventIoServiceDispatch(
    struct HdfDeviceIoClient *client, int id, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct HdfKeventModule *keventModule;
    (void)reply;

    keventModule = (struct HdfKeventModule *)client->device->priv;
    if (keventModule == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    switch (id) {
        case KEVENT_COMPLETE_EVENT: {
            CompleteKevent(keventModule, data);
            break;
        }
        default:
            break;
    }

    return 0;
}

static int32_t HdfKeventDriverOpen(struct HdfDeviceIoClient *client)
{
    struct HdfKeventModule *keventModule = NULL;
    struct HdfKeventClient *kClient = NULL;

    if (client == NULL || client->device == NULL || client->device->priv == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    keventModule = (struct HdfKeventModule *)client->device->priv;
    kClient = OsalMemCalloc(sizeof(struct HdfKeventClient));
    if (kClient == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    kClient->ioClient = client;
    client->priv = kClient;

    OsalMutexLock(&keventModule->clientMutex);
    DListInsertTail(&kClient->listNode, &keventModule->clientList);
    keventModule->clientCount++;
    OsalMutexUnlock(&keventModule->clientMutex);

    HDF_LOGI("%s:kevnet usecount=%d", __func__, keventModule->clientCount);
    return 0;
}

static void HdfKeventDriverClose(struct HdfDeviceIoClient *client)
{
    struct HdfKeventClient *kClient = NULL;
    struct HdfKeventModule *keventModule;

    keventModule = (struct HdfKeventModule *)client->device->priv;
    if (keventModule == NULL) {
        return;
    }

    kClient = (struct HdfKeventClient *)client->priv;
    if (kClient == NULL) {
        return;
    }

    OsalMutexLock(&keventModule->clientMutex);
    DListRemove(&kClient->listNode);
    keventModule->clientCount--;
    OsalMemFree(kClient);
    client->priv = NULL;
    OsalMutexUnlock(&keventModule->clientMutex);

    HDF_LOGI("%s:kevnet usecount=%d", __func__, keventModule->clientCount);
}

static void HdfKeventDriverRelease(struct HdfDeviceObject *object)
{
    struct HdfKeventModule *keventModule = (struct HdfKeventModule *)object->priv;
    if (keventModule == NULL) {
        return;
    }

    unregister_pm_notifier(&keventModule->keventPmNotifier);
    fb_unregister_client(&keventModule->keventPmNotifier);
    OsalMutexDestroy(&keventModule->mutex);
    OsalMutexDestroy(&keventModule->clientMutex);
    OsalMemFree(keventModule);
    object->priv = NULL;
    return;
}

static int32_t HdfKeventDriverBind(struct HdfDeviceObject *dev)
{
    static struct IDeviceIoService keventService = {
        .Open = HdfKeventDriverOpen,
        .Dispatch = HdfKeventIoServiceDispatch,
        .Release = HdfKeventDriverClose,
    };
    struct HdfKeventModule *keventModule = NULL;
    if (dev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    keventModule = (struct HdfKeventModule *)OsalMemCalloc(sizeof(struct HdfKeventModule));
    if (keventModule == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    if (OsalMutexInit(&keventModule->mutex) != HDF_SUCCESS) {
        OsalMemFree(keventModule);
        return HDF_FAILURE;
    }

    if (OsalMutexInit(&keventModule->clientMutex) != HDF_SUCCESS) {
        OsalMutexDestroy(&keventModule->mutex);
        OsalMemFree(keventModule);
        return HDF_FAILURE;
    }
    DListHeadInit(&keventModule->waitList);
    DListHeadInit(&keventModule->clientList);
    keventModule->devObj = dev;
    dev->priv = keventModule;
    dev->service = &keventService;

    return HDF_SUCCESS;
}

static int32_t HdfKeventDriverInit(struct HdfDeviceObject *dev)
{
    int32_t ret;
    struct HdfKeventModule *keventModule = (struct HdfKeventModule *)dev->priv;

    keventModule->keventPmNotifier.notifier_call = KeventPmNotifierFn;
    ret = register_pm_notifier(&keventModule->keventPmNotifier);
    if (ret != 0) {
        HDF_LOGE("%s:failed to register pm notifier %d", __func__, ret);
    } else {
        HDF_LOGI("%s:register pm notifier success", __func__);
    }

    keventModule->fbNotifier.notifier_call = KeventFbNotifierFn;
    ret = fb_register_client(&keventModule->fbNotifier);
    if (ret != 0) {
        HDF_LOGE("%s:failed to register fb notifier %d", __func__, ret);
        unregister_pm_notifier(&keventModule->keventPmNotifier);
    } else {
        HDF_LOGI("%s:register fb notifier success", __func__);
    }

    g_keventModule = keventModule;
    return ret;
}

static struct HdfDriverEntry g_hdfKeventDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_KEVENT",
    .Bind = HdfKeventDriverBind,
    .Init = HdfKeventDriverInit,
    .Release = HdfKeventDriverRelease,
};

HDF_INIT(g_hdfKeventDriverEntry);