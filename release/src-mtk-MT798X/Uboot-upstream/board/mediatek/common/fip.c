// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 MediaTek Inc. All Rights Reserved.
 *
 * Author: Sam Shih <sam.shih@mediatek.com>
 */

#include <string.h>
#include <stdint.h>
#include "firmware_image_package.h"
#include "fip.h"

/* The images used depends on the platform. */
static const struct toc_entry toc_entries[MAX_TOE_ENTRY] = {
	{ .desc = "SCP Firmware Updater Configuration FWU SCP_BL2U",
	  .uuid = UUID_TRUSTED_UPDATE_FIRMWARE_SCP_BL2U,
	  .name = "scp-fwu-cfg" },
	{ .desc = "AP Firmware Updater Configuration BL2U",
	  .uuid = UUID_TRUSTED_UPDATE_FIRMWARE_BL2U,
	  .name = "ap-fwu-cfg" },
	{ .desc = "Firmware Updater NS_BL2U",
	  .uuid = UUID_TRUSTED_UPDATE_FIRMWARE_NS_BL2U,
	  .name = "fwu" },
	{ .desc = "Non-Trusted Firmware Updater certificate",
	  .uuid = UUID_TRUSTED_FWU_CERT,
	  .name = "fwu-cert" },
	{ .desc = "Trusted Boot Firmware BL2",
	  .uuid = UUID_TRUSTED_BOOT_FIRMWARE_BL2,
	  .name = "tb-fw" },
	{ .desc = "SCP Firmware SCP_BL2",
	  .uuid = UUID_SCP_FIRMWARE_SCP_BL2,
	  .name = "scp-fw" },
	{ .desc = "EL3 Runtime Firmware BL31",
	  .uuid = UUID_EL3_RUNTIME_FIRMWARE_BL31,
	  .name = "soc-fw" },
	{ .desc = "Secure Payload BL32 (Trusted OS)",
	  .uuid = UUID_SECURE_PAYLOAD_BL32,
	  .name = "tos-fw" },
	{ .desc = "Secure Payload BL32 Extra1 (Trusted OS Extra1)",
	  .uuid = UUID_SECURE_PAYLOAD_BL32_EXTRA1,
	  .name = "tos-fw-extra1" },
	{ .desc = "Secure Payload BL32 Extra2 (Trusted OS Extra2)",
	  .uuid = UUID_SECURE_PAYLOAD_BL32_EXTRA2,
	  .name = "tos-fw-extra2" },
	{ .desc = "Non-Trusted Firmware BL33",
	  .uuid = UUID_NON_TRUSTED_FIRMWARE_BL33,
	  .name = "nt-fw" },
	{ .desc = "Realm Monitor Management Firmware",
	  .uuid = UUID_REALM_MONITOR_MGMT_FIRMWARE,
	  .name = "rmm-fw" },

	/* Dynamic Configs */
	{ .desc = "FW_CONFIG",
	  .uuid = UUID_FW_CONFIG,
	  .name = "fw-config" },
	{ .desc = "HW_CONFIG",
	  .uuid = UUID_HW_CONFIG,
	  .name = "hw-config" },
	{ .desc = "TB_FW_CONFIG",
	  .uuid = UUID_TB_FW_CONFIG,
	  .name = "tb-fw-config" },
	{ .desc = "SOC_FW_CONFIG",
	  .uuid = UUID_SOC_FW_CONFIG,
	  .name = "soc-fw-config" },
	{ .desc = "TOS_FW_CONFIG",
	  .uuid = UUID_TOS_FW_CONFIG,
	  .name = "tos-fw-config" },
	{ .desc = "NT_FW_CONFIG",
	  .uuid = UUID_NT_FW_CONFIG,
	  .name = "nt-fw-config" },

	/* Key Certificates */
	{ .desc = "Root Of Trust key certificate",
	  .uuid = UUID_ROT_KEY_CERT,
	  .name = "rot-cert" },
	{ .desc = "Trusted key certificate",
	  .uuid = UUID_TRUSTED_KEY_CERT,
	  .name = "trusted-key-cert" },
	{ .desc = "SCP Firmware key certificate",
	  .uuid = UUID_SCP_FW_KEY_CERT,
	  .name = "scp-fw-key-cert" },
	{ .desc = "SoC Firmware key certificate",
	  .uuid = UUID_SOC_FW_KEY_CERT,
	  .name = "soc-fw-key-cert" },
	{ .desc = "Trusted OS Firmware key certificate",
	  .uuid = UUID_TRUSTED_OS_FW_KEY_CERT,
	  .name = "tos-fw-key-cert" },
	{ .desc = "Non-Trusted Firmware key certificate",
	  .uuid = UUID_NON_TRUSTED_FW_KEY_CERT,
	  .name = "nt-fw-key-cert" },

	/* Content certificates */
	{ .desc = "Trusted Boot Firmware BL2 certificate",
	  .uuid = UUID_TRUSTED_BOOT_FW_CERT,
	  .name = "tb-fw-cert" },
	{ .desc = "SCP Firmware content certificate",
	  .uuid = UUID_SCP_FW_CONTENT_CERT,
	  .name = "scp-fw-cert" },
	{ .desc = "SoC Firmware content certificate",
	  .uuid = UUID_SOC_FW_CONTENT_CERT,
	  .name = "soc-fw-cert" },
	{ .desc = "Trusted OS Firmware content certificate",
	  .uuid = UUID_TRUSTED_OS_FW_CONTENT_CERT,
	  .name = "tos-fw-cert" },
	{ .desc = "Non-Trusted Firmware content certificate",
	  .uuid = UUID_NON_TRUSTED_FW_CONTENT_CERT,
	  .name = "nt-fw-cert" },
	{ .desc = "SiP owned Secure Partition content certificate",
	  .uuid = UUID_SIP_SECURE_PARTITION_CONTENT_CERT,
	  .name = "sip-sp-cert" },
	{ .desc = "Platform owned Secure Partition content certificate",
	  .uuid = UUID_PLAT_SECURE_PARTITION_CONTENT_CERT,
	  .name = "plat-sp-cert" },
	{
		.desc = NULL,
		.uuid = UUID_NULL,
		.name = NULL,
	}
};

