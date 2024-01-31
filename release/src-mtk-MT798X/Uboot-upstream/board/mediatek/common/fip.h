/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2022 MediaTek Inc. All Rights Reserved.
 *
 * Author: Sam Shih <sam.shih@mediatek.com>
 */

#ifndef FIP_H
#define FIP_H

#include <uuid.h>
#include <stdint.h>

#define MAX_TOE_ENTRY	50
#define MAX_IMAGE_NUM	50

/* Length of a node address (an IEEE 802 address). */
#define _UUID_NODE_LEN	6

/* Length of UUID string including dashes. */
#define _UUID_STR_LEN	36

struct atf_uuid {
	u8 time_low[4];
	u8 time_mid[2];
	u8 time_hi_and_version[2];
	u8 clock_seq_hi_and_reserved;
	u8 clock_seq_low;
	u8 node[_UUID_NODE_LEN];
} __packed;

union fip_uuid {
	struct atf_uuid atf_uuid;
	struct uuid uboot_uuid;
};

struct fip_toc_header {
	u32 name;
	u32 serial_number;
	u64 flags;
};

struct fip_toc_entry {
	union fip_uuid uuid;
	u64 offset_address;
	u64 size;
	u64 flags;
};

struct image {
	struct fip_toc_entry toc_e;
	const void *buffer;
};

struct image_desc {
	union fip_uuid uuid;
	const char *desc;
	const char *name;
	struct image *image;
	struct image_desc *next;
};

/*****************************************************************************
 * TOC ENTRIES
 *****************************************************************************/
struct toc_entry {
	const char *desc;
	union fip_uuid uuid;
	const char *name;
};

const void *locate_fip_image(const char *name, const void *fip, ulong fip_len,
			     ulong *len);
int unpack_fip_image(const char *name, const void *fip, ulong fip_len,
		     void *data, ulong max_len, ulong *data_len_out);

#endif /* FIP_H */
