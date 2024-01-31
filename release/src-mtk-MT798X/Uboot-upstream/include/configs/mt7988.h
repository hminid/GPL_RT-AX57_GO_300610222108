/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Configuration for MediaTek MT7988 SoC
 *
 * Copyright (C) 2021 MediaTek Inc.
 * Author: Sam Shih <sam.shih@mediatek.com>
 */

#ifndef __MT7988_H
#define __MT7988_H

#include <linux/sizes.h>

/* Size of malloc() pool */
#define CONFIG_SYS_NONCACHED_MEMORY	SZ_1M

#define CONFIG_SYS_MMC_ENV_DEV		0

/* Uboot definition */
#define CONFIG_SYS_UBOOT_BASE		CONFIG_SYS_TEXT_BASE

/* SPL -> Uboot */
#define CONFIG_SYS_UBOOT_START		CONFIG_SYS_TEXT_BASE

/* DRAM */
#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_VERY_BIG_RAM
#define CONFIG_MAX_MEM_MAPPED		0xC0000000

/* Ethernet */
#define CONFIG_IPADDR			192.168.1.1
#define CONFIG_SERVERIP			192.168.1.2
#define CONFIG_NETMASK			255.255.255.0

#endif