/*****************************************************************************
 * FIP PARSE
 *****************************************************************************/
static struct image_desc image_desc_list[MAX_TOE_ENTRY];
static struct image image_list[MAX_IMAGE_NUM];
static size_t nr_image_descs;
static size_t nr_image;

void fill_image_descs(void)
{
	struct image_desc *root = image_desc_list;
	const struct toc_entry *toc_entry;
	struct image_desc *desc;

	nr_image_descs = 0;

	for (toc_entry = toc_entries; toc_entry->name; toc_entry++) {
		desc = &(*(root + nr_image_descs));

		/* create a nex image desc */
		memcpy(&desc->uuid, &toc_entry->uuid, sizeof(union fip_uuid));
		desc->desc = toc_entry->desc;
		desc->name = toc_entry->name;
		desc->next = &(*(root + nr_image_descs + 1));
		nr_image_descs++;
	}
}

void free_image_descs(void)
{
	int i;

	nr_image_descs = 0;
	for (i = 0; i < MAX_TOE_ENTRY; i++)
		memset(&image_desc_list[i], 0x0, sizeof(struct image_desc));
}

static struct image_desc *
lookup_image_desc_from_uuid(const union fip_uuid *uuid)
{
	struct image_desc *root = image_desc_list;
	struct image_desc *desc;

	for (desc = root; desc; desc = desc->next) {
		if (memcmp(&desc->uuid, uuid, sizeof(union fip_uuid)) == 0)
			return desc;
	}

	return NULL;
}

static int parse_fip(const void *buf, ulong size,
		     struct fip_toc_header *toc_header_out,
		     struct image_desc *root_out)
{
	struct image_desc *root = image_desc_list;
	static const union fip_uuid uuid_null;
	struct fip_toc_header *toc_header;
	struct fip_toc_entry *toc_entry;
	const u8 *bufend = buf + size;
	struct image_desc *desc;
	struct image *image;
	int terminated = 0;

	nr_image = 0;
	toc_header = (struct fip_toc_header *)buf;
	toc_entry = (struct fip_toc_entry *)(toc_header + 1);

