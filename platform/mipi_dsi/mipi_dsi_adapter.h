/*
 * mipi_dsi_adapter.h
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

#ifndef MIPI_DSI_ADAPTER_H
#define MIPI_DSI_ADAPTER_H

#include <linux/fs.h>

int MipiDsiRegisterDevice(const char *name, uint32_t id, unsigned short mode, struct file_operations *ops);
void MipiDsiUnregisterDevice(uint32_t id);
int MipiDsiProcRegister(const char *name, uint32_t id, unsigned short mode, const struct file_operations *ops);
void MipiDsiProcUnregister(const char *name, uint32_t id);
#endif // MIPI_DSI_ADAPTER_H
