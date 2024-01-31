include upb.inc
-include $(TOPPREFIX)/.config
-include $(LINUXDIR)/.config

define ll_tar
	if [ -z "$(V)" ] ; then echo "  [TAR]  $(3) -> $(3)" ; fi
	$(quiet)[ -e "$(2)" ] || mkdir -p "$(2)"
	$(quiet)cd "$(1)" && tar cf - $(3) | tar xf - -C "$(2)"
endef

prpb-y += conninfra
prpb-y += warp
prpb-y += mt_wifi
prpb-y += mii_mgr
prpb-y += libnl-tiny
prpb-y += switch
prpb-y += ated_ext
prpb-y += 8021xd
prpb-y += regs
ifeq ($(MTK_BSD),y)
prpb-y += mapd
prpb-y += wapp
endif
all: $(prpb-y) $(prpb-m)


############################################################################
# Generate short variable for destination directory.
# NOTE: Only one variable can be defined in one target.
############################################################################
$(prpb-y) $(prpb-m): S=$(PLATFORM_TOPDIR)/$@
$(prpb-y) $(prpb-m): D=$(PLATFORM_PBTOPDIR)/$@/prebuilt


############################################################################
# Define special S or D variable here.
# NOTE: Only one variable can be defined in one target.
############################################################################


############################################################################
# Copy binary
############################################################################
conninfra:
	$(call inst,$(S),$(D),source/conninfra.ko)

warp:
	$(call inst,$(S),$(D),source/mtk_warp.ko)
ifeq ($(MT7981),y)
	$(call inst,$(S),$(D),source/bin/7981_WOCPU0_RAM_CODE_release.bin)
else
	$(call inst,$(S),$(D),source/bin/7986_WOCPU0_RAM_CODE_release.bin)
	$(call inst,$(S),$(D),source/bin/7986_WOCPU1_RAM_CODE_release.bin)
endif

mt_wifi:
	$(call inst,$(S),$(D),source/mt_wifi_ap/mt_wifi.ko)
	$(call inst,$(S),$(D),source/mt_wifi/embedded/plug_in/warp_proxy/mtk_warp_proxy.ko)
ifeq ($(MT7981),y)
	$(call inst,$(S),$(D),source/bin/mt7981/rebb/$(BUILD_NAME)/MT7981_iPAiLNA_EEPROM.bin)
	$(call inst,$(S),$(D),source/bin/mt7981/rebb/$(BUILD_NAME)/MT7981_ePAeLNA_EEPROM.bin)
endif

mii_mgr:
	$(call inst,$(S),$(D),source/mii_mgr)

libnl-tiny:
	$(call inst,$(S),$(D),source/libnl-tiny.so)

switch:
	$(call inst,$(S),$(D),source/switch)

ated_ext:
	$(call inst,$(S),$(D),source/ated_ext)

8021xd:
	$(call inst,$(S),$(D),source/8021xd)

regs:
	$(call inst,$(S),$(D),source/regs)

mapd:
	$(call inst,$(S),$(D),source/bs20)
	$(call inst,$(S),$(D),source/mapd_cli)
	$(call inst,$(S),$(D),source/lib/libmapd_interface_client.so)

wapp:
	$(call inst,$(S),$(D),source/wapp)
	$(call inst,$(S),$(D),source/wappctrl)
	$(call inst,$(S),$(D),source/lib/libkvcutil.so)

.PHONY: all $(prpb-y) $(prpb-m)

