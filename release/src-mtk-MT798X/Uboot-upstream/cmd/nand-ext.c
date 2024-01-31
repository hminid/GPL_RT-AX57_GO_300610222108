// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 */

#include <command.h>
#include <stdbool.h>
#include <malloc.h>
#include <mtd.h>
#include <dm/devres.h>
#include <linux/types.h>
#include <linux/mtd/mtd.h>

#if defined(CONFIG_ASUS_PRODUCT)
#include <init.h>
//#define TEST_CMD
#ifdef TEST_CMD
#include <flash_wrapper.h>
#include <net.h>
#endif
#endif

static struct mtd_info *curr_dev;

static void mtd_show_parts(struct mtd_info *mtd, int level)
{
	struct mtd_info *part;
	int i;

	list_for_each_entry(part, &mtd->partitions, node) {
		for (i = 0; i < level; i++)
			printf("\t");
		printf("  - 0x%012llx-0x%012llx : \"%s\"\n",
		       part->offset, part->offset + part->size, part->name);

		mtd_show_parts(part, level + 1);
	}
}

static void mtd_show_device(struct mtd_info *mtd)
{
	/* Device */
	printf("* %s\n", mtd->name);
#if defined(CONFIG_DM)
	if (mtd->dev) {
		printf("  - device: %s\n", mtd->dev->name);
		printf("  - parent: %s\n", mtd->dev->parent->name);
		printf("  - driver: %s\n", mtd->dev->driver->name);
	}
#endif

	/* MTD device information */
	printf("  - type: ");
	switch (mtd->type) {
	case MTD_NANDFLASH:
		printf("NAND flash\n");
		break;
	case MTD_MLCNANDFLASH:
		printf("MLC NAND flash\n");
		break;
	case MTD_ABSENT:
	default:
		printf("Not supported\n");
		break;
	}

	printf("  - block size:        0x%x bytes\n", mtd->erasesize);
	printf("  - page size:         0x%x bytes\n", mtd->writesize);
	printf("  - OOB size:          %u bytes\n", mtd->oobsize);
	printf("  - OOB available:     %u bytes\n", mtd->oobavail);

	if (mtd->ecc_strength) {
		printf("  - ECC strength:      %u bits\n", mtd->ecc_strength);
		printf("  - ECC step size:     %u bytes\n", mtd->ecc_step_size);
		printf("  - bitflip threshold: %u bits\n",
		       mtd->bitflip_threshold);
	}

	printf("  - 0x%012llx-0x%012llx : \"%s\"\n",
	       mtd->offset, mtd->offset + mtd->size, mtd->name);

	/* MTD partitions, if any */
	mtd_show_parts(mtd, 1);
}

