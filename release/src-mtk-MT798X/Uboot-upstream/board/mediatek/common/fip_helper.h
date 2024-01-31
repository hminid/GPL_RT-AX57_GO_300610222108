/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2022 MediaTek Inc. All Rights Reserved.
 *
 * Author: Sam Shih <sam.shih@mediatek.com>
 */

#ifndef _FIP_HELPER_H_
#define _FIP_HELPER_H_

#include "upgrade_helper.h"

/* 16MB fip max size */
#define MAX_FIP_SIZE	0x1000000

/* 2MB u-boot max size */
#define MAX_UBOOT_SIZE	0x200000

typedef bool (*unpack_handler_t)(ulong addr, ulong fip_size, ulong data_out,
				 ulong max_size, ulong *len);

bool fip_check_uboot_data(const void *addr, ulong fip_size);
bool fip_unpack_uboot(ulong addr, ulong fip_size, ulong data_out,
		      ulong max_size, ulong *len);

#endif /* _FIP_HELPER_ */
