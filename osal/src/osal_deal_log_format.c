/*
 * osal_deal_log_format.c
 *
 * osal driver
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

#include <linux/printk.h>
#include "securec.h"

#define NUMBER 2
static const char *g_property[NUMBER] = { "%{private}", "%{public}" };
static size_t g_property_len[NUMBER] = { 10, 9 };

/* remove "{private}" and "{public}" from fmt and copy the rest to dest */
bool deal_format(const char *fmt, char *dest, size_t size)
{
	const char *ptr = fmt;
	const char *ptr_cur = ptr;
	errno_t ret;
	size_t index = 0;
	size_t i;

	if (fmt == NULL || dest == NULL || size == 0 || strlen(fmt) >= (size - 1)) {
		printk("%s invalid para", __func__);
		return false;
	}

	while (ptr_cur != NULL && *ptr_cur != '\0') {
		if (*ptr_cur != '%') {
			ptr_cur++;
			continue;
		}
		for (i = 0; i < NUMBER; i++) {
			if (strncmp(ptr_cur, g_property[i], g_property_len[i]) != 0)
				continue;

			/* add 1 is for to copy char '%' */
			ret = strncpy_s(&dest[index], size - index, ptr, ptr_cur - ptr + 1);
			if (ret != EOK) {
				printk("%s strncpy_s error %d", __func__, ret);
				return false;
			}
			index += (ptr_cur - ptr + 1);
			ptr = ptr_cur + g_property_len[i];
			ptr_cur = ptr;
			break;
		}
		if (i == NUMBER)
			ptr_cur++;
	}
	ret = strcat_s(&dest[index], size - index, ptr);
	if (ret != EOK) {
		printk("%s strcat_s error %d", __func__, ret);
		return false;
	}
	return true;
}
EXPORT_SYMBOL(deal_format);
