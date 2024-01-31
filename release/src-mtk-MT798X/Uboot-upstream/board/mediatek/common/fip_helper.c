// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 MediaTek Inc. All Rights Reserved.
 *
 * Author: Sam Shih <sam.shih@mediatek.com>
 */

#include <asm/global_data.h>
#include <common.h>
#include <fdt_support.h>
#include <image.h>
#include <linux/libfdt.h>
#include "fip.h"
#include "fip_helper.h"
#include "board_info.h"

DECLARE_GLOBAL_DATA_PTR;

static const void *locate_fdt_in_uboot(const void *uboot, ulong search_size)
{
	struct image_header *header;
	const u8 *buf = uboot;
	ulong fdt_size;
	ulong offset;

	if (search_size < sizeof(struct image_header))
		return NULL;

	/* use backward search to find fdt magic header and check fdt size */
	offset = search_size - sizeof(struct image_header);
	while (offset) {
		header = (struct image_header *)(buf + offset);
		if (image_get_magic(header) == FDT_MAGIC) {
			fdt_size = fdt_totalsize(buf + offset);
			if (fdt_size <= (search_size - offset))
				return (void *)(buf + offset);
		}
		offset--;
	}

	return NULL;
}

bool fip_check_uboot_data(const void *fip, ulong fip_size)
{
	struct compat_list t_compat, c_compat;
	const void *uboot, *fdt_blob;
	unsigned int count;
	ulong uboot_len;
	int i;

	if (fdt_read_compat_list(gd->fdt_blob, 0, "compatible", &c_compat)) {
		log_err("Error: Compatible not found in current u-boot\n");
		return false;
	}

	uboot = locate_fip_image("nt-fw", fip, fip_size, &uboot_len);
	if (!uboot) {
		log_err("Error: Not a FIP image\n");
		return false;
	}

	fdt_blob = locate_fdt_in_uboot(uboot, uboot_len);
	if (!fdt_blob) {
		log_err("Error: FDT not found in the FIP u-boot image\n");
		return false;
	}

	if (fdt_read_compat_list(fdt_blob, 0, "compatible", &t_compat)) {
		log_err("Error: Compatible not found in current u-boot\n");
		return false;
	}

	if (c_compat.count > t_compat.count)
		count = t_compat.count;
	else
		count = c_compat.count;

	/* check the longer compat list is the superset of the shorter one */
	for (i = 0; i < count; i++)
		if (strcmp(c_compat.compats[i], t_compat.compats[i]))
			break;
	if (i == count)
		return true;

	log_err("Error: new u-boot is not compatible with current u-boot\n");
	log_err("       current compatible strings: ");
	print_compat_list(&c_compat);
	log_err("       new u-boot compatible strings: ");
	print_compat_list(&t_compat);

	return false;
}

bool fip_unpack_uboot(ulong addr, ulong fip_size, ulong data_out,
		      ulong max_size, ulong *len)
{
	int ret;

	if (max_size > MAX_UBOOT_SIZE)
		max_size = MAX_UBOOT_SIZE;

	/* unpack u-boot.bin from fip.bin */
	ret = unpack_fip_image("nt-fw", (void *)addr, fip_size,
			       (void *)data_out, max_size, len);
	if (!ret)
		return true;

	return false;
}
