/*
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

#ifndef REGULATOR_ADAPTER_H
#define REGULATOR_ADAPTER_H
#include "hdf_base.h"

#define VOLTAGE_2500_UV 2500
#define CURRENT_2500_UA 2500

struct LinuxRegulatorInfo {
    const char *devName; // note:linux kernel constraints:len(devName) + len(supplyName) < REG_STR_SIZE(64)
    const char *supplyName; // note:linux kernel constraints:len(devName) + len(supplyName) < REG_STR_SIZE(64)
    struct device *dev;
    struct regulator *adapterReg;
};
int32_t LinuxRegulatorSetConsumerDev(struct device *dev);
#endif /* REGULATOR_ADAPTER_H */
