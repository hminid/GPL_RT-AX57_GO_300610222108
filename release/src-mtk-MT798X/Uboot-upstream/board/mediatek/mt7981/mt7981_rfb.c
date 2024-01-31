// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 MediaTek Inc.
 * Author: Sam Shih <sam.shih@mediatek.com>
 */

#if defined(CONFIG_ASUS_PRODUCT)
#include <gpio.h>
#include <common.h>
#endif

#if defined(CONFIG_ASUS_PRODUCT)
const char *model = CONFIG_MODEL;
const char *blver = CFG_BLVER;
const char *bl_stage = "";
#endif

int board_init(void)
{
	return 0;
}
