// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 MediaTek Inc. All Rights Reserved.
 *
 * Author: Sam Shih <sam.shih@mediatek.com>
 */

#include <common.h>
#include <command.h>
#include "fip_helper.h"
#include "colored_print.h"

enum fip_cmd {
	FIP_CHECK_UBOOT,
	FIP_UNPACK,
};

static int do_fip_check_uboot(int argc, char *const argv[])
{
	const char *str_fip_addr, *str_fip_size;
	ulong fip_size = MAX_FIP_SIZE;
	ulong addr, size;

	str_fip_addr = *argv;
	argc--;
	argv++;

	if (argc > 0) {
		str_fip_size = *argv;
		size = simple_strtoul(str_fip_size, NULL, 0);
		if (size)
			fip_size = size;
	}

	addr = simple_strtoul(str_fip_addr, NULL, 0);
	if (addr) {
		if (fip_check_uboot_data((void *)addr, size)) {
			cprintln(NORMAL, "*** FIP verification passed ***");
			return CMD_RET_SUCCESS;
		}
		cprintln(ERROR, "*** FIP verification failed ***");
	}

	return CMD_RET_FAILURE;
}

static int do_fip_unpack_image(int argc, char *const argv[])
{
	const char *str_image, *str_fip_addr, *str_out_addr;
	ulong addr, addr_out, max_size, len;
	unpack_handler_t unpack_func;

	str_image = *argv;
	argc--;
	argv++;

	switch (*str_image) {
	case 'u':
		unpack_func = fip_unpack_uboot;
		max_size = MAX_UBOOT_SIZE;
		break;
	default:
		return CMD_RET_USAGE;
	}

	str_fip_addr = *argv;
	argc--;
	argv++;

	str_out_addr = *argv;
	argc--;
	argv++;

	addr = simple_strtoul(str_fip_addr, NULL, 0);
	if (!addr)
		return CMD_RET_USAGE;
	addr_out = simple_strtoul(str_out_addr, NULL, 0);
	if (!addr_out)
		return CMD_RET_USAGE;

	if (unpack_func) {
		if (unpack_func(addr, MAX_FIP_SIZE, addr_out, max_size, &len)) {
			cprintln(NORMAL, "*** FIP unpack passed ***");
			return CMD_RET_SUCCESS;
		}
	}

	cprintln(ERROR, "*** FIP unpack failed ***");

	return CMD_RET_FAILURE;
}

int do_fip(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	const char *str_cmd;
	enum fip_cmd sub_cmd;
	int ret;

	if (argc < 2)
		return CMD_RET_USAGE;

	str_cmd = argv[1];
	argc -= 2;
	argv += 2;

	switch (*str_cmd) {
	case 'c':
		sub_cmd = FIP_CHECK_UBOOT;
		if (argc < 1)
			return CMD_RET_USAGE;
		break;
	case 'u':
		sub_cmd = FIP_UNPACK;
		if (argc < 3)
			return CMD_RET_USAGE;
		break;
	default:
		return CMD_RET_USAGE;
	}

	switch (sub_cmd) {
	case FIP_CHECK_UBOOT:
		ret = do_fip_check_uboot(argc, argv);
		break;
	case FIP_UNPACK:
		ret = do_fip_unpack_image(argc, argv);
		break;
	default:
		return CMD_RET_USAGE;
	}

	return ret;
}

U_BOOT_CMD(fip, CONFIG_SYS_MAXARGS, 0, do_fip, "FIP utility commands", "\n"
	"fip check <addr> [length] - check fip image is valid for this u-boot or not\n"
	"fip unpack uboot <addr> <addr_out> - unpack u-boot in fip to target address\n"
);
