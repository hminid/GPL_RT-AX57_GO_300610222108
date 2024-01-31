define platformL2Router
	@( \
		if [ "$(MT7981)" = "y" ]; then \
			sed -i "/RTCONFIG_SOC_MT7981\>/d" $(1); \
			echo "RTCONFIG_SOC_MT7981=y" >>$(1); \
		fi; \
		if [ "$(MT7531)" = "y" ]; then \
			sed -i "/RTCONFIG_SWITCH_MT7986_MT7531\>/d" $(1); \
			echo "RTCONFIG_SWITCH_MT7986_MT7531=y" >>$(1); \
		fi; \
		if [ "$(BUILD_NAME)" = "PRT-AX57_GO" ] ; then \
			sed -i "/RTCONFIG_ZENWIFI_RGBLED\>/d" $(1); \
			echo "RTCONFIG_ZENWIFI_RGBLED=y" >>$(1); \
			sed -i "/RTCONFIG_PWMX3_RGBLED\>/d" $(1); \
			echo "RTCONFIG_PWMX3_RGBLED=y" >>$(1); \
		fi; \
	)
endef

define platformL2Busybox
	@( true )
endef

define platformL2Kernel
	@( \
		if [ "$(BUILD_NAME)" = "PRT-AX57_GO" ] ; then \
			sed -i "/CONFIG_PWM_MTK_MM\>/d" $(1); \
			echo "CONFIG_PWM_MTK_MM=y" >>$(1); \
		fi; \
	)
endef