	if (toc_header->name != TOC_HEADER_NAME) {
		debug("%s: input data is not a FIP file\n", __func__);
		goto err;
	}

	/* Return the ToC header if the caller wants it. */
	if (toc_header_out)
		*toc_header_out = *toc_header;

	/* Walk through each ToC entry in the file. */
	while ((u8 *)toc_entry + sizeof(*toc_entry) - 1 < bufend) {
		/* Found the ToC terminator, we are done. */
		if (!memcmp(&toc_entry->uuid, &uuid_null,
			    sizeof(union fip_uuid))) {
			terminated = 1;
			break;
		}

		/*
		 * Build a new image out of the ToC entry and add it to the
		 * table of images.
		 */
		image = &image_list[nr_image++];
		memset(image, 0x0, sizeof(struct image));

		if (nr_image >= MAX_IMAGE_NUM) {
			debug("%s: failed to allocate memory for image desc\n",
			      __func__);
			goto err;
		}

		image->toc_e = *toc_entry;

		/* Just point to the memory region of the sub image in the fip */
		image->buffer = buf + toc_entry->offset_address;
		desc = lookup_image_desc_from_uuid(&toc_entry->uuid);
		if (desc->image) {
			debug("%s: unexpect internal fail", __func__);
			goto err;
		}

		desc->image = image;
		toc_entry++;
	}

	if (terminated == 0) {
		debug("%s: FIP does not have a ToC terminator entry", __func__);
		goto err;
	}

	if (root_out)
		*root_out = *root;

	return 0;

err:
	return -1;
}

struct image_desc *find_image_desc_with_name(struct image_desc *root,
					     const char *name)
{
	struct image_desc *desc;
	struct image *image;

	for (desc = root; desc; desc = desc->next) {
		image = desc->image;
		if (!image)
			continue;
		debug("%s: offset=0x%llX, size=0x%llX, name=\"--%s\"\n",
		      desc->desc,
		      (unsigned long long)image->toc_e.offset_address,
		      (unsigned long long)image->toc_e.size,
		      desc->name);
		if (strcmp(desc->name, name) == 0)
			return desc;
	}

	return NULL;
}

static int find_fip_image_desc(const char *name, const void *fip, ulong fip_len,
			       struct image_desc *out_desc)
{
	struct fip_toc_header toc_header;
	struct image_desc *desc;
	struct image_desc root;
	int ret;

	fill_image_descs();
	ret = parse_fip(fip, fip_len, &toc_header, &root);
	if (ret < 0) {
		debug("%s: unable to parse fip image\n", __func__);
		ret = -1;
		goto err;
	}

	desc = find_image_desc_with_name(&root, name);
	if (!desc) {
		debug("%s: image '%s' not found in fip\n", __func__, name);
		ret = -1;
		goto err;
	}

	memcpy(out_desc, desc, sizeof(struct image_desc));

err:
	free_image_descs();
	return ret;
}

const void *locate_fip_image(const char *name, const void *fip, ulong fip_len,
			     ulong *len)
{
	struct image_desc desc;

	if (find_fip_image_desc(name, fip, fip_len, &desc))
		return NULL;
	*len = desc.image->toc_e.size;

	return desc.image->buffer;
}

int unpack_fip_image(const char *name, const void *fip, ulong fip_len,
		     void *data, ulong max_len, ulong *data_len_out)
{
	struct fip_toc_header toc_header;
	struct image_desc *desc;
	struct image_desc root;
	int ret;

	fill_image_descs();
	ret = parse_fip(fip, fip_len, &toc_header, &root);
	if (ret < 0) {
		debug("%s: unable to parse fip image\n", __func__);
		ret = -1;
		goto err;
	}

	desc = find_image_desc_with_name(&root, name);
	if (!desc) {
		debug("%s: image '%s' not found in fip\n", __func__, name);
		ret = -1;
		goto err;
	}

	if (desc->image->toc_e.size > max_len) {
		debug("%s: image '%s' size is too large\n", __func__, name);
		ret = -1;
		goto err;
	}

	memcpy(data, desc->image->buffer, desc->image->toc_e.size);
	*data_len_out = desc->image->toc_e.size;

err:
	free_image_descs();
	return ret;
}
