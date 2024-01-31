# SPDX-License-Identifier:	GPL-2.0

obj-y += mt7629_rfb.o

ccflags-y += -I$(srctree)/board/mediatek/common

ifndef CONFIG_SPL_BUILD
ifndef CONFIG_MTK_BOOTMENU_DEFAULT
obj-$(CONFIG_MEDIATEK_BOOTMENU) += bootmenu_mtd_legacy.o
endif
endif