static int do_nand_list(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{
	struct mtd_info *mtd;
	int dev_nb = 0;

	/* Ensure all devices (and their partitions) are probed */
	mtd_probe_devices();

	printf("List of NAND devices:\n");
	mtd_for_each_device(mtd) {
		if (mtd->type != MTD_NANDFLASH && mtd->type != MTD_MLCNANDFLASH)
			continue;

		if (!mtd_is_partition(mtd))
			mtd_show_device(mtd);

		dev_nb++;
	}

	if (!dev_nb)
		printf("No NAND MTD device found\n");

	return CMD_RET_SUCCESS;
}

static struct mtd_info *nand_get_curr_dev(void)
{
	struct mtd_info *mtd, *first_dev = NULL;
	int err, dev_nb = 0;

	if (curr_dev) {
		mtd = get_mtd_device(curr_dev, -1);
		if (!IS_ERR_OR_NULL(mtd)) {
			__put_mtd_device(mtd);
			return mtd;
		}

		curr_dev = NULL;
	}

	/* Ensure all devices (and their partitions) are probed */
	mtd_probe_devices();

	mtd_for_each_device(mtd) {
		if (mtd->type != MTD_NANDFLASH && mtd->type != MTD_MLCNANDFLASH)
			continue;

		if (!mtd_is_partition(mtd)) {
			if (!first_dev)
				first_dev = mtd;
			dev_nb++;
		}
	}

	if (!dev_nb) {
		printf("No NAND MTD device found\n");
		return NULL;
	}

	if (dev_nb > 1) {
		printf("No active NAND MTD device specified\n");
		return NULL;
	}

	err = __get_mtd_device(first_dev);
	if (err) {
		printf("Failed to get MTD device '%s': err %d\n",
		       first_dev->name, err);
		return NULL;
	}

	curr_dev = first_dev;

	printf("'%s' is now active device\n", first_dev->name);

	return curr_dev;
}

static struct mtd_info *nand_get_part(struct mtd_info *master,
				       const char *name)
{
	struct mtd_info *slave;

	list_for_each_entry(slave, &master->partitions, node) {
		if (!strcmp(slave->name, name))
			return slave;
	}

	return NULL;
}

static int do_nand_info(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{
	struct mtd_info *mtd = nand_get_curr_dev();

	if (!mtd)
		return CMD_RET_FAILURE;

	mtd_show_device(mtd);

	return 0;
}

#if defined(CONFIG_ASUS_PRODUCT)
static int do_reset_default(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{
	reset_to_default();
	return 0;
}

#include <ubi_uboot.h>
void clear_spinand(void);
extern struct udevice *gl_flash_udev;
static int do_reprobe(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{
	char *ubi_detach[] = { "ubi", "detach"};
	do_ubi(NULL, 0, ARRAY_SIZE(ubi_detach), ubi_detach);
	clear_spinand();
	curr_dev=NULL;
	if (gl_flash_udev && gl_flash_udev->driver)
		gl_flash_udev->driver->probe(gl_flash_udev);
	return 0;
}
#ifdef TEST_CMD
static int do_d1(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{
	/* destroy linux 1MB size */
	memset((uchar *)CONFIG_SYS_LOAD_ADDR, 0xff, 0x100000);
	ra_flash_erase_write((uchar *)CONFIG_SYS_LOAD_ADDR, CFG_KERN_ADDR, 0x100000, 0);
	return 0;
}

static int do_d2(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{
	/* destroy linux2 1MB size */
	memset((uchar *)CONFIG_SYS_LOAD_ADDR, 0xff, 0x100000);
	ra_flash_erase_write((uchar *)CONFIG_SYS_LOAD_ADDR, CFG_KERN2_ADDR, 0x100000, 0);
	return 0;
}

static int do_rescue(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{
	printf(" \nHello!! Enter Recuse Mode: (Check error)\n\n");
	if (net_loop(TFTPD) < 0)
		return -1;
	return 0;
}
#endif
#endif
static int do_nand_select(struct cmd_tbl *cmdtp, int flag, int argc,
			   char *const argv[])
{
	struct mtd_info *mtd, *old;

	if (argc < 2) {
		printf("MTD device name must be specified\n");
		return CMD_RET_USAGE;
	}

	mtd = get_mtd_device_nm(argv[1]);
	if (!mtd) {
		printf("MTD device '%s' not found\n", argv[1]);
		return CMD_RET_FAILURE;
	}

	if (mtd_is_partition(mtd)) {
		printf("Error: '%s' is a MTD partition\n", argv[1]);
		__put_mtd_device(mtd);
		return CMD_RET_FAILURE;
	}

	if (mtd->type != MTD_NANDFLASH && mtd->type != MTD_MLCNANDFLASH) {
		printf("Error: '%s' is not a NAND device\n", argv[1]);
		__put_mtd_device(mtd);
		return CMD_RET_FAILURE;
	}

	if (mtd == curr_dev) {
		__put_mtd_device(mtd);
		return CMD_RET_SUCCESS;
	}

	if (curr_dev) {
		old = get_mtd_device(curr_dev, -1);
		if (!IS_ERR_OR_NULL(old)) {
			__put_mtd_device(old);
			__put_mtd_device(curr_dev);
		}

		curr_dev = NULL;
	}

	curr_dev = mtd;

	printf("'%s' is now active device\n", curr_dev->name);

	return CMD_RET_SUCCESS;
}

static void dump_buf(const u8 *data, size_t size, u64 addr)
{
	const u8 *p = data;
	u32 i, chklen;

	while (size) {
		chklen = 16;
		if (chklen > size)
			chklen = (u32)size;

		printf("%08llx: ", addr);

		for (i = 0; i < chklen; i++) {
			if (i && (i % 4 == 0))
				printf(" ");

			printf("%02x ", p[i]);
		}

		for (i = chklen; i < 16; i++) {
			if (i && (i % 4 == 0))
				printf(" ");

			printf("   ");
		}
		printf(" ");

		for (i = 0; i < chklen; i++) {
			if (p[i] < 32 || p[i] >= 0x7f)
				printf(".");
			else
				printf("%c", p[i]);
		}
		printf("\n");

		p += chklen;
		size -= chklen;
		addr += chklen;
	}
}

static int do_nand_dump(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{
	struct mtd_info *mtd = nand_get_curr_dev();
	struct mtd_oob_ops io_op = {};
	bool raw = false;
	int ret;
	u64 off;
	u8 *buf;

	if (!mtd)
		return CMD_RET_FAILURE;

	if (strstr(argv[0], ".raw"))
		raw = true;

	if (argc < 2) {
		printf("Dump offset must be specified\n");
		return CMD_RET_USAGE;
	}

	off = simple_strtoull(argv[1], NULL, 0);
	if (off >= mtd->size) {
		printf("Offset 0x%llx is larger than flash size\n", off);
		return CMD_RET_FAILURE;
	}

	off &= ~(u64)mtd->writesize_mask;

	buf = malloc(mtd->writesize + mtd->oobsize);
	if (!buf) {
		printf("Failed to allocate buffer\n");
		return CMD_RET_FAILURE;
	}

	io_op.mode = raw ? MTD_OPS_RAW : MTD_OPS_PLACE_OOB;
	io_op.len = mtd->writesize;
	io_op.datbuf = buf;
	io_op.ooblen = mtd->oobsize;
	io_op.oobbuf = buf + mtd->writesize;

	ret = mtd_read_oob(mtd, off, &io_op);
	if (ret < 0 && ret != -EUCLEAN && ret != -EBADMSG) {
		printf("Failed to read page at 0x%llx, err %d\n", off, ret);
		free(buf);
		return CMD_RET_FAILURE;
	}

	printf("Dump of %spage at 0x%llx (page %llx):\n", raw ? "raw " : "", off, off >> mtd->writesize_shift);
	dump_buf(buf, mtd->writesize, off);

	printf("\n");
	printf("OOB:\n");
	dump_buf(buf + mtd->writesize, mtd->oobsize, 0);

	free(buf);

	return CMD_RET_SUCCESS;
}

static int do_nand_bad(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	struct mtd_info *mtd = nand_get_curr_dev();
	u64 off = 0;

	if (!mtd)
		return CMD_RET_FAILURE;

	while (off < mtd->size) {
		if (mtd_block_isbad(mtd, off))
			printf("\t%08llx\n", off);

		off += mtd->erasesize;
	}

	return 0;
}

static int do_nand_markbad(struct cmd_tbl *cmdtp, int flag, int argc,
			    char *const argv[])
{
	struct mtd_info *mtd = nand_get_curr_dev();
	u64 off;
	int ret;

	if (!mtd)
		return CMD_RET_FAILURE;

	if (argc < 2) {
		printf("Missing address within a block to be marked bad\n");
		return CMD_RET_USAGE;
	}

	off = simple_strtoull(argv[1], NULL, 0);
	if (off >= mtd->size) {
		printf("Offset 0x%llx is larger than flash size\n", off);
		return CMD_RET_FAILURE;
	}

	off &= ~(u64)mtd->erasesize_mask;

	ret = mtd_block_markbad(mtd, off);

	if (!ret)
		printf("Block at 0x%08llx has been marked bad\n", off);
	else
		printf("Failed to mark bad block at 0x%08llx\n", off);

	return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

static int do_nand_bitflip(struct cmd_tbl *cmdtp, int flag, int argc,
			    char *const argv[])
{
	struct mtd_info *mtd = nand_get_curr_dev();
	struct mtd_oob_ops io_op = {};
	u32 col, bit;
	bool res;
	u64 off;
	u8 *buf;
	int ret;

	if (!mtd)
		return CMD_RET_FAILURE;

	if (argc < 2) {
		printf("Missing address to generate bitflip\n");
		return CMD_RET_USAGE;
	}

	off = simple_strtoull(argv[1], NULL, 0);
	if (off >= mtd->size) {
		printf("Offset 0x%llx is larger than flash size\n", off);
		return CMD_RET_FAILURE;
	}

	if (argc < 3) {
		printf("Missing column address\n");
		return CMD_RET_USAGE;
	}

	col = simple_strtoul(argv[2], NULL, 0);
	if (col >= mtd->writesize + mtd->oobsize) {
		printf("Column address must be less than %u\n",
		       mtd->writesize + mtd->oobsize);
		return CMD_RET_FAILURE;
	}

	if (argc < 4) {
		printf("Missing bit position\n");
		return CMD_RET_USAGE;
	}

	bit = simple_strtoul(argv[3], NULL, 0);
	if (bit > 7) {
		printf("Bit position must be less than 8\n");
		return CMD_RET_FAILURE;
	}

	off &= ~(u64)mtd->writesize_mask;

	buf = malloc(mtd->writesize + mtd->oobsize);
	if (!buf) {
		printf("Failed to allocate buffer\n");
		return CMD_RET_FAILURE;
	}

	io_op.mode = MTD_OPS_RAW;
	io_op.len = mtd->writesize;
	io_op.datbuf = buf;
	io_op.ooblen = mtd->oobsize;
	io_op.oobbuf = buf + mtd->writesize;

	ret = mtd_read_oob(mtd, off, &io_op);
	if (ret < 0) {
		printf("Failed to read page at 0x%llx, err %d\n", off, ret);
		free(buf);
		return CMD_RET_FAILURE;
	}

	if (!(buf[col] & (1 << bit))) {
		printf("Bit %u at byte %u is already zero\n", bit, col);
		free(buf);
		return CMD_RET_FAILURE;
	}

	buf[col] &= ~(1 << bit);

	memset(&io_op, 0, sizeof(io_op));
	io_op.mode = MTD_OPS_RAW;
	io_op.len = mtd->writesize;
	io_op.datbuf = buf;
	io_op.ooblen = mtd->oobsize;
	io_op.oobbuf = buf + mtd->writesize;

	ret = mtd_write_oob(mtd, off, &io_op);

	if (ret < 0) {
		printf("Failed to write page at 0x%llx, err %d\n", off, ret);
		return CMD_RET_FAILURE;
	}

	memset(&io_op, 0, sizeof(io_op));
	io_op.mode = MTD_OPS_RAW;
	io_op.len = mtd->writesize;
	io_op.datbuf = buf;
	io_op.ooblen = mtd->oobsize;
	io_op.oobbuf = buf + mtd->writesize;

	ret = mtd_read_oob(mtd, off, &io_op);
	if (ret < 0) {
		printf("Failed to read page at 0x%llx, err %d\n", off, ret);
		free(buf);
		return CMD_RET_FAILURE;
	}

	res = (buf[col] & (1 << bit)) == 0;
	free(buf);

	if (res) {
		printf("Bit %u at byte %u has been changed to 0\n", bit, col);
		return CMD_RET_SUCCESS;
	}

	printf("Failed to change bit %u at byte %u to 0\n", bit, col);
	return CMD_RET_FAILURE;
}

#if defined(DEBUG_ECC_CORRECTION)
static int raw_access(struct mtd_info *mtd, ulong addr, loff_t off, ulong count, int read)
{
	int ret;
	struct mtd_oob_ops io_op = {};

	if (!mtd || !addr || !count)
		return CMD_RET_FAILURE;

	off &= ~(u64)mtd->writesize_mask;
	for (; count > 0 && off < mtd->size;
	     --count, off += mtd->writesize, addr += mtd->writesize + mtd->oobsize)
	{
		io_op.mode = MTD_OPS_RAW;
		io_op.len = mtd->writesize;
		io_op.datbuf = (u8*) addr;
		io_op.ooblen = mtd->oobsize;
		io_op.oobbuf = (u8*) (addr + mtd->writesize);

		ret = mtd_read_oob(mtd, off, &io_op);
		if (ret < 0 && ret != -EUCLEAN && ret != -EBADMSG) {
			printf("Failed to read page at 0x%llx, err %d\n", off, ret);
			return CMD_RET_FAILURE;
		}

	}
	return count? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

static int do_nand_flipbits(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	/* nand flipbits <page_number> byte_addr:bit_addr[,bit_addr][,bit_addr...]
	 * [byte_addr:bit_addr[,bit_addr][,bit_addr...]
	 * [byte_addr:bit_addr[,bit_addr][,bit_addr...]
	 * [byte_addr:bit_addr[,bit_addr][,bit_addr...]
	 * Up to 4 bytes can be alerted.
	 */
	struct mtd_info *mtd = nand_get_curr_dev();
	const int pages_per_block = mtd->erasesize / mtd->writesize;
	struct mtd_oob_ops ops;
	int i, mod_cnt = 0, ret, cnt;
	ulong off;
	struct mod_s {
		unsigned int byte_addr;
		unsigned int bit_mask;
	} mod_ary[4], *mod = &mod_ary[0];
	unsigned int block, page, start_page, byte_addr, bit;
	char *q;
	unsigned char c, *p;
	unsigned char blk_buf[mtd->erasesize + pages_per_block * mtd->oobsize]  __attribute__ ((aligned(4)));
	struct erase_info ei;
	uint32_t erasesize_shift = ffs(mtd->erasesize) - 1;
	uint32_t writesize_shift = ffs(mtd->writesize) - 1;

	if (argc < 3)
		return 1;
	if (!mtd->_erase || !mtd->_write_oob) {
		printf("Invalid mtd->_erase %p or mtd->_write_oob %p\n", mtd->_erase, mtd->_write_oob);
		return 1;
	}

	page = simple_strtoul(argv[1], NULL, 16);
	if (page * mtd->writesize >= mtd->size) {
		printf("invalid page 0x%x\n", page);
		return 1;
	}
	start_page = page & ~(pages_per_block - 1);
	printf("erasesize_shift %d writesize_shift %d\n", erasesize_shift, writesize_shift);
	block = start_page >> (erasesize_shift - writesize_shift);
	printf("page 0x%x start_page 0x%x block 0x%x\n", page, start_page, block);

	/* parsing byte address, bit address */
	for (i = 2; i < argc; ++i) {
		if ((q = strchr(argv[i], ':')) == NULL) {
			printf("colon symbol not found.\n");
			return 1;
		}

		*q = '\0';
		byte_addr = simple_strtoul(argv[i], NULL, 16);
		if (byte_addr >= (2048 + 64)) {
			printf("invalid byte address 0x%x\n", byte_addr);
			return 1;
		}
		mod->byte_addr = byte_addr;
		mod->bit_mask = 0;

		q++;
		while (q && *q != '\0') {
			if (*q < '0' || *q > '9') {
				printf("invalid character. (%c %x)\n", *q, *q);
				return 1;
			}
			bit = simple_strtoul(q, NULL, 16);
			if (bit >= 8) {
				printf("invalid bit address %d\n", bit);
				return 1;
			}
			mod->bit_mask |= (1 << bit);
			q = strchr(q, ',');
			if (q)
				q++;
		}
		mod_cnt++;
		mod++;
	}

	if (!mod_cnt) {
		printf("byte address/bit address pair is not specified.\n");
		return 1;
	}

	/* read a block from block-aligned address with valid OOB information */
	for (i = 0, cnt = 0, p = &blk_buf[0], off = start_page << writesize_shift;
	     i < pages_per_block;
	     ++i, p += mtd->writesize + mtd->oobsize, off += mtd->writesize)
	{
		if ((ret = raw_access(mtd, (ulong) p, off, 1, 1)) < 0)
			printf("read page 0x%x fail. (ret %d)\n", start_page + i, ret);
		else
			cnt++;
	}

	if (cnt != pages_per_block)
		return 1;

	/* erase block */
	memset(&ei, 0, sizeof(ei));
	ei.mtd = mtd;
	ei.addr = block << erasesize_shift;
	ei.len = mtd->erasesize;
	if ((ret = mtd->_erase(mtd, &ei)) != 0) {
		printf("Erase addr %x len %x fail. (ret %d)\n",
			(unsigned int) ei.addr, (unsigned int)ei.len, ret);
		return CMD_RET_FAILURE;
	}

	/* flip bits */
	p = &blk_buf[0] + ((page - start_page) << writesize_shift) + ((page - start_page) * mtd->oobsize);
	for (i = 0, mod = &mod_ary[0]; i < mod_cnt; ++i, ++mod) {
		c = *(p + mod->byte_addr);
		*(p + mod->byte_addr) ^= mod->bit_mask;
		printf("flip page 0x%x byte 0x%x bitmask 0x%x: orig val %02x -> %02x\n",
			page, mod->byte_addr, mod->bit_mask, c, *(p + mod->byte_addr));
	}

	/* use raw write to write back page and oob information */
	for (i = 0, p = &blk_buf[0]; i < pages_per_block; ++i) {
		memset(&ops, 0, sizeof(ops));
		ops.datbuf = p;
		ops.len = mtd->writesize;
		ops.oobbuf = p + mtd->writesize;
		ops.ooblen = mtd->oobsize;
		ops.mode =  MTD_OPS_RAW;
		if ((ret = mtd->_write_oob(mtd, (start_page + i) << writesize_shift, &ops)) != 0)
			printf("write page 0x%x fail. (ret %d)\n", start_page + i, ret);

		p += mtd->writesize + mtd->oobsize;
	}
	return CMD_RET_SUCCESS;
}
#endif

#if defined(CONFIG_ASUS_PRODUCT)
int do_nand_erase(struct cmd_tbl *cmdtp, int flag, int argc,
			  char *const argv[])
#else
static int do_nand_erase(struct cmd_tbl *cmdtp, int flag, int argc,
			  char *const argv[])
#endif
{
	struct mtd_info *mtd = nand_get_curr_dev(), *part;
	bool spread = false, force = false;
	u64 off, size, end, limit;
	struct erase_info ei;
	char *ends;
	int ret;

	if (!mtd)
		return CMD_RET_FAILURE;

	if (strstr(argv[0], ".spread"))
		spread = true;

	if (strstr(argv[0], ".force"))
		force = true;

	if (spread && force) {
		printf("spread and force must not be set at the same time\n");
		return CMD_RET_FAILURE;
	}

	if (argc < 2) {
		printf("Erase start offset/partition must be specified\n");
		return CMD_RET_USAGE;
	}

	part = nand_get_part(mtd, argv[1]);
	if (part) {
		off = part->offset;

		if (argc < 3)
			size = part->size;
		else
			size = simple_strtoull(argv[2], NULL, 0);

		if (size > part->size) {
			printf("Erase end offset is larger than partition size\n");
			printf("Erase size reduced to 0x%llx\n", part->size);

			size = part->size;
		}

		limit = off + part->size;
	} else {
		off = simple_strtoull(argv[1], &ends, 0);

		if (ends == argv[1] || *ends) {
			printf("Partition '%s' not found\n", argv[1]);
			return CMD_RET_FAILURE;
		}

		if (off >= mtd->size) {
			printf("Offset 0x%llx is larger than flash size\n", off);
			return CMD_RET_FAILURE;
		}

		if (argc < 3) {
			printf("Erase size offset must be specified\n");
			return CMD_RET_USAGE;
		}

		size = simple_strtoull(argv[2], NULL, 0);

		if (off + size > mtd->size) {
			printf("Erase end offset is larger than flash size\n");

			size = mtd->size - off;
			printf("Erase size reduced to 0x%llx\n", size);
		}

		limit = mtd->size;
	}

	end = off + size;
	off &= ~(u64)mtd->erasesize_mask;
	end = (end + mtd->erasesize_mask) & (~(u64)mtd->erasesize_mask);
	size = end - off;

	printf("Erasing from 0x%llx to 0x%llx, size 0x%llx ...\n",
	       off, end - 1, end - off);

	while (size && off < limit) {
		if (mtd_block_isbad(mtd, off)) {
			printf("Bad block at 0x%llx", off);

			if (spread) {
				printf(" ... skipped\n");
				off += mtd->erasesize;
				continue;
			}

			if (!force) {
				printf(" ... aborted\n");
				return CMD_RET_FAILURE;
			}

			printf(" ... will be force erased\n");
		}

		memset(&ei, 0, sizeof(ei));

		ei.mtd = mtd;
		ei.addr = off;
		ei.len = mtd->erasesize;
		ei.scrub = force;

		ret = mtd_erase(mtd, &ei);
		if (ret) {
			printf("Erase failed at 0x%llx\n", off);
			return CMD_RET_FAILURE;
		}

		off += mtd->erasesize;
		size -= mtd->erasesize;
	}

	printf("Succeeded\n");

	return CMD_RET_SUCCESS;
}

static bool is_empty_page(const u8 *buf, size_t size)
{
	size_t i;

	for (i = 0; i < size; i++) {
		if (buf[i] != 0xff)
			return false;
	}

	return true;
}

static int do_nand_io_normal(int argc, char *const argv[])
{
	struct mtd_info *mtd = nand_get_curr_dev(), *part;
	bool spread = false, force = false, raw = false, writeff = false;
	bool read = false, checkbad = true;
	struct mtd_oob_ops io_op = {};
	size_t size, padding, chksz;
	uintptr_t addr;
	u64 off, offp;
	char *ends;
	u8 *buf;
	int ret;

	if (!mtd)
		return CMD_RET_FAILURE;

	if (!strncmp(argv[0], "read", 4))
		read = true;

	if (strstr(argv[0], ".spread"))
		spread = true;

	if (strstr(argv[0], ".force"))
		force = true;

	if (strstr(argv[0], ".raw"))
		raw = true;

	if (strstr(argv[0], ".ff"))
		writeff = true;

	if (spread && force) {
		printf("spread and force must not be set at the same time\n");
		return CMD_RET_FAILURE;
	}

	if (argc < 2) {
		printf("Data address must be specified\n");
		return CMD_RET_USAGE;
	}

	addr = simple_strtoul(argv[1], NULL, 0);

	if (argc < 3) {
		printf("Flash address/partition must be specified\n");
		return CMD_RET_USAGE;
	}

	part = nand_get_part(mtd, argv[2]);
	if (part) {
		if (argc < 4) {
			off = 0;
		} else {
			off = simple_strtoull(argv[3], NULL, 0);
			if (off + part->offset >= part->size) {
				printf("Offset is larger than partition size\n");
				return CMD_RET_FAILURE;
			}
		}

		if (argc < 5) {
			size = part->size - off;
		} else {
			size = simple_strtoul(argv[4], NULL, 0);
			if (off + size > part->size) {
				printf("Data size is too large\n");
				return CMD_RET_FAILURE;
			}
		}

		off += part->offset;
	} else {
		off = simple_strtoull(argv[2], &ends, 0);

		if (ends == argv[1] || *ends) {
			printf("Partition '%s' not found\n", argv[2]);
			return CMD_RET_FAILURE;
		}

		if (off >= mtd->size) {
			printf("Offset 0x%llx is larger than flash size\n", off);
			return CMD_RET_FAILURE;
		}

		if (argc < 4) {
			printf("Data size must be specified\n");
			return CMD_RET_USAGE;
		}

		size = simple_strtoul(argv[3], NULL, 0);
		if (off + size > mtd->size) {
			printf("Data size is too large\n");
			return CMD_RET_FAILURE;
		}
	}

	buf = malloc(mtd->writesize);
	if (!buf) {
		printf("Failed to allocate buffer\n");
		return CMD_RET_FAILURE;
	}

	printf("%s from 0x%llx to 0x%llx, size 0x%zx ...\n",
	       read ? "Reading" : "Writing", off, off + size - 1, size);

	while (size && off < mtd->size) {
		if (checkbad || !(off & mtd->erasesize_mask)) {
			offp = off & ~(u64)mtd->erasesize_mask;

			if (mtd_block_isbad(mtd, offp)) {
				printf("Bad block at 0x%llx", offp);

				if (spread) {
					printf(" ... skipped\n");
					off += mtd->erasesize;
					checkbad = true;
					continue;
				}

				if (!force) {
					printf(" ... aborted\n");
					goto err_out;
				}

				printf(" ... continue\n");
			}

			checkbad = false;
		}

		padding = off & mtd->writesize_mask;
		chksz = mtd->writesize - padding;
		chksz = min_t(size_t, chksz, size);

		offp = off & ~(u64)mtd->writesize_mask;

		memset(&io_op, 0, sizeof(io_op));
		io_op.mode = raw ? MTD_OPS_RAW : MTD_OPS_PLACE_OOB;
		io_op.len = mtd->writesize;

		if (chksz < mtd->writesize)
			io_op.datbuf = buf;
		else
			io_op.datbuf = (void *)addr;

		if (read) {
			ret = mtd_read_oob(mtd, offp, &io_op);
			if (ret && ret != -EUCLEAN && ret != -EBADMSG)
				goto io_err;

			if (chksz < mtd->writesize)
				memcpy((void *)addr, buf + padding, chksz);
		} else {
			if (chksz < mtd->writesize) {
				memset(buf, 0xff, mtd->writesize);
				memcpy(buf + padding, (void *)addr, chksz);
			}

			if (is_empty_page(io_op.datbuf, io_op.len) && !writeff)
				ret = 0;
			else
				ret = mtd_write_oob(mtd, offp, &io_op);

			if (ret)
				goto io_err;
		}

		size -= chksz;
		addr += chksz;
		off += chksz;
	}

	if (!size) {
		printf("Succeeded\n");
		ret = CMD_RET_SUCCESS;
		goto out;
	}

	printf("0x%zx byte%s remained for %s\n", size, size > 1 ? "s" : "",
	       read ? "read" : "write");
	goto err_out;

io_err:
	printf("%s error %d at 0x%llx\n", read ? "Read" : "Write", ret, offp);

err_out:
	ret = CMD_RET_FAILURE;

out:
	free(buf);
	return ret;
}

static int do_nand_io_page(int argc, char *const argv[])
{
	struct mtd_info *mtd = nand_get_curr_dev(), *part;
	bool spread = false, force = false, raw = false, autooob = false;
	bool read = false, checkbad = true, writeff = false;
	struct mtd_oob_ops io_op = {};
	uintptr_t addr;
	u64 off, offp;
	char *ends;
	u32 count;
	int ret;

	if (!mtd)
		return CMD_RET_FAILURE;

	if (!strncmp(argv[0], "read", 4))
		read = true;

	if (strstr(argv[0], ".spread"))
		spread = true;

	if (strstr(argv[0], ".force"))
		force = true;

	if (strstr(argv[0], ".raw"))
		raw = true;

	if (strstr(argv[0], ".auto"))
		autooob = true;

	if (spread && force) {
		printf("spread and force must not be set at the same time\n");
		return CMD_RET_FAILURE;
	}

	if (raw && autooob) {
		printf("raw and auto must not be set at the same time\n");
		return CMD_RET_FAILURE;
	}

	if (argc < 2) {
		printf("Data address must be specified\n");
		return CMD_RET_USAGE;
	}

	addr = simple_strtoul(argv[1], NULL, 0);

	if (argc < 3) {
		printf("Flash address/partition must be specified\n");
		return CMD_RET_USAGE;
	}

	part = nand_get_part(mtd, argv[2]);
	if (part) {
		if (argc < 4) {
			printf("Partition offset / page count must be specified\n");
			return CMD_RET_USAGE;
		}

		if (argc < 5) {
			off = 0;

			count = simple_strtoul(argv[3], NULL, 0);
			if (part->offset + count * mtd->writesize > part->size) {
				printf("Page count exceeds partition size\n");
				return CMD_RET_FAILURE;
			}
		} else {
			off = simple_strtoull(argv[3], NULL, 0);
			if (off >= part->size) {
				printf("Offset 0x%llx is larger than partition size\n", off);
				return CMD_RET_FAILURE;
			}

			off &= ~(u64)mtd->writesize_mask;

			count = simple_strtoul(argv[4], NULL, 0);
			if (part->offset + off + count * mtd->writesize > part->size) {
				printf("Page count exceeds partition size\n");
				return CMD_RET_FAILURE;
			}
		}

		off += part->offset;
	} else {
		off = simple_strtoull(argv[2], &ends, 0);

		if (ends == argv[1] || *ends) {
			printf("Partition '%s' not found\n", argv[2]);
			return CMD_RET_FAILURE;
		}

		if (off >= mtd->size) {
			printf("Offset 0x%llx is larger than flash size\n", off);
			return CMD_RET_FAILURE;
		}

		off &= ~(u64)mtd->writesize_mask;

		if (argc < 4) {
			printf("Page count must be specified\n");
			return CMD_RET_USAGE;
		}

		count = simple_strtoul(argv[3], NULL, 0);
		if (off + count * mtd->writesize > mtd->size) {
			printf("Page count exceeds flash size\n");
			return CMD_RET_FAILURE;
		}
	}

	printf("%s from 0x%llx to 0x%llx (+%u), count %u ...\n",
	       read ? "Reading" : "Writing", off,
	       off + count * mtd->writesize - 1, mtd->oobsize, count);

	while (count && off < mtd->size) {
		if (checkbad || !(off & mtd->erasesize_mask)) {
			offp = off & ~(u64)mtd->erasesize_mask;

			if (mtd_block_isbad(mtd, offp)) {
				printf("Bad block at 0x%llx", offp);

				if (spread) {
					printf(" ... skipped\n");
					off += mtd->erasesize;
					checkbad = true;
					continue;
				}

				if (!force) {
					printf(" ... aborted\n");
					return CMD_RET_FAILURE;
				}

				printf(" ... continue\n");
			}

			checkbad = false;
		}

		memset(&io_op, 0, sizeof(io_op));

		if (raw)
			io_op.mode = MTD_OPS_RAW;
		else if (autooob)
			io_op.mode = MTD_OPS_AUTO_OOB;
		else
			io_op.mode = MTD_OPS_PLACE_OOB;

		io_op.len = mtd->writesize;
		io_op.ooblen = mtd->oobsize;
		io_op.datbuf = (void *)addr;
		io_op.oobbuf = io_op.datbuf + mtd->writesize;

		if (read) {
			ret = mtd_read_oob(mtd, off, &io_op);
			if (ret && ret != -EUCLEAN && ret != -EBADMSG)
				goto io_err;
		} else {
			if (is_empty_page((void *)addr, mtd->writesize + mtd->oobsize) && !writeff)
				ret = 0;
			else
				ret = mtd_write_oob(mtd, off, &io_op);

			if (ret)
				goto io_err;
		}

		count--;
		addr += mtd->writesize + mtd->oobsize;
		off += mtd->writesize;
	}

	if (!count) {
		printf("Succeeded\n");
		return CMD_RET_SUCCESS;
	}

	printf("%u page%s remained for %s\n", count, count > 1 ? "s" : "",
	       read ? "read" : "write");
	return CMD_RET_FAILURE;

io_err:
	printf("%s error %d at 0x%llx\n", read ? "Read" : "Write", ret, off);
	return CMD_RET_FAILURE;
}

#if defined(CONFIG_ASUS_PRODUCT)
int do_nand_io(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
#else
static int do_nand_io(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
#endif
{
	if (strstr(argv[0], ".oob"))
		return do_nand_io_page(argc, argv);

	return do_nand_io_normal(argc, argv);
}

#ifdef CONFIG_SYS_LONGHELP
static char nand_help_text[] =
	"- NAND flash R/W and debugging utility\n"
	"nand list\n"
	"nand info - Show active NAND devices\n"
#if defined(CONFIG_ASUS_PRODUCT)
	"nand restoredefault - clear ENV/nvram\n"
	"nand reprobe\n"
#ifdef TEST_CMD
	"nand d1\n"
	"nand d2\n"
	"nand rescue\n"
#endif
#endif
	"nand select <name> - Select active NAND devices\n"
	"nand dump[.raw] <off>\n"
	"nand bad\n"
	"nand markbad <off>\n"
	"nand bitflip <off> <col> <bit>\n"
#if defined(DEBUG_ECC_CORRECTION)
	"nand flipbits <page_number> \n"
	"    byte_addr:bit_addr[,bit_addr][,bit_addr...]\n"
	"    [byte_addr:bit_addr[,bit_addr][,bit_addr...]\n"
	"    [byte_addr:bit_addr[,bit_addr][,bit_addr...]\n"
	"    [byte_addr:bit_addr[,bit_addr][,bit_addr...]\n"
#endif
	"nand erase[.spread|.force] [<off> <size>|<part> [<size>]]\n"
	"nand read[.spread|.force][.raw] <addr> <off> <size>\n"
	"                                <addr> <part> [<off> [<size>]]\n"
	"nand write[.spread|.force][.raw][.ff] <addr> <off> <size>\n"
	"                                      <addr> <part> [<off> [<size>]]\n"
	"nand read.oob[.spread|.force][.raw|.auto] <addr> <off> <count>\n"
	"                                          <addr> <part> [<off>] <count>\n"
	"nand write.oob[.spread|.force][.raw|.auto][.ff] <addr> <off> <count>\n"
	"                                                <addr> <part> [<off>] <count>\n";
#endif

U_BOOT_CMD_WITH_SUBCMDS(nand, "NAND utility",
	nand_help_text,
	U_BOOT_SUBCMD_MKENT(list, 1, 0, do_nand_list),
	U_BOOT_SUBCMD_MKENT(info, 1, 0, do_nand_info),
#if defined(CONFIG_ASUS_PRODUCT)
	U_BOOT_SUBCMD_MKENT(restoredefault, 1, 0, do_reset_default),
	U_BOOT_SUBCMD_MKENT(reprobe, 1, 0, do_reprobe),
#ifdef TEST_CMD
	U_BOOT_SUBCMD_MKENT(d1, 1, 0, do_d1),
	U_BOOT_SUBCMD_MKENT(d2, 1, 0, do_d2),
	U_BOOT_SUBCMD_MKENT(rescue, 1, 0, do_rescue),
#endif
#endif
	U_BOOT_SUBCMD_MKENT(select, 2, 0, do_nand_select),
	U_BOOT_SUBCMD_MKENT(dump, 2, 0, do_nand_dump),
	U_BOOT_SUBCMD_MKENT(bad, 1, 0, do_nand_bad),
	U_BOOT_SUBCMD_MKENT(markbad, 2, 0, do_nand_markbad),
	U_BOOT_SUBCMD_MKENT(bitflip, 4, 0, do_nand_bitflip),
#if defined(DEBUG_ECC_CORRECTION)
	U_BOOT_SUBCMD_MKENT(flipbits, 6, 0, do_nand_flipbits),
#endif
	U_BOOT_SUBCMD_MKENT(erase, 3, 0, do_nand_erase),
	U_BOOT_SUBCMD_MKENT(read, 5, 0, do_nand_io),
	U_BOOT_SUBCMD_MKENT(write, 5, 0, do_nand_io)
);
