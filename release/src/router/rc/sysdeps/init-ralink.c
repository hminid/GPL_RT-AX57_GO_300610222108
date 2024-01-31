/*

	Copyright 2005, Broadcom Corporation
	All Rights Reserved.

	THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
	KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
	SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
	FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.

*/

#include "rc.h"

#include <termios.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <time.h>
#include <errno.h>
#include <paths.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <sys/klog.h>
#ifdef LINUX26
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#endif
#if defined(RTCONFIG_MT798X)
#include <sys/sysmacros.h>
#endif
#include <wlutils.h>
#include <bcmdevs.h>

#include <shared.h>

#ifdef RTCONFIG_RALINK
#include <ralink.h>
#include <flash_mtd.h>
#endif

#ifdef RTCONFIG_RALINK_RT3052
#include <ra3052.h>
#endif

#ifdef RTAC51U /* FIX EU2CN */
#include "rtac51u_eu2cn.h"
#endif /* RTAC51U FIX EU2CN */


void init_devs(void)
{
#if !defined(RTCONFIG_MT798X)
#define MKNOD(name,mode,dev)	if(mknod(name,mode,dev)) perror("## mknod " name)

#if defined(LINUX30) && !defined(RTN14U) && !defined(RTAC52U) && !defined(RTAC51U) && !defined(RTN11P) && !defined(RTN300) && !defined(RTN54U) && !defined(RTAC1200HP) && !defined(RTN56UB1) && !defined(RTN56UB2) && !defined(RTAC54U) && !defined(RTAC51UP) && !defined(RTAC53) && !defined(RTAC1200GA1) && !defined(RTAC1200GU) && !defined(RTAC1200) && !defined(RTAC1200V2) && !defined(RTN11P_B1) && !defined(RPAC87) && !defined(RTAC85U) && !defined(RTAC85P) && !defined(RTN800HP) && !defined(RTACRH26) && !defined(TUFAC1750) && !defined(RTACRH18) && !defined(RT4GAC86U) && !defined(RTCONFIG_WLMODULE_MT7915D_AP)
	/* Below device node are used by proprietary driver.
	 * Thus, we cannot use GPL-only symbol to create/remove device node dynamically.
	 */
	MKNOD("/dev/swnat0", S_IFCHR | 0666, makedev(210, 0));
	MKNOD("/dev/hwnat0", S_IFCHR | 0666, makedev(220, 0));
	MKNOD("/dev/acl0", S_IFCHR | 0666, makedev(230, 0));
	MKNOD("/dev/ac0", S_IFCHR | 0666, makedev(240, 0));
	MKNOD("/dev/mtr0", S_IFCHR | 0666, makedev(250, 0));
	MKNOD("/dev/rtkswitch", S_IFCHR | 0666, makedev(206, 0));
	MKNOD("/dev/nvram", S_IFCHR | 0666, makedev(228, 0));
#else
	MKNOD("/dev/video0", S_IFCHR | 0666, makedev(81, 0));
#if !defined(RTN14U) && !defined(RTAC52U) && !defined(RTAC51U) && !defined(RTN11P) && !defined(RTN300) && !defined(RTN54U) && !defined(RTAC1200HP) && !defined(RTN56UB1) && !defined(RTN56UB2) && !defined(RTAC54U) && !defined(RTAC1200GA1) && !defined(RTAC1200GU)  && !defined(RTAC1200) && !defined(RTAC1200V2) && !defined(RTN11P_B1) && !defined(RPAC87) && !defined(RTAC85U) && !defined(RTAC85P) && !defined(RTN800HP) && !defined(RTACRH26) && !defined(TUFAC1750) && !defined(RTACRH18) && !defined(RTCONFIG_WLMODULE_MT7915D_AP)
	MKNOD("/dev/rtkswitch", S_IFCHR | 0666, makedev(206, 0));
#endif
	MKNOD("/dev/spiS0", S_IFCHR | 0666, makedev(217, 0));
	MKNOD("/dev/i2cM0", S_IFCHR | 0666, makedev(218, 0));
#if defined(RTN14U) || defined(RTAC52U) || defined(RTAC51U) || defined(RTN11P) || defined(RTN300) || defined(RTN54U) || defined(RTAC1200HP) || defined(RTN56UB1) || defined(RTN56UB2) || defined(RTAC54U)
#elif defined(RTAC1200) || defined(RTAC1200V2) || defined(RTAC1200GA1) || defined(RTAC1200GU) || defined(RTN11P_B1) || defined(RPAC87) || defined(RTAC51UP) || defined(RTAC53) || defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750) || defined(RTACRH18) || defined(RT4GAC86U) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
	MKNOD("/dev/rdm0", S_IFCHR | 0x666, makedev(253, 0));
#else
	MKNOD("/dev/rdm0", S_IFCHR | 0666, makedev(254, 0));
#endif
	MKNOD("/dev/flash0", S_IFCHR | 0666, makedev(200, 0));
	MKNOD("/dev/swnat0", S_IFCHR | 0666, makedev(210, 0));
	MKNOD("/dev/hwnat0", S_IFCHR | 0666, makedev(220, 0));
	MKNOD("/dev/acl0", S_IFCHR | 0666, makedev(230, 0));
	MKNOD("/dev/ac0", S_IFCHR | 0666, makedev(240, 0));
	MKNOD("/dev/mtr0", S_IFCHR | 0666, makedev(250, 0));
	MKNOD("/dev/gpio0", S_IFCHR | 0666, makedev(252, 0));
	MKNOD("/dev/nvram", S_IFCHR | 0666, makedev(228, 0));
	MKNOD("/dev/PCM", S_IFCHR | 0666, makedev(233, 0));
	MKNOD("/dev/I2S", S_IFCHR | 0666, makedev(234, 0));
#endif
#endif // end of !defined(RTCONFIG_MT798X)
	{
		int status;
		if((status = WEXITSTATUS(modprobe("nvram_linux"))))	printf("## modprove(nvram_linux) fail status(%d)\n", status);
	}
}

void init_others(void) 
{
#if defined(RTACRH18)
    // mii_mgr_cl45 -s -p 0 -d 0x1f -r 21 -v 8009 --> WAN LED 0x21 LED Basic Control Register Set 0x8009
    eval("mii_mgr_cl45", "-s", "-p", "0", "-d", "0x1f", "-r", "21", "-v", "8009");
    // mii_mgr_cl45 -s -p 0 -d 0x1f -r 24 -v c007 --> WAN LED LED0 On Control Register
    eval("mii_mgr_cl45", "-s", "-p", "0", "-d", "0x1f", "-r", "24", "-v", "c007");
    // mii_mgr_cl45 -s -p 0 -d 0x1f -r 25 -v C13F --> WAN LED LED0 Blinking Control Register
    eval("mii_mgr_cl45", "-s", "-p", "0", "-d", "0x1f", "-r", "25", "-v", "c13f");
#elif defined(RTAX53U) || defined(RTAX54) || defined(XD4S)
	if(nvram_match("AllLED", "1"))
	{
		//mii_mgr -s -p 0 -r 13 -v 0x1f
		eval("mii_mgr", "-s", "-p", "0", "-r", "13", "-v", "0x1f");
		//mii_mgr -s -p 0 -r 14 -v 0x24
		eval("mii_mgr", "-s", "-p", "0", "-r", "14", "-v", "0x24");
		//mii_mgr -s -p 0 -r 13 -v 0x401f
		eval("mii_mgr", "-s", "-p", "0", "-r", "13", "-v", "0x401f");
		//mii_mgr -s -p 0 -r 14 -v 0xc007
		eval("mii_mgr", "-s", "-p", "0", "-r", "14", "-v", "0xc007");
	}
	//mii_mgr -s -p 0 -r 13 -v 0x1f
	eval("mii_mgr", "-s", "-p", "0", "-r", "13", "-v", "0x1f");
	//mii_mgr -s -p 0 -r 14 -v 0x25
	eval("mii_mgr", "-s", "-p", "0", "-r", "14", "-v", "0x25");
	//mii_mgr -s -p 0 -r 13 -v 0x401f
	eval("mii_mgr", "-s", "-p", "0", "-r", "13", "-v", "0x401f");
	//mii_mgr -s -p 0 -r 14 -v 0xc007
	eval("mii_mgr", "-s", "-p", "0", "-r", "14", "-v", "0x3f");

#endif	// RTACRH18

#if defined(TUFAX4200)
	if (nvram_match("HwId", "B")) {
		mount("overlay", "/www/images", "overlay", MS_MGC_VAL, "lowerdir=/TUF-AX4200Q/images:/www/images");
		mount("/rom/dlna.TUF-AX4200Q", "/rom/dlna", "none", MS_BIND, NULL);
	}
#endif
#if defined(RTCONFIG_TS_UI)
	if (nvram_match("CoBrand", "8")) {
		mount("overlay", "/www", "overlay", MS_MGC_VAL, "lowerdir=/TS_UI:/www");
		dbG("mount overlay\n");
	}
#endif

#if defined(TUFAX6000)
	if (nvram_match("odmpid", "TX-AX6000")) {
		mount("overlay", "/www/images", "overlay", MS_MGC_VAL, "lowerdir=/TX-AX6000/images:/www/images");
	}
#endif

#if defined(RTCONFIG_MT798X)
	if (nvram_match("lacp_enabled", "1"))
		f_write_string("/sys/kernel/no_dsa_offload", "1", 0, 0);
	nvram_unset("wifidat_dbg");
#endif
	return;
}

int is_if_up(char *ifname)
{
    int s;
    struct ifreq ifr;

    /* Open a raw socket to the kernel */
    if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) return -1;

    /* Set interface name */
    strlcpy(ifr.ifr_name, ifname, IFNAMSIZ);

    /* Get interface flags */
    if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0) {
        fprintf(stderr, "SIOCGIFFLAGS error\n");
    } else {
        if (ifr.ifr_flags & IFF_UP) {
            fprintf(stderr, "%s is up\n", ifname);
            close(s);
            return 1;
        }
    }
    close(s);
    return 0;
}

//void init_gpio(void)
//{
//	ralink_gpio_init(0, GPIO_DIR_OUT); // Power
//	ralink_gpio_init(13, GPIO_DIR_IN); // RESET
//	ralink_gpio_init(26, GPIO_DIR_IN); // WPS
//}

void generate_switch_para(void)
{
	int model;
	int wans_cap = get_wans_dualwan() & WANSCAP_WAN;
	int wanslan_cap = get_wans_dualwan() & WANSCAP_LAN;

	// generate nvram nvram according to system setting
	model = get_model();

	switch(model) {
		case MODEL_RTN13U:
			if(!is_routing_enabled()) {
				// override boardflags with no VLAN flag
				nvram_set_int("boardflags", nvram_get_int("boardflags")&(~BFL_ENETVLAN));
				nvram_set("lan_ifnames", "eth2 ra0");
			}
			else if(nvram_match("switch_stb_x", "1")) {
				nvram_set("vlan0ports", "0 1 2 5*");
				nvram_set("vlan1ports", "3 4 5u");
			}
			else if(nvram_match("swtich_stb_x", "2")) {
				nvram_set("vlan0ports", "0 1 3 5*");
				nvram_set("vlan1ports", "2 4 5u");
			}
			else if(nvram_match("switch_stb_x", "3")) {
				nvram_set("vlan0ports", "0 2 3 5*");
				nvram_set("vlan1ports", "1 4 5u");
			}
			else if(nvram_match("switch_stb_x", "4")) {
				nvram_set("vlan0ports", "1 2 3 5*");
				nvram_set("vlan1ports", "0 4 5u");
			}
			else if(nvram_match("switch_stb_x", "5")) {
				nvram_set("vlan0ports", "2 3 5*");
				nvram_set("vlan1ports", "0 1 4 5u");
			}
			else {	// default for 0
				nvram_set("vlan0ports", "0 1 2 3 5*");
				nvram_set("vlan1ports", "4 5u");
			}
			break;
		case MODEL_RTN11P:	/* fall through */
		case MODEL_RTN300:	/* fall through */
		case MODEL_RTN14U:	/* fall through */
		case MODEL_RTN54U:      /* fall through */
		case MODEL_RTAC54U:      /* fall through */
		case MODEL_RTN56UB1:      /* fall through */
		case MODEL_RTN56UB2:      /* fall through */
		case MODEL_RTAC1200HP:  /* fall through */
		case MODEL_RTAC51U:	/* fall through */
		case MODEL_RTAC52U:
		case MODEL_RTAC1200GA1:      /* fall through */
		case MODEL_RTAC1200GU:      /* fall through */
		case MODEL_RTAC1200:
		case MODEL_RTAC1200V2:
		case MODEL_RTACRH18:
		case MODEL_RTAX53U:
		case MODEL_XD4S:
		case MODEL_RTAX54:
		case MODEL_RT4GAX56:
		case MODEL_RTAC51UP:	/* fall through */
		case MODEL_RTAC53:
		case MODEL_RTN11P_B1:
		case MODEL_RTAC85U:
		case MODEL_RTAC85P:
		case MODEL_RTACRH26:
		case MODEL_TUFAC1750:
			nvram_unset("vlan3hwname");
			if ((wans_cap && wanslan_cap) ||
			    (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
			   )
				nvram_set("vlan3hwname", "et0");
			break;

		case MODEL_RT4GAC86U:
			nvram_unset("vlan3hwname");
			nvram_unset("vlan1hwname");
			if ((wans_cap && wanslan_cap) ||
				(wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
				) {
					nvram_set("vlan3hwname", "et0");
					nvram_set("vlan1hwname", "et0");
				}
			break;

		default:
			nvram_unset("vlan3hwname");
			if ((wans_cap && wanslan_cap) ||
			    (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
			   )
				nvram_set("vlan3hwname", "et0");

	}
}

static void init_switch_ralink(void)
{
        char *wl_ifnames;
        char word[8], *next = NULL;
	
	generate_switch_para();

	// TODO: replace to nvram controlled procedure later
#if defined(RTCONFIG_RALINK_MT7629) || defined(RTCONFIG_RALINK_MT7622) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	eval("ifconfig", "eth0", "hw", "ether", get_lan_hwaddr());
#else
	eval("ifconfig", "eth2", "hw", "ether", get_lan_hwaddr());
#endif	

#ifdef RTCONFIG_RALINK_RT3052
	if(is_routing_enabled()) config_3052(nvram_get_int("switch_stb_x"));
#else
#if !defined(RTCONFIG_CONCURRENTREPEATER)		
#if defined(RTCONFIG_AMAS)
	if (sw_mode() == SW_MODE_AP && nvram_match("re_mode", "1")) {
                wl_ifnames = strdup(nvram_safe_get("eth_ifnames"));
                if (wl_ifnames) {
                        foreach (word, wl_ifnames, next)
                        {
                                if (!nvram_match("et1macaddr", ""))
                                        eval("ifconfig", word, "hw", "ether", nvram_safe_get("et1macaddr"));
                                else
                                        eval("ifconfig", word, "hw", "ether", nvram_safe_get("et0macaddr"));
                        }
                        free(wl_ifnames);
                }
	}
	else
#endif
	{
		if(strlen(nvram_safe_get("wan0_ifname"))) {
			if (!nvram_match("et1macaddr", ""))
				eval("ifconfig", nvram_safe_get("wan0_ifname"), "hw", "ether", nvram_safe_get("et1macaddr"));
			else
				eval("ifconfig", nvram_safe_get("wan0_ifname"), "hw", "ether", nvram_safe_get("et0macaddr"));
		}
	}
#endif
#if defined(RTN56UB1) || defined(RTN56UB2) || defined(RTAC1200GA1) || defined (RTAC1200GU) || defined (RTAC85U) || defined (RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750) || defined(RTACRH18) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
//workaround, let network device initialize before config_switch()
#if !defined(RTACRH18) && !defined(RTCONFIG_WLMODULE_MT7915D_AP) && !defined(RTCONFIG_MT798X)
	eval("ifconfig", "eth2", "up");
#else
	eval("ifconfig", "eth0", "up");
#endif	
	sleep(1);		
#endif

#if !defined(RTCONFIG_RALINK_MT7628)
	config_switch();
#endif
#endif

#ifdef RTCONFIG_SHP
	if (nvram_get_int("qos_enable") == 1 || nvram_get_int("lfp_disable_force")) {
		nvram_set("lfp_disable", "1");
	} else {
		nvram_set("lfp_disable", "0");
	}

	if(nvram_get_int("lfp_disable")==0) {
		restart_lfp();
	}
#endif
#if defined(RTCONFIG_WLMODULE_MT7629_AP) || defined(RTCONFIG_SWITCH_MT7986_MT7531)
	if(nvram_get_int("jumbo_frame_enable")==0)
		eval("switch", "reg", "w", "30e0", "3e3d");		// MAX_RX_PKT_LEN: 1:1536 bytes
	else
		eval("switch", "reg", "w", "30e0", "3e3f");		// MAX_RX_PKT_LEN: 3:MAX_RX_JUMBO
#endif
//	reinit_hwnat(-1);

}

void init_switch()
{
#ifdef RTCONFIG_DSL
	init_switch_dsl();
	config_switch_dsl();	
#else
	init_switch_ralink();
#endif
}

/* Configure LED controlled by hardware. Turn off LED by default. */
void config_hwctl_led(void)
{
#if defined(TUFAX4200) || defined(TUFAX6000)
	/* GPY211 PHY LED */
	/* 2.5G LAN: LED0, port5, active high, disable LED function and turn it OFF */
	eval("mii_mgr", "-s", "-p", "5", "-d", "0", "-r", "0x1b", "-v",  "0x000");	/* Reg 0.27 */
	/* Blinks on TX/RX, ON on 10M/100M/1G/2.5G link */
	eval("mii_mgr", "-s", "-p", "5", "-d", "0x1e", "-r", "1", "-v",  "0x3f0");	/* Reg 30.1 */

	/* 2.5G WAN: LED0, port6, active low, disable LED function and turn it OFF */
	eval("mii_mgr", "-s", "-p", "6", "-d", "0", "-r", "0x1b", "-v", "0x1000");	/* Reg 0.27 */
	/* Blinks on TX/RX, ON on 10M/100M/1G/2.5G link */
	eval("mii_mgr", "-s", "-p", "6", "-d", "0x1e", "-r", "1", "-v",  "0x3f0");	/* Reg 30.1 */

	/* MT7531 switch LED */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x21",    "0x8");	/* Use LED_MODE and disabled LED */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x22",  "0xc00");	/* Link on duration */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x23", "0x1400");	/* Blink duration */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x24", "0x8000");	/* Enable LED0, active low, ON no events */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x25",    "0x0");	/* LED0, none of any events blink */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x26", "0xc007");	/* Enable LED1, active high, link 10M/100M/1G ON */
	eval("switch", "phy", "cl45", "w", "0", "0x1f", "0x27",   "0x3f");	/* LED1, blinks on 10M/100M/1G TX/RX activity */
#elif defined(RTAX52)
	set_mt7531_led(nvram_match("AllLED", "0") ? 0 : 1, 0);
#endif
}

#ifndef RTCONFIG_SWCONFIG
/**
 * Setup a VLAN.
 * @vid:	VLAN ID
 * @prio:	VLAN PRIO
 * @mask:	bit31~16:	untag mask
 * 		bit15~0:	port member mask
 * @return:
 * 	0:	success
 *  otherwise:	fail
 *
 * bit definition of untag mask/port member mask
 * 0:	Port 0, LANx port which is closed to WAN port in visual.
 * 1:	Port 1
 * 2:	Port 2
 * 3:	Port 3
 * 4:	Port 4, WAN port
 * 9:	Port 9, RGMII/MII port that is used to connect CPU and WAN port.
 * 	a. If you only have one RGMII/MII port and it is shared by WAN/LAN ports,
 * 	   you have to define two VLAN interface for WAN/LAN ports respectively.
 * 	b. If your switch chip choose another port as same feature, convert bit9
 * 	   to your own port in low-level driver.
 */
static int __setup_vlan(int vid, int prio, unsigned int mask)
{
	char vlan_str[] = "4096XXX";
	char prio_str[] = "7XXX";
	char mask_str[] = "0x00000000XXX";
	char *set_vlan_argv[] = { "rtkswitch", "36", vlan_str , NULL };
	char *set_prio_argv[] = { "rtkswitch", "37", prio_str , NULL };
	char *set_mask_argv[] = { "rtkswitch", "39", mask_str , NULL };

	if (vid > 4096) {
		_dprintf("%s: invalid vid %d\n", __func__, vid);
		return -1;
	}

	if (prio > 7)
		prio = 0;

	_dprintf("%s: vid %d prio %d mask 0x%08x\n", __func__, vid, prio, mask);

	if (vid >= 0) {
		sprintf(vlan_str, "%d", vid);
		_eval(set_vlan_argv, NULL, 0, NULL);
	}

	if (prio >= 0) {
		sprintf(prio_str, "%d", prio);
		_eval(set_prio_argv, NULL, 0, NULL);
	}

	sprintf(mask_str, "0x%08x", mask);
	_eval(set_mask_argv, NULL, 0, NULL);

	return 0;
}
#endif

#if defined(RTAC51UP) || defined(RTAC53) || defined(RT4GAC86U)
/*set internal  vlan id and associated member on port 5/6/7*/
static int _set_vlan_mbr(int vid)
{
	char vlan_str[] = "4096XXX";
	char *set_vlan_argv[] = { "mtkswitch", "1", vlan_str , NULL };

	if (vid > 4096) {
		_dprintf("%s: invalid vid %d\n", __func__, vid);
		return -1;
	}

	_dprintf("%s: vid %d \n", __func__, vid);

	if (vid >= 0) {
		sprintf(vlan_str, "%d", vid);
		_eval(set_vlan_argv, NULL, 0, NULL);
	}

	return 0;
}

/*set port accept tag only frame type for VoIP */
static int _set_portAcceptFrameType(int port)
{
	char port_str[] = "7XXX";
	char *set_portAcceptFrameType_argv[] = { "rtkswitch", "35", port_str , NULL };

	if (port >= 0) {
		sprintf(port_str, "%d", port);
		_eval(set_portAcceptFrameType_argv, NULL, 0, NULL);
	}
}
#endif

int config_switch_for_first_time = 1;
#ifndef RTCONFIG_SWCONFIG

#if defined(RTCONFIG_3LANPORT_DEVICE)
int lan_port_bit_shift = 1;
#else
#if defined(RTCONFIG_PORT2_DEVICE)
#if defined(XD4S) 
int lan_port_bit_shift = 3; //in order to specify LAN1 of switch_port_mapping
#elif defined(PRTAX57_GO)
int lan_port_bit_shift = 0; 
#else
//TBD
#endif
#else
int lan_port_bit_shift = 0;
#endif
#endif

static int stb_bitmask_shift(int input)
{
	int output = input << lan_port_bit_shift;
#if defined(RTCONFIG_PORT2_DEVICE)
	if(input!=0)
		output=1<< lan_port_bit_shift; //match LAN1 of switch_port_mapping
#endif	
	return output;
}

static int vlan_bitmask_shift(unsigned int input)
{
#define NR_WANLAN_PORT 5	
	unsigned int output = (input & 0xFFF0FFF0) | ((input & 0x000F000F) << lan_port_bit_shift);
#if defined(RTCONFIG_PORT2_DEVICE)
	int i;
	int mask=input & 0xFFF0FFF0;
	int lan_untag=(input & 0x000F0000)>>16;
	int lan_mbr  =input & 0x0000000F;
	for(i = 0; i < NR_WANLAN_PORT-1; i++) 
	{
		lan_untag=((input & 0x000F0000)>>(16+i)) & 0x1;
		lan_mbr  =((input & 0x0000000F) >>i) & 0x1;
		if(lan_mbr)
		{
			output=mask | ((lan_untag <<16) | lan_mbr) << lan_port_bit_shift;
			return output;
		}	
	}	
#endif
	return output;
}

void config_switch()
{
	int model = get_model();
	int stbport;
	int controlrate_unknown_unicast;
	int controlrate_unknown_multicast;
	int controlrate_multicast;
	int controlrate_broadcast;
	int merge_wan_port_into_lan_ports;
	int stb_bitmask;
	unsigned int vlan_bitmask;
	char stb_bitmask_str[sizeof("0xXXXXXXXXYYY")];

	dbG("link down all ports\n");
	eval("rtkswitch", "17");	// link down all ports

	switch (model) {
	case MODEL_RTN11P:	/* fall through */
	case MODEL_RTN300:	/* fall through */
	case MODEL_RTN14U:	/* fall through */
	case MODEL_RTN36U3:	/* fall through */
	case MODEL_RTN65U:	/* fall through */
	case MODEL_RTN54U:
	case MODEL_RTAC54U:   
	case MODEL_RTAC1200HP:   
	case MODEL_RTAC51U:	/* fall through */
	case MODEL_RTAC52U:	/* fall through */
	case MODEL_RTN56UB1:	/* fall through */
	case MODEL_RTN56UB2:	/* fall through */
	case MODEL_RTAC51UP:	/* fall through */
	case MODEL_RTAC53:	/* fall through */
	case MODEL_RTAC1200GA1:	/* fall through */
	case MODEL_RTAC1200GU:	/* fall through */
	case MODEL_RTAC1200:	/* fall through */
	case MODEL_RTAC1200V2:	/* fall through */
	case MODEL_RTACRH18:
	case MODEL_RTAX53U:
	case MODEL_XD4S:
	case MODEL_RTAX54:
	case MODEL_RT4GAC86U:
	case MODEL_RT4GAX56:
	case MODEL_RTN11P_B1:	
	case MODEL_RTAC85U:
	case MODEL_RTAC85P:
	case MODEL_RTACRH26:
	case MODEL_RPAC87:
	case MODEL_RTN800HP:
	case MODEL_TUFAC1750:
		merge_wan_port_into_lan_ports = 1;
		break;
	default:
		merge_wan_port_into_lan_ports = 0;
	}

	if (config_switch_for_first_time){
#if defined(RTAC51UP) || defined(RTAC53) // we have to do software reset,because the register can't be empty
		if(!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", ""))
		{
			dbG("software reset\n");
			eval("rtkswitch", "27");
		}
#endif		
			config_switch_for_first_time = 0;
	}
	else
	{
		dbG("software reset\n");
		eval("rtkswitch", "27");	// software reset
	}

	pre_config_switch();

#if defined(RTN14U) || defined(RTAC52U) || defined(RTAC51U) || defined(RTN11P) || defined(RTN300) || defined(RTN54U) || defined(RTAC1200HP) || defined(RTN56UB1) || defined(RTAC54U) || defined(RTN56UB2) || defined(RTAC51UP)  || defined(RTAC53)|| defined(RTAC1200GA1) || defined(RTAC1200GU) || defined(RTAC1200) || defined(RTAC1200V2) || defined(RTN11P_B1) || defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750) || defined(RTACRH18) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	system("rtkswitch 8 0"); //Barton add
#endif

#if defined(RTAC53) || defined(RTAC51UP) 
	system("mtkswitch 0"); //internal switch port5/6/7 set user mode
#endif

	if (is_routing_enabled())
	{
		char parm_buf[] = "XXX";

		stbport = nvram_get_int("switch_stb_x");

		if (stbport < 0 || stbport > 6) stbport = 0;
#if defined(RTCONFIG_PORT2_DEVICE)
		if(stbport!=0)
			stbport=1;
#endif			


		dbG("ISP Profile/STB: %s/%d\n", nvram_safe_get("switch_wantag"), stbport);
#if defined(RTCONFIG_RALINK_MT7628)
		/* P0    P1    P2    P3    P4    P6  */
		/* WAN   L1    L2    L3    L4    CPU */
#elif defined(RTAC53)
		/* P0    P1    P2    P3    P4    P6    */
		/* WAN   NA    NA    L1    L2    GMAC1 */
		
		/* Convert STB port value for RTAC53*/
		switch (stbport) {
			case 1: // P1 -> P3
				stbport = 3;
				break;
			case 2: // P2 -> P4
				stbport = 4;
				break;
			case 5: // P1&P2 -> P3&P4
				stbport = 6;
				break;
			default:
				break; /* Nothing to do. */
		}
#else
		/* stbport:	Model-independent	unifi_malaysia=1	otherwise
		 * 		IPTV STB port		(RT-N56U)		(RT-N56U)
		 * -----------------------------------------------------------------------
		 *	0:	N/A			LLLLW
		 *	1:	LAN1			LLLTW			LLLWW
		 *	2:	LAN2			LLTLW			LLWLW
		 *	3:	LAN3			LTLLW			LWLLW
		 *	4:	LAN4			TLLLW			WLLLW
		 *	5:	LAN1 + LAN2		LLTTW			LLWWW
		 *	6:	LAN3 + LAN4		TTLLW			WWLLW
		 */
#endif
		if(!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", ""))//2012.03 Yau modify
		{
			int voip_port __attribute__((unused))= 0;
			int t, vlan_val = -1, prio_val = -1;
			unsigned int mask = 0;

#if defined(RTCONFIG_RALINK_MT7628)
			/* Create WAN VLAN interface */
			if (nvram_get_int("switch_wan0tagid") != 0) {
				char wan_dev[10];
				//eval("vconfig", "rem", "vlan2");
				eval("vconfig", "add", "eth2", nvram_safe_get("switch_wan0tagid"));

				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", nvram_get_int("switch_wan0tagid"));

				prio_val = nvram_get_int("switch_wan1prio");
				if (prio_val >= 0 && prio_val <= 7)
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
			}
#endif
#if defined(RTCONFIG_RALINK_MT7629)
			/* Create WAN VLAN interface */
			if (nvram_get_int("switch_wan0tagid") != 0) {
				char wan_dev[10];
				eval("vconfig", "rem", "vlan2");
				eval("vconfig", "add", "eth2", nvram_safe_get("switch_wan0tagid"));

				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", nvram_get_int("switch_wan0tagid"));

				prio_val = nvram_get_int("switch_wan1prio");
				if (prio_val >= 0 && prio_val <= 7)
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
			}
#endif			
			switch (model) {
#if defined(RTAC51UP) || defined(RTAC53)
			case MODEL_RTAC51UP:
			case MODEL_RTAC53:

				/* Fixed Ports Now*/
				stbport = 4;
				voip_port = 3;

				if(!strncmp(nvram_safe_get("switch_wantag"), "unifi", 5)) {
					if(strstr(nvram_safe_get("switch_wantag"), "home")) {
						system("rtkswitch 38 16");		/* IPTV: P4 */
						/* Internet:	port: P0, P9 */
						__setup_vlan(500, 0, 0x00000201);
						/* IPTV:	untag: P4;   port: P4, P0 */
						__setup_vlan(600, 0, 0x00100011);
						/* internal switch p5/6/7  add vlan id member*/
						_set_vlan_mbr(500);
						_set_vlan_mbr(600);
					}
					else {
						/* No IPTV. Business package */
						/* Internet:	port: P0, P9 */
						system("rtkswitch 38 0");
						__setup_vlan(500, 0, 0x00000201);
						/* internal switch p5/6/7  add vlan id member*/
						_set_vlan_mbr(500);
					}
				}
				else if(!strncmp(nvram_safe_get("switch_wantag"), "singtel", 7)) {
					if(strstr(nvram_safe_get("switch_wantag"), "mio")) {
						/* Connect Singtel MIO box to P4 */
						system("rtkswitch 40 1");		/* admin all frames on all ports */
						system("rtkswitch 38 24");		/* IPTV: P4  VoIP: P3 */
						/* Internet:	port: P0, P9 */
						__setup_vlan(10, 0, 0x00000201);
						/* VoIP:	untag: N/A;  port: P3, P0 */
						//VoIP Port: P3 tag
						__setup_vlan(30, 4, 0x00000009);
						/* internal switch p5/6/7  add vlan id member*/
						_set_vlan_mbr(10);
						_set_vlan_mbr(30);
						/* keep vlan tag for egress packet on port 5 for VoIP*/
						eval("mtkswitch", "2", "5");
						/* p3 only accept tagged packet*/
						_set_portAcceptFrameType(3);
					}
					else {
						//Connect user's own ATA to lan port and use VoIP by Singtel WAN side VoIP gateway at voip.singtel.com
						system("rtkswitch 38 16");		/* IPTV: P4 */
						/* Internet:	port: P0, P9 */
						__setup_vlan(10, 0, 0x00000201);
						/* internal switch p5/6/7  add vlan id member*/
						_set_vlan_mbr(10);
						
					}

					/* IPTV */
					__setup_vlan(20, 4, 0x00100011);		/* untag: P4;   port: P4, P0 */
					/* internal switch p5/6/7  add vlan id member*/
					_set_vlan_mbr(20);

				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "m1_fiber")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					system("rtkswitch 38 8");			/* VoIP: P3  8 = 0x1000 */
					/* Internet:	port: P0, P9 */
					__setup_vlan(1103, 1, 0x00000201);
					/* VoIP:	untag: N/A;  port: P3, P0 */
					//VoIP Port: P3 tag
					__setup_vlan(1107, 1, 0x00000009);
					/* internal switch p5/6/7  add vlan id member*/
					_set_vlan_mbr(1103);
					_set_vlan_mbr(1107);
					/* keep vlan tag for egress packet on port 5 for VoIP*/
					eval("mtkswitch", "2", "5");
					/* p3 only accept tagged packet*/
					_set_portAcceptFrameType(3);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					system("rtkswitch 38 8");			/* VoIP: P3  8 = 0x1000 */
					/* Internet:	port: P0, P9 */
					__setup_vlan(621, 0, 0x00000201);
					/* VoIP:	untag: N/A;  port: P3, P0 */
					__setup_vlan(821, 0, 0x00000009);

					__setup_vlan(822, 0, 0x00000009);		/* untag: N/A;  port: P3, P0 */ //VoIP Port: P3 tag

					/* internal switch p5/6/7  add vlan id member*/
					_set_vlan_mbr(621);
					_set_vlan_mbr(821);
					_set_vlan_mbr(822);
					/* keep vlan tag for egress packet on port 5 for VoIP*/
					eval("mtkswitch", "2", "5");
					/* p3 only accept tagged packet*/
					_set_portAcceptFrameType(3);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber_sp")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					system("rtkswitch 38 8");			/* VoIP: P3  8 = 0x1000 */
					/* Internet:	port: P0, P9 */
					__setup_vlan(11, 0, 0x00000201);
					/* VoIP:	untag: N/A;  port: P3, P0 */
					//VoIP Port: P3 tag
					__setup_vlan(14, 0, 0x00000009);
					/* internal switch p5/6/7  add vlan id member*/
					_set_vlan_mbr(11);
					_set_vlan_mbr(14);
					/* keep vlan tag for egress packet on port 5 for VoIP*/
					eval("mtkswitch", "2", "5");
					/* p3 only accept tagged packet*/
					_set_portAcceptFrameType(3);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_cts")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					system("rtkswitch 38 8");			/* VoIP: P3  8 = 0x1000 */
					/* Internet:	port: P0, P9 */
					__setup_vlan(41, 1, 0x00000201);
					/* VoIP:	untag: N/A;  port: P3, P0 */
					//VoIP Port: P3 tag
					__setup_vlan(44, 1, 0x00000009);
					/* internal switch p5/6/7  add vlan id member*/
					_set_vlan_mbr(41);
					_set_vlan_mbr(44);
					/* keep vlan tag for egress packet on port 5 for VoIP*/
					eval("mtkswitch", "2", "5");
					/* p3 only accept tagged packet*/
					_set_portAcceptFrameType(3);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_sacofa")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					system("rtkswitch 38 8");			/* VoIP: P3  8 = 0x1000 */
					/* Internet:	port: P0, P9 */
					__setup_vlan(31, 1, 0x00000201);
					/* VoIP:	untag: N/A;  port: P3, P0 */
					//VoIP Port: P3 tag
					__setup_vlan(34, 1, 0x00000009);
					/* internal switch p5/6/7  add vlan id member*/
					_set_vlan_mbr(31);
					_set_vlan_mbr(34);
					/* keep vlan tag for egress packet on port 5 for VoIP*/
					eval("mtkswitch", "2", "5");
					/* p3 only accept tagged packet*/
					_set_portAcceptFrameType(3);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_tnb")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					system("rtkswitch 38 8");			/* VoIP: P3  8 = 0x1000 */
					/* Internet:	port: P0, P9 */
					__setup_vlan(51, 1, 0x00000201);
					/* VoIP:	untag: N/A;  port: P3, P0 */
					//VoIP Port: P3 tag
					__setup_vlan(54, 1, 0x00000009);
					/* internal switch p5/6/7  add vlan id member*/
					_set_vlan_mbr(51);
					_set_vlan_mbr(54);
					/* keep vlan tag for egress packet on port 5 for VoIP*/
					eval("mtkswitch", "2", "5");
					/* p3 only accept tagged packet*/
					_set_portAcceptFrameType(3);
				}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "hinet")) { /* Hinet MOD */
					eval("rtkswitch", "8", "4");			/* LAN4 with WAN */
					/* internal switch p5/6/7  add vlan id member*/
					_set_vlan_mbr(2);
				}	
				else {
					/* Initialize VLAN and set Port Isolation */
					if(strcmp(nvram_safe_get("switch_wan1tagid"), "") && strcmp(nvram_safe_get("switch_wan2tagid"), ""))
						system("rtkswitch 38 24");		// 24 = 0x11000 IPTV: P4  VoIP: P3
					else if(strcmp(nvram_safe_get("switch_wan1tagid"), ""))
						system("rtkswitch 38 16");		// 16 = 0x10000 IPTV: P4
					else if(strcmp(nvram_safe_get("switch_wan2tagid"), ""))
						system("rtkswitch 38 8");		//  8 = 0x1000 VoIP: P3
					else
						system("rtkswitch 38 0");		//No IPTV and VoIP ports

					/*++ Get and set Vlan Information */
					t = nvram_get_int("switch_wan0tagid") & 0x0fff;
					if (t != 0) {
						// Internet on WAN (port 0)
						if (t >= 2 && t <= 4094)
							vlan_val = t;
 
						prio_val = nvram_get_int("switch_wan0prio") & 0x7;
	
						__setup_vlan(vlan_val, prio_val, 0x00000201);

						/* internal switch p5/6/7  add vlan id member*/
						_set_vlan_mbr(vlan_val);
					}
	
					t = nvram_get_int("switch_wan1tagid") & 0x0fff;
					if (t != 0) {
						// IPTV on LAN4 (port 4)
						if (t >= 2 && t <= 4094)
							vlan_val = t;

						prio_val = nvram_get_int("switch_wan1prio") & 0x7;

						if (t == nvram_get_int("switch_wan2tagid"))
							mask = 0x00180019;	//IPTV=VOIP
						else
							mask = 0x00100011;	//IPTV Port: P4 untag 1048593 = 0x10 0011

						__setup_vlan(vlan_val, prio_val, mask);

						/* internal switch p5/6/7  add vlan id member*/
						_set_vlan_mbr(vlan_val);

						/* keep vlan tag for egress packet on port 5 for VoIP*/
						eval("mtkswitch", "2", "5");
					}	

					t = nvram_get_int("switch_wan2tagid") & 0x0fff;
					if (t != 0) {
						// VoIP on LAN3 (port 3)
						if (t >= 2 && t <= 4094)
							vlan_val = t;

						prio_val = nvram_get_int("switch_wan2prio") & 0x7;

						if (t == nvram_get_int("switch_wan1tagid"))
							mask = 0x00180019;	//IPTV=VOIP
						else
							mask = 0x00080009;	//VoIP Port: P3 untag

						__setup_vlan(vlan_val, prio_val, mask);

						/* internal switch p5/6/7  add vlan id member*/
						_set_vlan_mbr(vlan_val);
						
						/* keep vlan tag for egress packet on port 5 for VoIP*/
						eval("mtkswitch", "2", "5");
					}
				}
				break;
#endif
			default:
				/* Fixed Ports Now*/
#if defined(RTCONFIG_PORT2_DEVICE)
				stbport= 1;
#else
				stbport = 4;	
#endif				
				voip_port = 3;
				
				if(!strncmp(nvram_safe_get("switch_wantag"), "unifi", 5)) {
					/* Added for Unifi. Cherry Cho modified in 2011/6/28.*/
					if(strstr(nvram_safe_get("switch_wantag"), "home")) {
						stb_bitmask = stb_bitmask_shift(1);
						snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
						eval("rtkswitch", "38", stb_bitmask_str);	/* IPTV: P0 */

						/* Internet:	untag: P9;   port: P4, P9 */
						vlan_bitmask = vlan_bitmask_shift(0x02000210);
						__setup_vlan(500, 0, vlan_bitmask);

						/* IPTV:	untag: P0;   port: P0, P4 */
						vlan_bitmask = vlan_bitmask_shift(0x00010011);
						__setup_vlan(600, 0, vlan_bitmask);
					}
					else if (strstr(nvram_safe_get("switch_wantag"), "biz_voip")) {
						system("rtkswitch 40 1");		/* admin all frames on all ports */
						stb_bitmask = stb_bitmask_shift(2);
						snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
						eval("rtkswitch", "38", stb_bitmask_str);	/* VoIP: P1  2 = 0x10 */

						/* Internet:	untag: P9;   port: P4, P9 */
						vlan_bitmask = vlan_bitmask_shift(0x02000210);
						__setup_vlan(500, 0, vlan_bitmask);

						/* VoIP:	untag: P1;  port: P1, P4 */
						//VoIP Port: P1 untag (special case)
						vlan_bitmask = vlan_bitmask_shift(0x00020012);
						__setup_vlan(400, 0, vlan_bitmask);
					}
					else {
						/* No IPTV. Business package */
						/* Internet:	untag: P9;   port: P4, P9 */
						stb_bitmask = stb_bitmask_shift(0);
						snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
						eval("rtkswitch", "38", stb_bitmask_str);

						vlan_bitmask = vlan_bitmask_shift(0x02000210);
						__setup_vlan(500, 0, vlan_bitmask);
					}
				}
				else if(!strncmp(nvram_safe_get("switch_wantag"), "singtel", 7)) {
					/* Added for SingTel's exStream issues. Cherry Cho modified in 2011/7/19. */
					if(strstr(nvram_safe_get("switch_wantag"), "mio")) {
						/* Connect Singtel MIO box to P3 */
						system("rtkswitch 40 1");		/* admin all frames on all ports */
						stb_bitmask = stb_bitmask_shift(3);
						snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
						eval("rtkswitch", "38", stb_bitmask_str);	/* IPTV: P0  VoIP: P1 */

						/* Internet:	untag: P9;   port: P4, P9 */
						vlan_bitmask = vlan_bitmask_shift(0x02000210);
						__setup_vlan(10, 0, vlan_bitmask);

						/* VoIP:	untag: N/A;  port: P1, P4 */
						//VoIP Port: P1 tag
						vlan_bitmask = vlan_bitmask_shift(0x00000012);
						__setup_vlan(30, 4, vlan_bitmask);
					}
					else {
						//Connect user's own ATA to lan port and use VoIP by Singtel WAN side VoIP gateway at voip.singtel.com
						stb_bitmask = stb_bitmask_shift(1);
						eval("rtkswitch", "38", stb_bitmask_str);	/* IPTV: P0 */

						/* Internet:	untag: P9;   port: P4, P9 */
						vlan_bitmask = vlan_bitmask_shift(0x02000210);
						__setup_vlan(10, 0, vlan_bitmask);
					}

					/* IPTV */
					if(strstr(nvram_safe_get("switch_wantag"), "mstb"))
						vlan_bitmask = vlan_bitmask_shift(0x00030013);	/* untag: P0, P1;   port: P0, P1, P4 */
					else
						vlan_bitmask = vlan_bitmask_shift(0x00010011);	/* untag: P0;   port: P0, P4 */

					__setup_vlan(20, 4, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "m1_fiber")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/1/13.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(1103, 1, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(1107, 1, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(621, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(821, 0, vlan_bitmask);

					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(822, 0, vlan_bitmask);		/* untag: N/A;  port: P1, P4 */ //VoIP Port: P1 tag
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber_sp")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(11, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(14, 0, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_cts")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(41, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(44, 0, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_sacofa")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(31, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(34, 0, vlan_bitmask);
				}
				else if(!strcmp(nvram_safe_get("switch_wantag"), "maxis_tnb")) {
					//VoIP: P1 tag. Cherry Cho added in 2012/11/6.
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(2);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);			/* VoIP: P1  2 = 0x10 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000210);
					__setup_vlan(51, 0, vlan_bitmask);

					/* VoIP:	untag: N/A;  port: P1, P4 */
					//VoIP Port: P1 tag
					vlan_bitmask = vlan_bitmask_shift(0x00000012);
					__setup_vlan(54, 0, vlan_bitmask);
				}
#ifdef RTCONFIG_MULTICAST_IPTV
			else if (!strcmp(nvram_safe_get("switch_wantag"), "movistar")) {
#if 0	//set in set_wan_tag() since (switch_stb_x > 6) and need vlan interface by vconfig.
				system("rtkswitch 40 1");			/* admin all frames on all ports */
				/* Internet/STB/VoIP:	untag: N/A;   port: P4, P9 */
				__setup_vlan(6, 0, 0x00000210);
				__setup_vlan(2, 0, 0x00000210);
				__setup_vlan(3, 0, 0x00000210);
#endif
#if defined(RTCONFIG_MT798X)
				/* IPTV tag only NIC */
				//doSystem("echo 1 > /sys/class/net/%s/vlan_only", nvram_safe_get("wan0_gw_ifname"));
				doSystem("echo 1 > /sys/class/net/%s/vlan_only", "eth1");
#endif
			}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "starhub")) {
					//skip setting any lan port to IPTV port.
					stb_bitmask = stb_bitmask_shift(0);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);
#if defined(RTCONFIG_RALINK_MT7620) || defined(RTCONFIG_RALINK_MT7621)
					vlan_bitmask = vlan_bitmask_shift(0x02100210);
					__setup_vlan(2, 0, vlan_bitmask);
#endif
				}
#endif
				else if (!strcmp(nvram_safe_get("switch_wantag"), "meo")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(1);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);			/* VoIP: P0 */

					/* Internet/VoIP:	untag: P9;   port: P0, P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000211);
					__setup_vlan(12, 0, vlan_bitmask);
				}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "vodafone")) {
					system("rtkswitch 40 1");			/* admin all frames on all ports */
					stb_bitmask = stb_bitmask_shift(3);
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);		/* Vodafone: P0  IPTV: P1 */

					/* Internet:	untag: P9;   port: P4, P9 */
					vlan_bitmask = vlan_bitmask_shift(0x02000211);
					__setup_vlan(100, 1, vlan_bitmask);

					/* IPTV:	untag: N/A;  port: P0, P4 */
					vlan_bitmask = vlan_bitmask_shift(0x00000011);
					__setup_vlan(101, 0, vlan_bitmask);

					/* Vodafone:	untag: P1;   port: P0, P1, P4 */
					vlan_bitmask = vlan_bitmask_shift(0x00020013);
					__setup_vlan(105, 1, vlan_bitmask);
				}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "hinet")
				      || !strcmp(nvram_safe_get("switch_wantag"), "nowtv")) {
					if (sw_bridge_iptv_different_switches()) {
						/* Bridge:	untag: P0, P4, P9;	port: P0, P4, P9
						 * WAN:		no VLAN (hacked in API for SW based IPTV)
						 * STB:		Ctag, return value of get_sw_bridge_iptv_vid().
						 */
						__setup_vlan(get_sw_bridge_iptv_vid(), 0, 0x02110211);
					} else {
#if defined(RTCONFIG_3LANPORT_DEVICE)
					eval("rtkswitch", "8", "3");			/* LAN3 with WAN */
#elif defined(RTCONFIG_PORT2_DEVICE)
					eval("rtkswitch", "8", "1");			/* LAN1 with WAN */
#else					
					eval("rtkswitch", "8", "4");			/* LAN4 with WAN */
#endif
					}
				}
				else if (!strcmp(nvram_safe_get("switch_wantag"), "hinet_mesh")) { /* Hinet MOD Mesh */
					/* Nothing to do. */
				}
				else {
					/* Cherry Cho added in 2011/7/11. */
					/* Initialize VLAN and set Port Isolation */
					if(strcmp(nvram_safe_get("switch_wan1tagid"), "") && strcmp(nvram_safe_get("switch_wan2tagid"), ""))
						stb_bitmask = stb_bitmask_shift(3);		// 3 = 0x11 IPTV: P0  VoIP: P1
					else if(strcmp(nvram_safe_get("switch_wan1tagid"), ""))
						stb_bitmask = stb_bitmask_shift(1);		// 1 = 0x01 IPTV: P0
					else if(strcmp(nvram_safe_get("switch_wan2tagid"), ""))
						stb_bitmask = stb_bitmask_shift(2);		// 2 = 0x10 VoIP: P1
					else
						stb_bitmask = stb_bitmask_shift(0);		//No IPTV and VoIP ports
					snprintf(stb_bitmask_str, sizeof(stb_bitmask_str), "0x%x", stb_bitmask);
					eval("rtkswitch", "38", stb_bitmask_str);

					/*++ Get and set Vlan Information */
					t = nvram_get_int("switch_wan0tagid") & 0x0fff;
					if (t != 0) {

						// Internet on WAN (port 4)
						if (t >= 2 && t <= 4094)
							vlan_val = t;

						prio_val = nvram_get_int("switch_wan0prio") & 0x7;

						vlan_bitmask = vlan_bitmask_shift(0x02000210);

						__setup_vlan(vlan_val, prio_val, vlan_bitmask);
					}
#if defined(RTCONFIG_RALINK_MT7620) || defined(RTCONFIG_RALINK_MT7621)
					else {
						/* Internet: untag: P4, P9; port: P4, P9 */
						vlan_bitmask = vlan_bitmask_shift(0x02100210);
						__setup_vlan(2, 0, vlan_bitmask);
					}
#endif
	
					t = nvram_get_int("switch_wan1tagid") & 0x0fff;
					if (t != 0) {
						// IPTV on LAN4 (port 0)
						if (t >= 2 && t <= 4094)
							vlan_val = t;
						
						prio_val = nvram_get_int("switch_wan1prio") & 0x7;

						if (t == nvram_get_int("switch_wan2tagid"))
							mask = 0x00030013;	//IPTV=VOIP
						else
							mask = 0x00010011;	//IPTV Port: P0 untag 65553 = 0x10 011
						vlan_bitmask = vlan_bitmask_shift(mask);
						__setup_vlan(vlan_val, prio_val, vlan_bitmask);
					}

					t = nvram_get_int("switch_wan2tagid") & 0x0fff;
					if (t != 0) {
						// VoIP on LAN3 (port 1)
						if (t >= 2 && t <= 4094)
							vlan_val = t;

						prio_val = nvram_get_int("switch_wan2prio") & 0x7;

						if (t == nvram_get_int("switch_wan1tagid"))
							mask = 0x00030013;	//IPTV=VOIP
						else
							mask = 0x00020012;	//VoIP Port: P1 untag

						vlan_bitmask = vlan_bitmask_shift(mask);
						__setup_vlan(vlan_val, prio_val, vlan_bitmask);
					}
				}
			}
		}
		else /* switch_wantag empty case */
		{
			const int sw_br_vid = get_sw_bridge_iptv_vid();

			if (stbport) {
				sprintf(parm_buf, "%d", stbport);
				eval("rtkswitch", "8", parm_buf);
			}
			if (sw_based_iptv()) {
				/* WAN:	no VLAN (hacked in API for SW based IPTV)
				 * STB:	according to switch_stb_x nvram variable.
				 */
				switch (stbport) {
				case 0:	/* none */
					break;
				case 1:	/* LAN1 */
					/* untag: P3, P4, P9;	port: P3, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02180218);
					break;
				case 2:	/* LAN2 */
					/* untag: P2, P4, P9;	port: P2, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02140214);
					break;
				case 3:	/* LAN3 */
					/* untag: P1, P4, P9;	port: P1, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02120212);
					break;
				case 4:	/* LAN4 */
					/* untag: P0, P4, P9;	port: P0, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02110211);
					break;
				case 5:	/* LAN1 & LAN2 */
					/* untag: P3, P2, P4, P9;	port: P3, P2, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x021C021C);
					break;
				case 6:	/* LAN3 & LAN4 */
					/* untag: P1, P0, P4, P9;	port: P1, P0, P4, P9 */
					__setup_vlan(sw_br_vid, 0, 0x02130213);
					break;
				default:
					dbg("%s: unknown stb_stb_x %d\n", __func__, nvram_get_int("switch_stb_x"));
				}
			}
		}

		/* unknown unicast storm control */
		if (!nvram_get("switch_ctrlrate_unknown_unicast"))
			controlrate_unknown_unicast = 0;
		else
			controlrate_unknown_unicast = nvram_get_int("switch_ctrlrate_unknown_unicast");
		if (controlrate_unknown_unicast < 0 || controlrate_unknown_unicast > 1024)
			controlrate_unknown_unicast = 0;
		if (controlrate_unknown_unicast)
		{
			sprintf(parm_buf, "%d", controlrate_unknown_unicast);
			eval("rtkswitch", "22", parm_buf);
		}
	
		/* unknown multicast storm control */
		if (!nvram_get("switch_ctrlrate_unknown_multicast"))
			controlrate_unknown_multicast = 0;
		else
			controlrate_unknown_multicast = nvram_get_int("switch_ctrlrate_unknown_multicast");
		if (controlrate_unknown_multicast < 0 || controlrate_unknown_multicast > 1024)
			controlrate_unknown_multicast = 0;
		if (controlrate_unknown_multicast)
		{
			sprintf(parm_buf, "%d", controlrate_unknown_multicast);
			eval("rtkswitch", "23", parm_buf);
		}
	
		/* multicast storm control */
		if (!nvram_get("switch_ctrlrate_multicast"))
			controlrate_multicast = 0;
		else
			controlrate_multicast = nvram_get_int("switch_ctrlrate_multicast");
		if (controlrate_multicast < 0 || controlrate_multicast > 1024)
			controlrate_multicast = 0;
		if (controlrate_multicast)
		{
			sprintf(parm_buf, "%d", controlrate_multicast);
			eval("rtkswitch", "24", parm_buf);
		}
	
		/* broadcast storm control */
		if (!nvram_get("switch_ctrlrate_broadcast"))
			controlrate_broadcast = 0;
		else
			controlrate_broadcast = nvram_get_int("switch_ctrlrate_broadcast");
		if (controlrate_broadcast < 0 || controlrate_broadcast > 1024)
			controlrate_broadcast = 0;
		if (controlrate_broadcast)
		{
			sprintf(parm_buf, "%d", controlrate_broadcast);
			eval("rtkswitch", "25", parm_buf);
		}

#ifdef RTN56U
		if (nvram_match("switch_wanport_force_1g", "1"))
			eval("rtkswitch", "26");
#endif
	}
	else if (access_point_mode())
	{
		if (merge_wan_port_into_lan_ports)
			eval("rtkswitch", "8", "100");
	}
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_PROXYSTA)
	else if (mediabridge_mode())
	{
		if (merge_wan_port_into_lan_ports)
			eval("rtkswitch", "8", "100");
	}
#endif

#ifdef RTCONFIG_DSL
	dbG("link up all ports\n");
	eval("rtkswitch", "16");	// link up all ports
#else
	dbG("link up wan port(s)\n");
	eval("rtkswitch", "114");	// link up wan port(s)
#endif

	post_config_switch();

#if defined(RTCONFIG_BLINK_LED)
	if (is_swports_bled("led_lan_gpio")) {
		update_swports_bled("led_lan_gpio", nvram_get_int("lanports_mask"));
	}
	if (is_swports_bled("led_wan_gpio")) {
		update_swports_bled("led_wan_gpio", nvram_get_int("wanports_mask"));
	}
#if defined(RTCONFIG_WANLEDX2)
	if (is_swports_bled("led_wan2_gpio")) {
		update_swports_bled("led_wan2_gpio", nvram_get_int("wan1ports_mask"));
	}
#endif
#endif
}
#endif //RTCONFIG_SWCONFIG

#if defined(RTCONFIG_MT798X)
void wan_force_link_sp(int unit)
{
	int wan_force_link, value;
	int port;

	if (find_word(nvram_safe_get("rc_support"), "wan_sp") == NULL)
		return;

	if (unit != 0)
		return;

#if defined(TUFAX4200) || defined(TUFAX6000)
	port = 6;
#elif defined(RTAX59U) || defined(RTAX52)
	port = 1;
#elif defined(PRTAX57_GO)
	port = 0;
#else
#error port need to be defined
#endif

	wan_force_link = nvram_get_int("wan0_force_link");
	switch (wan_force_link) {
		case 1:	//10Mbps
			value = 0;
			break;
		case 2:	//100Mbps
			value = 0x2000;
			break;
		case 3:	//1000Mbps
			value = 0x0040;
			break;
		case 4:	//2500Mbps
			value = 0x2040;
			break;
		case 0:	//auto
		default:
			value = 0x3040;
			break;
	}
	doSystem("mii_mgr -s -p %d -d 0 -r 0 -v 0x%x", port, value);
}
#endif	//RTCONFIG_MT798X

#ifdef RTCONFIG_MULTILAN_CFG
int get_vlan(char *vlan_format)
{
       char *nv = NULL, *nvp=NULL, *b;
       char vlan[5];
       /* basic necessary parameters */
       char *idx, *vid;
       /* continue to add parameters */
       char *port_isolation = NULL;
       size_t cnt = 0;

       if (!(nvp = nv = strdup(nvram_safe_get("vlan_rl"))))
       {
	       printf("sdn vlan get fail\n");
	       return -1;
       }

       while ((b = strsep(&nvp, "<")) != NULL) {
	       if (vstrsep(b, ">", &idx, &vid, &port_isolation) < VLAN_LIST_BASIC_PARAM)
		       continue;

	       if (cnt >= MTLAN_MAXINUM)
		       break;

	       if (vid && *vid)
	       {
		       if(cnt!=0)
			       strncat(vlan_format,">",1);
		       snprintf(vlan,sizeof(vlan),"%d",strtol(vid, NULL, 10));
		       strncat(vlan_format,vlan,strlen(vlan));
	       }
	       cnt++;
        }
       free(nv);
       return 0;
}
#endif

int
switch_exist(void)
{
	int ret;
#ifdef RTCONFIG_DSL
	// 0 means switch exist
	ret = 0;
#else
	ret = eval("rtkswitch", "41");
	_dprintf("eval(rtkswitch, 41) ret(%d)\n", ret);
#endif
	return (ret == 0);
}

void init_wl(void)
{
	unsigned char buffer[16];
	unsigned char *dst;
	char tmpStr1[16];
	char tmpStr2[24];
	char tmpStr3[24];
	char cmd[1024];
	int i;
#if defined(RTCONFIG_MT798X)
	char iptv_vids[32], vid[100], str[120];
#endif

	memset(tmpStr1, 0, sizeof(tmpStr1));
	memset(tmpStr2, 0, sizeof(tmpStr2));
	memset(tmpStr3, 0, sizeof(tmpStr3));
	memset(cmd, 0, sizeof(cmd));
	dst = buffer;
	memset(buffer, 0, sizeof(buffer));
	memset(dst, 0, MAX_REGSPEC_LEN+1);
	
	if(FRead(dst, REGSPEC_ADDR, MAX_REGSPEC_LEN) < 0)
	{
		_dprintf("READ REGSPEC_ADDR ERROR\n");
	}
	else
	{
		for(i = 0; i < MAX_REGSPEC_LEN && dst[i] != '\0'; i++) {
			if (dst[i] == 0xff)
			{
				dst[i] = '\0';
				break;
			}
		}
	}
	sprintf(tmpStr1, "regspec=%s", dst);
	
	memset(dst, 0, MAX_REGDOMAIN_LEN+1);
	if(FRead(dst, REG2G_EEPROM_ADDR, MAX_REGDOMAIN_LEN) < 0)
	{
		_dprintf("READ REG2G_EEPROM_ADDR ERROR\n");
	}
	else
	{
		for(i = 0; i < MAX_REGDOMAIN_LEN && dst[i] != '\0'; i++) {
			if (dst[i] == 0xff)
			{
				dst[i] = '\0';
				break;
			}
		}
	}
	sprintf(tmpStr2, "regspec_2g=%s", dst);

	memset(dst, 0, MAX_REGDOMAIN_LEN+1);
	if(FRead(dst, REG5G_EEPROM_ADDR, MAX_REGDOMAIN_LEN) < 0)
	{
		_dprintf("READ REG5G_EEPROM_ADDR ERROR\n");
	}
	else
	{
		for(i = 0; i < MAX_REGDOMAIN_LEN && dst[i] != '\0'; i++) {
			if (dst[i] == 0xff)
			{
				dst[i] = '\0';
				break;
			}
		}
	}
	sprintf(tmpStr3, "regspec_5g=%s", dst);

	if (!module_loaded("rt2860v2_ap"))
		modprobe("rt2860v2_ap");
#if defined (RTCONFIG_WLMODULE_RT3090_AP)
	if (!module_loaded("RTPCI_ap"))
	{
		modprobe("RTPCI_ap");
	}
#endif
#if defined (RTCONFIG_WLMODULE_RT3352_INIC_MII)
	if (!module_loaded("iNIC_mii"))
		modprobe("iNIC_mii", "mode=ap", "bridge=1", "miimaster=eth2", "syncmiimac=0");	// set iNIC mac address from eeprom need insmod with "syncmiimac=0"
#endif
#if defined (RTCONFIG_WLMODULE_MT7610_AP)
	if (!module_loaded("MT7610_ap"))
		//modprobe("MT7610_ap");
		modprobe("MT7610_ap", tmpStr1, tmpStr2, tmpStr3);
#endif

#if defined (RTCONFIG_WLMODULE_MT7628_AP)
	if (!module_loaded("mt_wifi_7628"))
		//modprobe("mt_wifi_7628");
		modprobe("mt_wifi_7628", tmpStr1, tmpStr2, tmpStr3);
#endif

#if defined (RTCONFIG_WLMODULE_RLT_WIFI)
	if (!module_loaded("rlt_wifi"))
	{   
		//modprobe("rlt_wifi");
		modprobe("rlt_wifi", tmpStr1, tmpStr2, tmpStr3);
	}
#endif

#if defined(RTCONFIG_MT798X)
	*vid = '\0';
#ifdef RTCONFIG_MULTILAN_CFG
	get_vlan(vid);
#endif
	if (iptv_enabled()) {
		int i, v;
		char nv[sizeof("switch_wanXtagidXXX")], tmp[sizeof("4096<XX")];

		*iptv_vids = '\0';
		for (i = 1; i <= 2; ++i) {
			snprintf(nv, sizeof(nv), "switch_wan%dtagid", i);
			v = nvram_get_int(nv);
			if (v > 1 && v < 4096) {
				snprintf(tmp, sizeof(tmp), "%s%d", (*vid == '\0')? "" : ">", v);
				strlcat(iptv_vids, tmp, sizeof(iptv_vids));
			}
		}

		if (*iptv_vids != '\0')
			strlcpy(vid, iptv_vids, sizeof(vid));
	}
	snprintf(str, sizeof(str), "vids=\"%s\"", vid);
	modprobe("mtkhnat", str);
#endif	/* RTCONFIG_MT798X */

#if defined (RTCONFIG_WLMODULE_MT7603E_AP)
#if defined(RTAC1200GA1) || defined(RTAC1200GU)
	if (!module_loaded("mt_wifi"))
		//modprobe("mt_wifi");
		modprobe("mt_wifi", tmpStr1, tmpStr2, tmpStr3);
#else
	if (!module_loaded("rlt_wifi_7603e"))
		modprobe("rlt_wifi_7603e");
#endif
#endif

#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
	if (!module_loaded("mt_wifi_7615E"))
		modprobe("mt_wifi_7615E", tmpStr1, tmpStr2, tmpStr3);
#endif

#if defined (RTCONFIG_WLMODULE_MT7663E_AP)
	system("dd if=/dev/mtdblock2 of=/lib/firmware/e2p bs=65535 skip=0 count=1");
	if (!module_loaded("mt_wifi_7663"))
		modprobe("mt_wifi_7663", tmpStr1, tmpStr2, tmpStr3);
#endif

#if defined (RTCONFIG_WLMODULE_MT7629_AP) || defined (RTCONFIG_WLMODULE_MT7622_AP) || defined (RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
#if defined (RTCONFIG_WLMODULE_MT7622_AP)
	system("dd if=/dev/mtdblock4 of=/lib/firmware/e2p bs=65536 skip=0 count=1");
#elif defined (RTCONFIG_WLMODULE_MT7915D_AP)
	system("dd if=/dev/mtdblock3 of=/lib/firmware/e2p bs=131072 skip=0 count=1");
#elif defined (RTCONFIG_MT798X) // Note: should parse INDEX[0-2]_EEPROM_size at 1profile.dat
	system("dd if=/dev/mtd3 of=/tmp/e2p bs=655360 skip=0 count=1");
#else
	system("dd if=/dev/mtdblock3 of=/lib/firmware/e2p bs=65536 skip=0 count=1");
#endif
#if !defined(RTCONFIG_MT798X)
	system("ln -sf /rom/etc/wireless/mediatek /etc/wireless/");
#endif
#if defined(RTCONFIG_WLMODULE_MT7629_AP)
	if (!module_loaded("wifi_emi_loader")){
		system("modprobe wifi_emi_loader");
		sleep(1);
	}
#endif

	if (!module_loaded("mt_wifi"))	{
		modprobe("mt_wifi", tmpStr1, tmpStr2, tmpStr3);
#if defined(RTCONFIG_MT798X)
		modprobe("mtk_warp_proxy"); // for WiFi HNAT
#endif
		sleep(1);
	}

#if defined(RTCONFIG_RALINK_MT7622)
	if (!module_loaded("mt_whnat"))	{
		modprobe("mt_whnat");
		sleep(1);
	}
#endif
#if defined (RTCONFIG_WLMODULE_MT7915D_AP)
	snprintf(cmd, sizeof(cmd), "iwpriv %s set RuntimePara_%s\n", get_wifname(0), tmpStr1);
	system(cmd);
	snprintf(cmd, sizeof(cmd), "iwpriv %s set RuntimePara_%s\n", get_wifname(0), tmpStr2);
	system(cmd);
	snprintf(cmd, sizeof(cmd), "iwpriv %s set RuntimePara_%s\n", get_wifname(0), tmpStr3);
	system(cmd);
#else
	if (!module_loaded("mt_wifi"))	{
		modprobe("mt_wifi", tmpStr1, tmpStr2, tmpStr3);
		sleep(1);
	}
#endif
#endif
	sleep(1);
}

void wl_ifdown(void)
{
	char *wl_ifnames;
	char nv[32], vif[512];
	int unit;
	char word[8], *next = NULL;
	int vidx;

#if defined(RTCONFIG_AMAS) && defined(RTCONFIG_BLINK_LED)
	if (aimesh_re_node()) {
		for (unit = 0; unit < MAX_NR_WL_IF; ++unit) {
			remove_netdev_bled_if(get_wl_led_gpio_nv(unit), get_staifname(unit));
		}
	}
#endif

#if defined(RTCONFIG_WISP)
	if (wisp_mode())
		ifconfig(get_staifname(nvram_get_int("wlc_band")), 0, NULL, NULL);
#endif

	unit = 0;
	wl_ifnames = strdup(nvram_safe_get("wl_ifnames"));
	if (wl_ifnames) {
		foreach (word, wl_ifnames, next)
		{
			ifconfig(word, 0, NULL, NULL);
#if defined(RTCONFIG_AMAS_WGN)
#if defined(RTCONFIG_RALINK) && defined(RALINK_DBDC_MODE)
			for (vidx = 1; vidx < MAX_SUBIF_NUM; vidx++) {
				snprintf(nv, sizeof(nv), "wl%d.%d_ifname", unit, vidx);
				snprintf(vif, sizeof(vif), "%s", nvram_safe_get(nv));
				if (strlen(vif))
					ifconfig(vif, 0, NULL, NULL);
			}
#endif
#endif	/* RTCONFIG_AMAS_WGN */
			unit++;
		}
		free(wl_ifnames);
	}

#if defined(RTCONFIG_RALINK) && defined(RTCONFIG_AMAS)
	for (unit = 0; unit < MAX_NR_WL_IF; unit++) {
		snprintf(nv, sizeof(nv), "wl%d_vifs", unit);
		snprintf(vif, sizeof(vif), "%s", nvram_safe_get(nv));
		if (strlen(vif))
		{
			foreach (word, vif, next)
			{	
                                ifconfig(word, 0, NULL, NULL);
			}	
		}	
	}
#endif

#if defined(RTCONFIG_RALINK)
	/* Turn off WDS interfaces. */
	for (vidx = 0; vidx < 8; ++vidx) {
		snprintf(vif, sizeof(vif), "wds%d", vidx);
		if (iface_exist(vif))
			ifconfig(vif, 0, NULL, NULL);
		snprintf(vif, sizeof(vif), "wdsx%d", vidx);
		if (iface_exist(vif))
			ifconfig(vif, 0, NULL, NULL);
	}
#endif
}

#if defined(RTCONFIG_MTK_BSD)
void force_stop_smartconnect(void)
{
       if(pids("bs20"))
        {
                doSystem("killall -9 bs20");
                unlink(BSD_LOG);
                unlink(BSD_PATH);
                unlink("/tmp/client_db.txt");
                logmessage("MTK BS20", "wifi:daemon should be stopped ");
        }
        if(pids("wapp"))
        {
                doSystem("killall -9 wapp");
                logmessage("MTK WAPP", "wifi:daemon should be stopped");
        }

}	
#endif

void fini_wl(void)
{
#if defined(RTCONFIG_MTK_BSD)
        force_stop_smartconnect();
#endif
	wl_ifdown();	/* all wireless interface down before rmmod wifi modules */

	if (module_loaded(MTK_HNAT_MOD)) {	
		unregister_hnat_wlifaces();
		modprobe_r(MTK_HNAT_MOD);
	}

#if defined (RTCONFIG_WLMODULE_MT7610_AP)
	if (module_loaded("MT7610_ap"))
		modprobe_r("MT7610_ap");
#endif

#if defined (RTCONFIG_WLMODULE_MT7628_AP)
#if !defined(RTAC1200V2)
	if (module_loaded("mt_wifi_7628"))
		modprobe_r("mt_wifi_7628");
#endif
#endif

#if defined (RTCONFIG_WLMODULE_RLT_WIFI)
	if (module_loaded("rlt_wifi"))
	{   
		modprobe_r("rlt_wifi");
#if defined(RTAC1200HP)
		//remove wifi driver, 5G wifi gpio led turn off 
		sleep(1);	
		led_onoff(1); 
#endif
	}
#endif
#if defined (RTCONFIG_WLMODULE_MT7603E_AP) || defined (RTCONFIG_WLMODULE_MT7629_AP) || defined (RTCONFIG_WLMODULE_MT7622_AP)
#if defined(RTAC1200GA1) || defined(RTAC1200GU) ||defined(RTACRH18) ||defined(RT4GAC86U)
#if defined(RTCONFIG_RALINK_MT7622)
	if (module_loaded("mt_whnat"))	{
		doSystem("rmmod mt_whnat");

	}
#endif
	if (module_loaded("mt_wifi")){
#if defined(RTCONFIG_RALINK_MT7622)
		doSystem("rmmod mt_wifi");
		sleep(1);
#else
		modprobe_r("mt_wifi");
#endif
	}



#else
	if (module_loaded("rlt_wifi_7603e"))
		modprobe_r("rlt_wifi_7603e");
#endif
#endif	/* RTCONFIG_WLMODULE_MT7603E_AP || RTCONFIG_WLMODULE_MT7629_AP || RTCONFIG_WLMODULE_MT7622_AP */

#if defined (RTCONFIG_WLMODULE_RT3352_INIC_MII)
	if (module_loaded("iNIC_mii"))
		modprobe_r("iNIC_mii");
#endif

#if defined (RTCONFIG_WLMODULE_RT3090_AP)
	if (module_loaded("RTPCI_ap"))
	{
		modprobe_r("RTPCI_ap");
	}
#endif

#if defined (RTCONFIG_WLMODULE_MT7663E_AP)
#if !defined(RTAC1200V2)
	if (module_loaded("mt_wifi_7663"))
		modprobe_r("mt_wifi_7663");
#endif
#endif

#if defined(RTCONFIG_MT798X)
	if (module_loaded("mtk_warp_proxy"))
	{
		__ctrl_hwnat(1); //force enable hwnat to avoid error when remove the hook of "mtk_warp_proxy" from "mtkhnat" module
		modprobe_r("mtk_warp_proxy");
	}
	if (module_loaded("mtk_warp"))
		modprobe_r("mtk_warp");
	if (module_loaded("mt_wifi"))
		modprobe_r("mt_wifi");
	if (is_hwnat_loaded())
		modprobe_r(MTK_HNAT_MOD);
#else
	if (module_loaded("rt2860v2_ap"))
		modprobe_r("rt2860v2_ap");
#endif
}


#if ! defined(RTCONFIG_NEW_REGULATION_DOMAIN)
static void chk_valid_country_code(char *country_code)
{
	if ((unsigned char)country_code[0]!=0xff)
	{
		//for specific power
		if     (memcmp(country_code, "Z1", 2) == 0)
			strcpy(country_code, "US");
		else if(memcmp(country_code, "Z2", 2) == 0)
			strcpy(country_code, "GB");
		else if(memcmp(country_code, "Z3", 2) == 0)
			strcpy(country_code, "TW");
		else if(memcmp(country_code, "Z4", 2) == 0)
			strcpy(country_code, "CN");
		//for normal
		if(memcmp(country_code, "BR", 2) == 0)
			strcpy(country_code, "UZ");
	}
	else
	{
		strcpy(country_code, "DB");
	}
}
#endif

#ifdef RA_SINGLE_SKU
static void create_SingleSKU(const char *path, const char *pBand, const char *reg_spec, const char *pFollow)
{
	char src[128];
	char dest[128];

	sprintf(src , "/ra_SKU/SingleSKU%s_%s%s.dat", pBand, reg_spec, pFollow);
	sprintf(dest, "%s/SingleSKU%s.dat", path, pBand);

	eval("mkdir", "-p", (char*)path);
	unlink(dest);
	eval("ln", "-s", src, dest);
}

void gen_ra_sku(const char *reg_spec)
{
#ifdef RTAC52U	// [0x40002] == 0x00 0x02
	unsigned char dst[16];
	if (!(FRead(dst, OFFSET_EEPROM_VER, 2) < 0) && dst[0] == 0x00 && dst[1] == 0x02)
	{
		create_SingleSKU("/etc/Wireless/RT2860", "", reg_spec, "_0002");
	}
	else
#endif
	create_SingleSKU("/etc/Wireless/RT2860", "", reg_spec, "");

#ifdef RTCONFIG_HAS_5G
#ifdef RTAC52U	// [0x40002] == 0x00 0x02
	if (!(FRead(dst, OFFSET_EEPROM_VER, 2) < 0) && dst[0] == 0x00 && dst[1] == 0x02)
	{
		create_SingleSKU("/etc/Wireless/iNIC", "_5G", reg_spec, "_0002");
	}
	else
#endif
	create_SingleSKU("/etc/Wireless/iNIC", "_5G", reg_spec, "");
#endif	/* RTCONFIG_HAS_5G */
}
#endif	/* RA_SINGLE_SKU */

void set_et0macaddr(char *macaddr2, char *macaddr)
{
#if defined(RTN14U) || defined(RTN11P) || defined(RTN300) || defined(RTN11P_B1) || defined(RTN800HP)// single band
	if (macaddr) {
		nvram_set("et0macaddr", macaddr);
		nvram_set("et1macaddr", macaddr);
	}
#else
#if defined(RTAC1200) || defined(RTAC1200V2) || defined(RTAC53) || defined(RTACRH18) || defined(RT4GAC86U) || defined(RTAX53U) || defined(RT4GAX56) || defined(RTAX54) ||defined(XD4S)
	if (macaddr2)
		nvram_set("et0macaddr", macaddr2);
	if (macaddr)
		nvram_set("et1macaddr", macaddr);
#else

	//TODO: separate for different chipset solution
	if (macaddr)
		nvram_set("et0macaddr", macaddr);
	if (macaddr2)
		nvram_set("et1macaddr", macaddr2);
#endif
#endif
}

#if defined(TUFAX4200) || defined(TUFAX6000) // EEPROM runtime fix
void eeprom_check(void);
void boot_version_ck(void);
#endif
void init_syspara(void)
{
	unsigned char buffer[16];
	unsigned char *dst, reg_spec[MAX_REGSPEC_LEN + 1] = { 0 }, reg_2g[MAX_REGDOMAIN_LEN + 1] = { 0 }, reg_5g[MAX_REGDOMAIN_LEN + 1] = { 0 };
	unsigned int bytes;
	int i;
	char macaddr[]="00:11:22:33:44:55";
	char macaddr2[]="00:11:22:33:44:58";
	char country_code[3];
	char pin[9];
	char productid[13];
	char fwver[8];
	char blver[20];
	unsigned char txbf_para[33];
	char ea[ETHER_ADDR_LEN] __attribute__((unused));
#ifdef RTCONFIG_ODMPID
#ifdef RTCONFIG_32BYTES_ODMPID
        char modelname[32];
#else
	char modelname[16];
#endif
#endif
#ifndef RTN56U
	const char *reg_spec_def;
#endif
#ifdef RTAC51U	/* FIX EU2CN */
	int NEED_eu2cn = 0;
#else
	const int NEED_eu2cn = 0;
#endif	/* RTAC51U */
#if defined(RTCONFIG_MT798X)
	unsigned char *p;
	unsigned char factory_var_buf[256];
	struct factory_var_s {
		char *nv_name;
		unsigned int factory_offset;
		unsigned int length;
		int (*set_func)(char *nv_name, unsigned char *buf);
	} factory_var_tbl[] = {
		{ "HwId", OFFSET_HWID, 4, NULL },
		{ "HwVer", OFFSET_HW_VERSION, 8, NULL },
		{ "HwBom", OFFSET_HW_BOM, 32, NULL },
		{ "DCode", OFFSET_HW_DATE_CODE, 8, NULL},
		{ NULL, 0, 0, NULL }
	}, *pfv;
#endif

#if defined(RTAC1200HP) || defined(RTN56UB1) || defined(RTN56UB2) || defined(RTAC1200GA1) || defined(RTAC1200GU) || defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750) || defined(RTAX53U)
#if defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750)
	char brstp;
#else
	char fixch;
#endif
	char value_str[MAX_REGSPEC_LEN+1];
	memset(value_str, 0, sizeof(value_str));
#endif

#if defined(TUFAX4200) || defined(TUFAX6000)
	boot_version_ck();
#endif

#if defined(RTCONFIG_ASUSCTRL)
	fix_location_code();
#endif

	config_hwctl_led();
#if defined(TUFAX4200) || defined(TUFAX6000) // EEPROM runtime fix
	if (nvram_get_int("no_e2pfix") == 0)
		eeprom_check();
#endif
	set_basic_fw_name();

	/* /dev/mtd/2, RF parameters, starts from 0x40000 */
	dst = buffer;
	bytes = 6;
	memset(buffer, 0, sizeof(buffer));
	memset(country_code, 0, sizeof(country_code));
	memset(pin, 0, sizeof(pin));
	memset(productid, 0, sizeof(productid));
	memset(fwver, 0, sizeof(fwver));
	memset(txbf_para, 0, sizeof(txbf_para));

#if defined(RTCONFIG_MT798X)
	for (pfv = &factory_var_tbl[0]; pfv->nv_name && pfv->length; ++pfv) {
		if (pfv->length >= sizeof(factory_var_buf))
			continue;
		*factory_var_buf = 0xFF;
		if (FRead(factory_var_buf, pfv->factory_offset, pfv->length)) {
			nvram_set(pfv->nv_name, "");
			continue;
		}

		*(factory_var_buf + pfv->length) = '\0';
		if ((p = strchr(factory_var_buf, 0xFF)) != NULL)
			*p = '\0';
		if (*factory_var_buf == '\0' || *factory_var_buf == 0xFF) {
			nvram_set(pfv->nv_name, "");
			continue;
		}

		if (pfv->set_func) {
			pfv->set_func(pfv->nv_name, factory_var_buf);
		} else {
			nvram_set(pfv->nv_name, factory_var_buf);
		}
	}
#endif

	if (FRead(dst, OFFSET_MAC_ADDR, bytes)<0)
	{
		_dprintf("READ MAC address: Out of scope\n");
	}
	else
	{
		if (buffer[0]!=0xff)
			ether_etoa(buffer, macaddr);
	}

#if !defined(RTN14U) && !defined(RTN11P) && !defined(RTN300) && !defined(RTN800HP) // single band
	if (FRead(dst, OFFSET_MAC_ADDR_2G, bytes)<0)
	{
		_dprintf("READ MAC address 2G: Out of scope\n");
	}
	else
	{
		if (buffer[0]!=0xff)
			ether_etoa(buffer, macaddr2);
	}
#endif
#ifdef RTAC51U	/* FIX EU2CN */
	_dprintf("# MAC_2G: %s\n", macaddr2);
	if(dst[0] == 0xD0 && dst[1] == 0x17 && dst[2] == 0xC2) {
		int i = 0;
		unsigned int mac_unsigned = dst[2] << 24 | dst[3] << 16 | dst[4] << 8 | dst[5];
		while(rtac51u_eu2cn_mac[i]) {
			if(rtac51u_eu2cn_mac[i] == mac_unsigned) {
				_dprintf("# NEED_eu2cn @ i(%d)\n", i);
				NEED_eu2cn = 1;
				break;
			}
			i++;
		}
	}
#endif	/* RTAC51U FIX EU2CN */

#if defined(RTAC1200HP) || defined(RTN56UB1) || defined(RTN56UB2) || defined(RTAC1200GA1) || defined(RTAC1200GU) || defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750) || defined(RTAX53U)
#if defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750)
	brstp='0';
	FRead(&brstp, OFFSET_BR_STP, 1);
	if(brstp=='1')
	{
		_dprintf("Disable br0's STP\n");
		nvram_set("lan_stp","0");
	} 
#endif

	FRead(value_str, REGSPEC_ADDR, MAX_REGSPEC_LEN);
	for(i = 0; i < MAX_REGSPEC_LEN && value_str[i] != '\0'; i++) {
		if ((unsigned char)value_str[i] == 0xff)
		{
			value_str[i] = '\0';
			break;
		}
	}
	if(!strcmp(value_str,"JP"))
	   nvram_set("JP_CS","1");
	else
	   nvram_set("JP_CS","0");
#endif
#if defined(RTN14U) || defined(RTN11P) || defined(RTN300) || defined(RTN11P_B1) || defined(RTN800HP) // single band
	if (!mssid_mac_validate(macaddr))
#else
	if (!mssid_mac_validate(macaddr) || !mssid_mac_validate(macaddr2))
#endif
		nvram_set("wl_mssid", "0");
	else
		nvram_set("wl_mssid", "1");

#if defined(RTCONFIG_MT798X)
	nvram_set("wl_mssid", "1");
#endif
#if defined(RTAC1200V2) || defined(RTACRH18) || defined(RT4GAC86U) || defined(RTAX53U) || defined(RT4GAX56) || defined(RTAX54) || defined(XD4S) || defined(RTCONFIG_MT798X)
	/* set et1macaddr the same as et0macaddr for spec. */
	strcpy(macaddr, macaddr2);
#endif
#if defined(RTCONFIG_MT798X)
	if (nvram_get_int("switch_stb_x") > 6) {
		buffer[0] |= 2; // local admin
		buffer[4] += 1; // make difference with 5G
		ether_etoa(buffer, macaddr2);
	}
#endif

	set_et0macaddr(macaddr2, macaddr);

#if !defined(RTCONFIG_MT798X)
	if (FRead(dst, OFFSET_MAC_GMAC0, bytes)<0)
		dbg("READ MAC address GMAC0: Out of scope\n");
	else
	{
		if (buffer[0]==0xff)
		{
			if (ether_atoe(macaddr, ea))
				FWrite(ea, OFFSET_MAC_GMAC0, 6);
		}
	}

	if (FRead(dst, OFFSET_MAC_GMAC2, bytes)<0)
		dbg("READ MAC address GMAC2: Out of scope\n");
	else
	{
		if (buffer[0]==0xff)
		{
			if (ether_atoe(macaddr2, ea))
				FWrite(ea, OFFSET_MAC_GMAC2, 6);
		}
	}
#endif

#ifdef RTCONFIG_ODMPID
#ifdef RTCONFIG_32BYTES_ODMPID
        FRead(modelname, OFFSET_32BYTES_ODMPID, 32);
        modelname[31] = '\0';
	if (modelname[0] != 0 && (unsigned char)(modelname[0]) != 0xff && is_valid_hostname(modelname) && strcmp(modelname, "ASUS")) {
                nvram_set("odmpid", modelname);
        } else
#endif
        {

		FRead(modelname, OFFSET_ODMPID, sizeof(modelname));
		modelname[sizeof(modelname)-1] = '\0';
		if (modelname[0] != 0 && (unsigned char)(modelname[0]) != 0xff && is_valid_hostname(modelname) && strcmp(modelname, "ASUS")) {
#if defined(RTN11P)  || defined(RTN300)
			if(strcmp(modelname, "RT-N12E_B")==0)
				nvram_set("odmpid", "RT-N12E_B1");
			else
#endif	/* RTN11P */
				nvram_set("odmpid", modelname);
		
                } else
                        nvram_unset("odmpid");
        }
#else
        nvram_unset("odmpid");
#endif

	if (FRead(reg_spec, REGSPEC_ADDR, MAX_REGSPEC_LEN) < 0)
		*reg_spec = '\0';
	if (FRead(reg_2g, REG2G_EEPROM_ADDR, MAX_REGDOMAIN_LEN) < 0)
		*reg_2g = '\0';
	if (FRead(reg_5g, REG5G_EEPROM_ADDR, MAX_REGDOMAIN_LEN) < 0)
		*reg_5g = '\0';
	trim_char(reg_spec, 0xFF);
	trim_char(reg_2g, 0xFF);
	trim_char(reg_5g, 0xFF);

#if defined(RTCONFIG_TCODE)
	/* Territory code */
	memset(buffer, 0, sizeof(buffer));
	if(NEED_eu2cn) {
		nvram_set("territory_code", "CN/01");	/* RT-AC51U: FIX EU2CN */
	} else {
		if (FRead(buffer, OFFSET_TERRITORY_CODE, 5) < 0) {
			_dprintf("READ ASUS territory code: Out of scope\n");
			nvram_unset("territory_code");
		} else {
			/* [A-Z][A-Z]/[0-9][0-9] */
			if (buffer[2] != '/' ||
			    !isupper(buffer[0]) || !isupper(buffer[1]) ||
			    !isdigit(buffer[3]) || !isdigit(buffer[4]))
			{
				nvram_unset("territory_code");
			} else {
				nvram_set("territory_code", buffer);
			}
		}
	}

#if defined(RTCONFIG_ASUSCTRL)
	nvram_unset("ctl_reg_spec");
	nvram_unset("ctl_wl_reg_2g");
	nvram_unset("ctl_wl_reg_5g");

	asus_ctrl_sku_check();

	if (*nvram_safe_get("ctl_reg_spec") != '\0')
		strlcpy(reg_spec, nvram_safe_get("ctl_reg_spec"), sizeof(reg_spec));
	if (*nvram_safe_get("ctl_wl_reg_2g") != '\0')
		strlcpy(reg_2g, nvram_safe_get("ctl_wl_reg_2g"), sizeof(reg_2g));
	if (*nvram_safe_get("ctl_wl_reg_5g") != '\0')
		strlcpy(reg_5g, nvram_safe_get("ctl_wl_reg_5g"), sizeof(reg_5g));
#endif
#endif	/* RTCONFIG_TCODE */

	/* reserved for Ralink. used as ASUS country code. */
#if !defined(RTCONFIG_NEW_REGULATION_DOMAIN)
	dst = (unsigned char*) country_code;
	bytes = 2;
	if (FRead(dst, OFFSET_COUNTRY_CODE, bytes)<0)
	{
		_dprintf("READ ASUS country code: Out of scope\n");
		nvram_set("wl_country_code", "");
	}
	else
	{
		chk_valid_country_code(country_code);
		nvram_set("wl_country_code", country_code);
		nvram_set("wl0_country_code", country_code);
#ifdef RTCONFIG_HAS_5G
		nvram_set("wl1_country_code", country_code);
#endif
	}
#if defined(RTN14U) // for CE Adaptivity
	if ((strcmp(country_code, "DE") == 0) || (strcmp(country_code, "EU") == 0))
		nvram_set("reg_spec", "CE");
	else
		nvram_set("reg_spec", "NDF");
#endif

#else	/* ! RTCONFIG_NEW_REGULATION_DOMAIN */

#if defined(RTAC51U) || defined(RTAC51UP) || defined(RTAC53) || defined(RTN11P) || defined(RTCONFIG_MT798X)
	reg_spec_def = "CE";
#else
	reg_spec_def = "FCC";
#endif

	if (NEED_eu2cn) {
		nvram_set("reg_spec", "CN");	/* RT-AC51U: FIX EU2CN */
	} else {
		nvram_set("reg_spec", (*reg_spec)? (char*) reg_spec : reg_spec_def);
	}

	if (NEED_eu2cn) {
		nvram_set("wl0_country_code", "CN");	/* RT-AC51U: FIX EU2CN */
	} else {
		if (*reg_2g == '\0' || memcmp(reg_2g, "2G_CH", 5) != 0) {
			_dprintf("Read REG2G_EEPROM_ADDR fail or invalid value\n");
			nvram_set("wl_country_code", "");
			nvram_set("wl0_country_code", "DB");
			nvram_set("wl_reg_2g", "2G_CH14");
		} else {
			nvram_set("wl_reg_2g", reg_2g);
			if (strcmp(reg_2g, "2G_CH11") == 0)
				nvram_set("wl0_country_code", "US");
			else if (strcmp(reg_2g, "2G_CH13") == 0) {
#if defined(RTAC1200V2)
				if(nvram_match("reg_spec","EAC"))
					nvram_set("wl0_country_code", "RU");
				else
#endif
					nvram_set("wl0_country_code", "GB");
			}
			else if (strcmp(reg_2g, "2G_CH13") == 0) {
				nvram_set("wl0_country_code", "GB");
			}
			else if (strcmp(reg_2g, "2G_CH14") == 0)
				nvram_set("wl0_country_code", "DB");
			else
				nvram_set("wl0_country_code", "DB");
		}
	}

#ifdef RTCONFIG_HAS_5G
	if (NEED_eu2cn) {
		nvram_set("wl1_country_code", "US");	/* RT-AC51U: FIX EU2CN */
	} else {
		if (*reg_5g == '\0' || memcmp(reg_5g, "5G_", 3) != 0) {
			_dprintf("Read REG5G_EEPROM_ADDR fail or invalid value\n");
			nvram_set("wl_country_code", "");
			nvram_set("wl1_country_code", "DB");
			nvram_set("wl_reg_5g", "5G_ALL");
		} else {
			nvram_set("wl_reg_5g", reg_5g);
			nvram_set("wl1_IEEE80211H", "0");
			if (strcmp(reg_5g, "5G_BAND1") == 0)
				nvram_set("wl1_country_code", "GB");
#if defined(RTCONFIG_MT798X)
			else if (strcmp(reg_5g, "5G_ALL") == 0) {
				char tcode[6] = { 0 };

				snprintf(tcode, sizeof(tcode), "%s", nvram_safe_get("territory_code"));
				tcode[2] = '\0';

				if (strlen(tcode) && strncmp(tcode, "DB", 2)) {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", tcode);
				}
				else {
					nvram_set("wl1_country_code", "DB");
				}
			}
			else if (strcmp(reg_5g, "5G_BAND123") == 0) {
				if(strncmp(nvram_safe_get("territory_code"), "JP/01", 2) == 0) {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "JP");
				}
				else {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "EU");
				}
			}
			else if (strcmp(reg_5g, "5G_BAND124") == 0) {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "CN");
			}
			else if (strcmp(reg_5g, "5G_BAND3") == 0) {
					nvram_set("wl1_IEEE80211H", "1");
					nvram_set("wl1_country_code", "EH");
			}
#endif
			else if (strcmp(reg_5g, "5G_BAND123") == 0) {
				nvram_set("wl1_country_code", "GB");
#ifdef RTCONFIG_RALINK_DFS
				nvram_set("wl1_IEEE80211H", "1");
#endif	/* RTCONFIG_RALINK_DFS */
			}
			else if (strcmp(reg_5g, "5G_BAND14") == 0)
				nvram_set("wl1_country_code", "US");
			else if (strcmp(reg_5g, "5G_BAND24") == 0)
				nvram_set("wl1_country_code", "TW");
			else if (strcmp(reg_5g, "5G_BAND4") == 0)
				nvram_set("wl1_country_code", "CN");
			else if (strcmp(reg_5g, "5G_BAND124") == 0)
				nvram_set("wl1_country_code", "IN");
			else if (strcmp(reg_5g, "5G_BAND12") == 0)	{
				nvram_set("wl1_country_code", "IL");
#ifdef RTCONFIG_RALINK_DFS
				nvram_set("wl1_IEEE80211H", "1");
#endif	/* RTCONFIG_RALINK_DFS */
			}
			else if (strcmp(reg_5g, "5G_ALL") == 0)	{
#if defined(RTAC1200V2) || defined(RTAC85P)
				if(nvram_match("reg_spec","EAC"))
					nvram_set("wl1_country_code", "RU");
				else
#elif defined(RT4GAX56)
					nvram_set("wl1_country_code", "AA");
#else
					nvram_set("wl1_country_code", "DB");
#endif
#ifdef RTCONFIG_RALINK_DFS
				nvram_set("wl1_IEEE80211H", "1");
#endif	/* RTCONFIG_RALINK_DFS */
			}
			else
				nvram_set("wl1_country_code", "DB");
		}
	}
#endif	/* RTCONFIG_HAS_5G */
#endif	/* ! RTCONFIG_NEW_REGULATION_DOMAIN */

#if defined(RTN56U) || defined(RTCONFIG_DSL)
	if (nvram_match("wl_country_code", "BR")) {
		nvram_set("wl_country_code", "UZ");
		nvram_set("wl0_country_code", "UZ");
#ifdef RTCONFIG_HAS_5G
		nvram_set("wl1_country_code", "UZ");
#endif	/* RTCONFIG_HAS_5G */
	}
#endif
	if (nvram_match("wl_country_code", "HK") && nvram_match("preferred_lang", ""))
		nvram_set("preferred_lang", "TW");

	/* reserved for Ralink. used as ASUS pin code. */
	dst = (char*)pin;
	bytes = 8;
	if (FRead(dst, OFFSET_PIN_CODE, bytes)<0)
	{
		_dprintf("READ ASUS pin code: Out of scope\n");
		nvram_set("wl_pin_code", "");
	}
	else
	{
		if (((unsigned char)pin[0] == 0xff)
		 || !strcmp(pin, "12345678")) {
			char devPwd[9];
			nvram_set("secret_code", wps_gen_pin(devPwd, sizeof(devPwd)) ? devPwd : "12345670");
		}
		else
			nvram_set("secret_code", pin);
	}

	dst = buffer;
	bytes = 16;
	if (linuxRead(dst, 0x20, bytes)<0)	/* The "linux" MTD partition, offset 0x20. */
	{
		fprintf(stderr, "READ firmware header: Out of scope\n");
		nvram_set("productid", "unknown");
		nvram_set("firmver", "unknown");
	}
	else
#if defined(RTCONFIG_MT798X)
	{
		strlcpy(productid, rt_buildname, sizeof(productid));
		nvram_set("productid", trim_r(productid));
		nvram_set("firmver", rt_version);
	}
#else
	{
		strncpy(productid, buffer + 4, 12);
		productid[12] = 0;
		sprintf(fwver, "%d.%d.%d.%d", buffer[0], buffer[1], buffer[2], buffer[3]);
		nvram_set("productid", trim_r(productid));
		nvram_set("firmver", trim_r(fwver));
	}
#endif

#if defined(RTCONFIG_TCODE)
#if defined(RTN56UB1)  
	if((nvram_match("territory_code","EU/01")|| nvram_match("territory_code","UK/01"))&& !nvram_match("wl1_IEEE80211H","1"))
	{
#ifdef RTCONFIG_RALINK_DFS
			nvram_set("wl1_IEEE80211H", "1");
#endif	/* RTCONFIG_RALINK_DFS */
	}
#endif

#if defined(RTN11P)
	if (nvram_match("odmpid", "RT-N12+") && nvram_match("reg_spec", "CN"))
	{
		char *str;
		str = nvram_get("territory_code");
		if(str == NULL || str[0] == '\0') {
			nvram_set("territory_code", "CN/01");
		}
	}
#endif	/* RTN11P */
	
	/* PSK */
        memset(buffer, 0, sizeof(buffer));
	if (FRead(buffer, OFFSET_PSK, 14) < 0) {
	_dprintf("READ ASUS PSK: Out of scope\n");
		nvram_set("wifi_psk", "");
	 } else {
		if (buffer[0] == 0xff)
			nvram_set("wifi_psk", "");
		else
		{
			for(i = 0; i < 14 && buffer[i] != '\0'; i++) {
				if ((unsigned char)buffer[i] == 0xff)
				{
					buffer[i] = '\0';
					break;
				}
			}
			nvram_set("wifi_psk", buffer);
		}
	}
#endif /* RTCONFIG_TCODE */

	memset(buffer, 0, sizeof(buffer));
	FRead(buffer, OFFSET_BOOT_VER, 4);
//	sprintf(blver, "%c.%c.%c.%c", buffer[0], buffer[1], buffer[2], buffer[3]);
	sprintf(blver, "%s-0%c-0%c-0%c-0%c", trim_r(productid), buffer[0], buffer[1], buffer[2], buffer[3]);
	nvram_set("blver", trim_r(blver));

	_dprintf("mtd productid: %s\n", nvram_safe_get("productid"));
	_dprintf("bootloader version: %s\n", nvram_safe_get("blver"));
	_dprintf("firmware version: %s\n", nvram_safe_get("firmver"));

#if !defined (RTCONFIG_WLMODULE_MT7615E_AP)
	dst = txbf_para;
	int count_0xff = 0;
	if (FRead(dst, OFFSET_TXBF_PARA, 33) < 0)
	{
		fprintf(stderr, "READ TXBF PARA address: Out of scope\n");
	}
	else
	{
		for (i = 0; i < 33; i++)
		{
			if (txbf_para[i] == 0xff)
				count_0xff++;
/*
			if ((i % 16) == 0) fprintf(stderr, "\n");
			fprintf(stderr, "%02x ", (unsigned char) txbf_para[i]);
*/
		}
/*
		fprintf(stderr, "\n");

		fprintf(stderr, "TxBF parameter 0xFF count: %d\n", count_0xff);
*/
	}

	if (count_0xff == 33)
		nvram_set("wl1_txbf_en", "0");
	else
		nvram_set("wl1_txbf_en", "1");
#endif

#if defined (RTCONFIG_WLMODULE_RT3352_INIC_MII)
#define EEPROM_INIC_SIZE (512)
#define EEPROM_INIT_ADDR 0x48000
#define EEPROM_INIT_FILE "/etc/Wireless/iNIC/iNIC_e2p.bin"
	{
		char eeprom[EEPROM_INIC_SIZE];
		if(FRead(eeprom, EEPROM_INIT_ADDR, sizeof(eeprom)) < 0)
		{
			fprintf(stderr, "FRead(eeprom, 0x%08x, 0x%x) failed\n", EEPROM_INIT_ADDR, sizeof(eeprom));
		}
		else
		{
			FILE *fp;
			char *filepath = EEPROM_INIT_FILE;

			system("mkdir -p /etc/Wireless/iNIC/");
			if((fp = fopen(filepath, "w")) == NULL)
			{
				fprintf(stderr, "fopen(%s) failed!!\n", filepath);
			}
			else
			{
				if(fwrite(eeprom, sizeof(eeprom), 1, fp) < 1)
				{
					perror("fwrite(eeprom)");
				}
				fclose(fp);
			}
		}
	}
#endif
	{
		char ipaddr_lan[16];
		FRead(ipaddr_lan, OFFSET_IPADDR_LAN, sizeof(ipaddr_lan));
		ipaddr_lan[sizeof(ipaddr_lan)-1] = '\0';
		if((unsigned char)(ipaddr_lan[0]) != 0xff)
		{
			nvram_set("IpAddr_Lan", ipaddr_lan);
		} else {
			nvram_unset("IpAddr_Lan");
		}
	}

#ifdef RA_SINGLE_SKU
#if defined(RTAC52U) || defined(RTAC51U) || defined(RTN11P) || defined(RTN300) || defined(RTN54U) || defined(RTAC1200HP) || defined(RTN56UB1) || defined(RTAC54U) || defined(RTN56UB2)
	gen_ra_sku(nvram_safe_get("reg_spec"));
#endif	/* RTAC52U && RTAC51U && RTN54U && RTAC54U && RTAC1200HP && RTN56UB1 && RTN56UB1 && RTN11P && RTN300 */
#endif	/* RA_SINGLE_SKU */

#ifdef RTCONFIG_AMAS
#ifdef RTCONFIG_PRELINK
	char bdl;
	char bdlkey_buf[CFGSYNC_GROUPID_LEN+1];
	if (FRead(&bdl, OFFSET_AMAS_BUNDLE_FLAG, 1) < 0) {
		_dprintf("READ AiMesh bundle flag: Out of scope\n");
	} else {
		if ((bdl > AB_FLAG_NONE) && (bdl < AB_FLAG_MAX))
			nvram_set_int("amas_bdl", bdl);
		else
			nvram_unset("amas_bdl");
	}
	if (FRead(bdlkey_buf, OFFSET_AMAS_BUNDLE_KEY, CFGSYNC_GROUPID_LEN) < 0) {
		_dprintf("READ AiMesh bundle key: Out of scope\n");
	} else {
		if (!is_valid_group_id(bdlkey_buf)) {
			bdlkey_buf[0]='\0';
			nvram_unset("amas_bdlkey");
		}
		else {
			bdlkey_buf[CFGSYNC_GROUPID_LEN]='\0';
			nvram_set("amas_bdlkey", bdlkey_buf);
		}
	}
#endif
#endif

#if defined(RTCONFIG_COBRAND)
        unsigned char color=0xff;
        if (FRead(&color, OFFSET_HW_COBRAND, 1)<0)
        {
                _dprintf("Read COBRAND value fail\n");
                nvram_set("CoBrand", "");
        }
        else
        {
                if(color!=0xff)
                        nvram_set_int("CoBrand",color);
                else
                        nvram_unset("CoBrand");
        }
#endif

	nvram_set("firmver", rt_version);
	nvram_set("productid", rt_buildname);

	_dprintf("odmpid: %s\n", nvram_safe_get("odmpid"));
	_dprintf("current FW productid: %s\n", nvram_safe_get("productid"));
	_dprintf("current FW firmver: %s\n", nvram_safe_get("firmver"));

	getSN();
}

#ifdef RTCONFIG_ATEUSB3_FORCE
void post_syspara(void)
{
	unsigned char buffer[16];
	buffer[0]='0';
	if (FRead(&buffer[0], OFFSET_FORCE_USB3, 1) < 0) {
		fprintf(stderr, "READ FORCE_USB3 address: Out of scope\n");
	}
	if (buffer[0]=='1')
		nvram_set("usb_usb3", "1");
}
#endif

void generate_wl_para(int unit, int subunit)
{
}

#if defined(RTAC52U) || defined(RTAC51U) || defined(RTN54U) || defined(RTAC1200HP) || defined(RTN56UB1) || defined(RTN56UB2)  || defined(RTAC54U) || defined(RTAC1200GA1)  || defined(RTAC1200GU) || defined(RTAC53) || defined(RTAC85U) || defined(RTN800HP) || defined(RTACRH26)
#define HW_NAT_WIFI_OFFLOADING		(0xFF00)
#define HW_NAT_DEVNAME			"hwnat0"
static void adjust_hwnat_wifi_offloading(void)
{
	int enable_hwnat_wifi = 1, fd;

	if (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")) {
		nvram_unset("isp_profile_hwnat_not_safe");
		eval("rtkswitch", "50");
		if (nvram_get_int("isp_profile_hwnat_not_safe") == 1)
			enable_hwnat_wifi = 0;
	}
	
#if defined (RTCONFIG_WLMODULE_MT7615E_AP) && !defined(RTCONFIG_MTK_8021XD3000)
		if (get_ipv6_service() == IPV6_PASSTHROUGH)
				enable_hwnat_wifi = 0;
		doSystem("iwpriv ra0 set wifi_hwnat=%d", enable_hwnat_wifi);	
#else

	if ((fd = open("/dev/" HW_NAT_DEVNAME, O_RDONLY)) < 0) {
		_dprintf("Open /dev/%s fail. errno %d (%s)\n", HW_NAT_DEVNAME, errno, strerror(errno));
		return;
	}

	_dprintf("hwnat_wifi = %d\n", enable_hwnat_wifi);
	if (ioctl(fd, HW_NAT_WIFI_OFFLOADING, &enable_hwnat_wifi) < 0)
		_dprintf("ioctl error. errno %d (%s)\n", errno, strerror(errno));

	close(fd);
#endif
}
#else
static inline void adjust_hwnat_wifi_offloading(void) { }
#endif

// only ralink solution can reload it dynamically
// only happened when hwnat=1
// only loaded when unloaded, and unloaded when loaded
// in restart_firewall for fw_pt_l2tp/fw_pt_ipsec
// in restart_qos for qos_enable
// in restart_wireless for wlx_mrate_x, etc
void reinit_hwnat(int unit)
{
	int prim_unit = wan_primary_ifunit();
	int act = 1;	/* -1/0/otherwise: ignore/remove hwnat/load hwnat */
	int nat_x = -1;
	char tmp[32], prefix[] = "wanXXX_";
#if defined(RTCONFIG_DUALWAN) && !defined(RTCONFIG_MT798X)
	int i, l, t, link_wan = 1, link_wans_lan = 1;
	int wans_cap = get_wans_dualwan() & WANSCAP_WAN;
	int wanslan_cap = get_wans_dualwan() & WANSCAP_LAN;
#endif

	if (!nvram_get_int("hwnat"))
	{
		disable_hwnat();
		return;
	}

#if defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750)
	if(!is_wan_connect(prim_unit))
		return;
#endif

#if defined(RTCONFIG_MT798X) && defined(RTCONFIG_BWDPI)
	/* disable at default */
	f_write_string("/proc/sys/net/netfilter/nf_conntrack_acct", "0", 0, 0);
	f_write_string("/sys/kernel/debug/hnat/dpi_using", "0", 0, 0);
#endif
	/* If QoS is enabled, disable hwnat. */
	if (nvram_get_int("qos_enable") == 1)
		act = 0;
#if defined(RTCONFIG_MT798X) && defined(RTCONFIG_BWDPI)
	/* MTK798X pure software for DPI */
	else if (check_bwdpi_nvram_setting() == 1) {
		f_write_string("/proc/sys/net/netfilter/nf_conntrack_acct", "1", 0, 0);
		f_write_string("/sys/kernel/debug/hnat/dpi_using", "1", 0, 0);
	}
#endif

#if defined(RTCONFIG_MT798X)
#if defined(RTCONFIG_IPSEC)
	/* If IPSec VPN is enabled, disable hwnat. */
	if (act > 0 && (nvram_get_int("ipsec_server_enable") == 1 || nvram_get_int("ipsec_client_enable") == 1))
		act = 0;
#endif
	if (act > 0) {
		snprintf(prefix, sizeof(prefix), "wan%d_", prim_unit);
		nat_x = nvram_get_int(strcat_r(prefix, "nat_x", tmp));

#if defined(RTCONFIG_DUALWAN)
		/* If secorand WAN up, ignore... */
		if (unit != -1 && unit != prim_unit)
			act = -1;
		/* Load Balance */
		else if (nvram_match("wans_mode", "lb") && (nvram_match("wan0_proto", "pptp")
							 || nvram_match("wan0_proto", "l2tp")
							 || nvram_match("wan1_proto", "pptp")
							 || nvram_match("wan1_proto", "l2tp")))
			act = 0;
		else
#endif
		/* Enable NAT */
		if (!nat_x)
			act = 0;
		/* Single WAN/Dual WAN Fail Over */
		else if (nvram_match(strcat_r(prefix, "proto", tmp), "pptp")
		      || nvram_match(strcat_r(prefix, "proto", tmp), "l2tp"))
			act = 0;
		/* If VPN server(PPTP) is enabled, disable hwnat. */
		else if (nvram_get_int("pptpd_enable"))
			act = 0;
		/* If VPN client(PPTP/L2TP) activated, disable hwnat. */
		else if (!nvram_match("vpnc_clientlist", "")) {
			char *nv = NULL, *nvp = NULL, *b = NULL;
			char *desc, *proto, *server, *username, *passwd, *active;

			nv = nvp = strdup(nvram_safe_get("vpnc_clientlist"));
			while (nv && (b = strsep(&nvp, "<")) != NULL) {
				if (vstrsep(b, ">", &desc, &proto, &server, &username, &passwd, &active) < 6)
					continue;
				if (atoi(active) == 1 && (!strcmp(proto, "PPTP")
						       || !strcmp(proto, "L2TP"))) {
					act = 0;
					break;
				}
			}
			free(nv);
		}
	}
#else /* RTCONFIG_MT798X */

#if defined(RTCONFIG_IPSEC) && defined(RTCONFIG_RALINK_MT7621)
	/* If IPSec VPN is enabled, disable hwnat. */
	if (nvram_get_int("ipsec_server_enable") == 1 || nvram_get_int("ipsec_client_enable") == 1)
		act = 0;
#endif

#if defined(RTN14U) || defined(RTAC52U) || defined(RTAC51U) || defined(RTN11P) || defined(RTN300) || defined(RTN54U) || defined(RTAC1200HP) || defined(RTN56UB1) || defined(RTAC54U) || defined(RTN56UB2) || defined(RTAC1200GA1)  || defined(RTAC1200GU) || defined(RTAC53) || defined(RTAC85U) || defined(RTN800HP) || defined(RTACRH26) || defined(RT4GAC86U)
	if (act > 0 && !nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", ""))
		act = 0;
#endif

	if (act > 0) {
#if defined(RTCONFIG_DUALWAN)
		if (unit < 0 || unit > WAN_UNIT_SECOND) {
			if ((wans_cap && wanslan_cap) ||
			    (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
			   )
				act = 0;
		} else {
			snprintf(prefix, sizeof(prefix), "wan%d_", unit);
			nat_x = nvram_get_int(strcat_r(prefix, "nat_x", tmp));

			if (unit == prim_unit && !nat_x)
				act = 0;
			else if ((wans_cap && wanslan_cap) ||
				 (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")))
				)
				act = 0;
			else if (unit != prim_unit)
				act = -1;
		}
#else
		nat_x = is_nat_enabled();
		if (!nat_x)
			act = 0;
#endif
	}

#if defined(RTN65U) || defined(RTN56U) || defined(RTN14U) || defined(RTAC52U) || defined(RTAC51U) || defined(RTN11P) || defined(RTN300) || defined(RTN54U) || defined(RTAC1200HP) || defined(RTN56UB1) || defined(RTAC54U) || defined(RTN56UB2) || defined(RTAC1200GA1) || defined(RTAC1200GU) || defined(RTAC51UP) || defined(RTAC53) || defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750)
	if (act > 0) {
#if defined(RTCONFIG_DUALWAN)
		if (unit < 0 || unit > WAN_UNIT_SECOND || nvram_match("wans_mode", "lb")) {
			if (get_wans_dualwan() & WANSCAP_USB)
				act = 0;
		} else {
			if (unit == prim_unit && get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_USB)
				act = 0;
		}
#else
		if (dualwan_unit__usbif(prim_unit))
			act = 0;
#endif
	}
#endif
#endif /* RTCONFIG_MT798X */

#if defined(RTCONFIG_DUALWAN) && !defined(RTCONFIG_RALINK_MT7622) && !defined(RTCONFIG_MT798X)
	if (act != 0 &&
	    ((wans_cap && wanslan_cap) || (wanslan_cap && (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", ""))))
	   )
	{
		/* If WANS_LAN and WAN is enabled, WANS_LAN is link-up and WAN is not link-up, hw_nat MUST be removed.
		 * If hw_nat exists in such scenario, LAN PC can't connect to Internet through WANS_LAN correctly.
		 *
		 * FIXME:
		 * If generic IPTV feature is enabled, STB port and VoIP port are recognized as WAN port(s).
		 * In such case, we don't know whether real WAN port is link-up/down.
		 * Thus, if WAN is link-up and primary unit is not WAN, assume WAN is link-down.
		 */
		for (i = WAN_UNIT_FIRST; i < WAN_UNIT_MAX; ++i) {
			if ((t = get_dualwan_by_unit(i)) == WANS_DUALWAN_IF_USB)
				continue;

			l = wanport_status(i);
			switch (t) {
			case WANS_DUALWAN_IF_WAN:
				link_wan = l && (i == prim_unit);
				break;
			case WANS_DUALWAN_IF_DSL:
				link_wan = l;
				break;
			case WANS_DUALWAN_IF_LAN:
				link_wans_lan = l;
				break;
			default:
				_dprintf("%s: Unknown WAN type %d\n", __func__, t);
			}
		}

		if (!link_wan && link_wans_lan)
			act = 0;
	}

	_dprintf("%s:DUALWAN: unit %d,%d type %d iptv [%s] nat_x %d qos %d wans_mode %s link %d,%d: action %d.\n",
		__func__, unit, prim_unit, get_dualwan_by_unit(unit), nvram_safe_get("switch_wantag"), nat_x,
		nvram_get_int("qos_enable"), nvram_safe_get("wans_mode"),
		link_wan, link_wans_lan, act);
#else
	_dprintf("%s:WAN: unit %d,%d type %d nat_x %d qos %d: action %d.\n",
		__func__, unit, prim_unit, get_dualwan_by_unit(unit),
		nat_x, nvram_get_int("qos_enable"), act);
#endif

	if (act < 0)
		return;

	switch (act) {
	case 0:		/* remove hwnat */
		if (module_loaded(MTK_HNAT_MOD))
		{
#if defined (RTCONFIG_WLMODULE_MT7615E_AP) && !defined(RTCONFIG_RALINK_MT7622)
			doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(0), 0);
#ifdef RTCONFIG_HAS_5G
			doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(1), 0);
#endif
#endif

#if defined(RTCONFIG_RALINK_MT7622)  || defined(RTCONFIG_RALINK_MT7629) || (defined(RTCONFIG_RALINK_MT7621) && defined (RTCONFIG_WLMODULE_MT7915D_AP)) || defined(RTCONFIG_MT798X)
			doSystem("echo 0 > /sys/kernel/debug/hnat/hook_toggle");
#else
			modprobe_r(MTK_HNAT_MOD);
#endif
			sleep(1);
		}
		break;
	default:	/* load hwnat */
#if defined(RTCONFIG_RALINK_MT7622) || defined(RTCONFIG_MT798X)
		doSystem("echo 1 > /sys/kernel/debug/hnat/hook_toggle");
#endif
		if (!module_loaded(MTK_HNAT_MOD))
		{
			modprobe(MTK_HNAT_MOD);
			sleep(1);
		}
#if defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_MT7622) || defined(RTCONFIG_MT798X) && defined(RTCONFIG_SOFTWIRE46)
		switch (get_ipv4_service()) {
		case WAN_MAPE:
		case WAN_V6PLUS:
		case WAN_OCNVC:
			doSystem("echo %d > /sys/kernel/debug/hnat/mape_toggle", 1);
			break;
		}
#endif
#if defined (RTCONFIG_WLMODULE_MT7615E_AP) && !defined(RTCONFIG_RALINK_MT7622)
		doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(0), 1);
#ifdef RTCONFIG_HAS_5G
		doSystem("iwpriv %s set hw_nat_register=%d", get_wifname(1), 1);
#endif
#endif
#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
		doSystem("iwpriv %s set LanNatSpeedUpEn=%d", get_wifname(0), 1);
#ifdef RTCONFIG_HAS_5G
		doSystem("iwpriv %s set LanNatSpeedUpEn=%d", get_wifname(1), 1);
#endif
#endif		
		adjust_hwnat_wifi_offloading();
	
	}
}

int
wl_exist(char *ifname, int band)
{
	int ret = 0;
	ret = eval("iwpriv", ifname, "stat");
	_dprintf("eval(iwpriv, %s, stat) ret(%d)\n", ifname, ret);
	return !ret;
}
#ifdef RTCONFIG_SWCONFIG
void set_wan_tag(char *interface)
{
	int model, wan_vid, iptv_vid, voip_vid, switch_stb;
	char wan_dev[10], port_id[7], vid_dev[10];
	
	model = get_model();
	wan_vid = nvram_get_int("switch_wan0tagid");
	iptv_vid = nvram_get_int("switch_wan1tagid");
	voip_vid = nvram_get_int("switch_wan2tagid");

	switch_stb = nvram_get_int("switch_stb_x");

	eval("vconfig", "set_name_type", "VLAN_PLUS_VID_NO_PAD");
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				/* Handle wan(IPTV) vlan traffic */
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vid_dev, "vlan%d", iptv_vid);
				eval("ifconfig", vid_dev, "up");
				nvram_set("wan10_ifname", vid_dev);
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				/* Handle wan(VOIP) vlan traffic */
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vid_dev, "vlan%d", voip_vid);
				eval("ifconfig", vid_dev, "up");
				nvram_set("wan11_ifname", vid_dev);
			}
		}
#endif

}
#else	/* !RTCONFIG_SWCONFIG */
void
set_wan_tag(char *interface)
{
	int model, wan_vid; //, iptv_vid, voip_vid, wan_prio, iptv_prio, voip_prio;
	char wan_dev[10], port_id[7];

	model = get_model();
	wan_vid = nvram_get_int("switch_wan0tagid") & 0x0fff;

	snprintf(wan_dev, sizeof(wan_dev), "vlan%d", wan_vid);

	switch(model) {
#if defined(RTCONFIG_RALINK_MT7628)
	default:
#else
	case MODEL_RTAC1200HP:
	case MODEL_RTAC51U:
	case MODEL_RTAC51UP:
	case MODEL_RT4GAC86U:
	case MODEL_RTAX53U:
	case MODEL_XD4S:
	case MODEL_RTAX54:
	case MODEL_RT4GAX56:
	case MODEL_RTAC53:
	case MODEL_RTAC52U:
	case MODEL_RTAC54U:
	case MODEL_RTN11P:
	case MODEL_RTN14U:
	case MODEL_RTN54U:
	case MODEL_RTAC1200GA1:
	case MODEL_RTAC1200GU:
	case MODEL_RTN56UB1:
	case MODEL_RTN56UB2:
	case MODEL_RTAC85U:
	case MODEL_RTAC85P:
	case MODEL_RTACRH26:
	case MODEL_RTN800HP:
	case MODEL_TUFAC1750:
#endif
		ifconfig(interface, IFUP, 0, 0);
		if(wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);

			/* Set Wan port PRIO */
			if (nvram_get_int("switch_wan0prio") != 0)
				eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
		}
		break;
	}

#ifdef RTCONFIG_MULTICAST_IPTV
	{
		int iptv_vid, voip_vid, iptv_prio, voip_prio, switch_stb;
		int mang_vid, mang_prio;
		char iptv_prio_str[4] = "4";
		char voip_prio_str[4], mang_prio_str[4];

		iptv_vid  = nvram_get_int("switch_wan1tagid") & 0x0fff;
		voip_vid  = nvram_get_int("switch_wan2tagid") & 0x0fff;
		iptv_prio = nvram_get_int("switch_wan1prio") & 0x7;
		voip_prio = nvram_get_int("switch_wan2prio") & 0x7;
		mang_vid  = nvram_get_int("switch_wan3tagid") & 0x0fff;
		mang_prio = nvram_get_int("switch_wan3prio") & 0x7;

		switch_stb = nvram_get_int("switch_stb_x");
		if (switch_stb >= 7) {
			system("rtkswitch 40 1");			/* admin all frames on all ports */
#if defined(RTN56U) || defined(RTN65U)
			/* Make sure admin all frames on all ports is applied to Realtek switch. */
			system("rtkswitch 38 0");
#endif
			if(wan_vid) { /* config wan port */
				__setup_vlan(wan_vid, 0, 0x00000210);	/* config WAN & WAN_MAC port */
			}

			if (iptv_vid) { /* config IPTV on wan port */
				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);

				__setup_vlan(iptv_vid, iptv_prio, 0x00000210);	/* config WAN & WAN_MAC port */

				if (iptv_prio) { /* config priority */
					snprintf(iptv_prio_str, sizeof(iptv_prio_str), "%d", iptv_prio);
					eval("vconfig", "set_egress_map", wan_dev, "0", iptv_prio_str);
				}
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);

				__setup_vlan(voip_vid, voip_prio, 0x00000210);	/* config WAN & WAN_MAC port */

				if (voip_prio) { /* config priority */
					snprintf(voip_prio_str, sizeof(voip_prio_str), "%d", voip_prio);
					eval("vconfig", "set_egress_map", wan_dev, "0", voip_prio_str);
				}
			}
		}
		if (switch_stb >=9 ) {
			if (mang_vid) { /* config tr069 on wan port */
				snprintf(wan_dev, sizeof(wan_dev), "vlan%d", mang_vid);
				nvram_set("wan12_ifname", wan_dev);
				sprintf(port_id, "%d", mang_vid);
				eval("vconfig", "add", interface, port_id);

				__setup_vlan(mang_vid, mang_prio, 0x00000210);	/* config WAN & WAN_MAC port */

				if (mang_prio) { /* config priority */
					snprintf(mang_prio_str, sizeof(mang_prio_str), "%d", mang_prio);
					eval("vconfig", "set_egress_map", wan_dev, "0", mang_prio_str);
				}
			}
		}
	}
#endif
}
#endif //RTCONFIG_SWCONFIG

#ifdef RA_SINGLE_SKU
void reset_ra_sku(const char *location, const char *country, const char *reg_spec)
{
	const char *try_list[] = { reg_spec, location, country, "CE", "FCC"};
	int i;
	for (i = 0; i < ARRAY_SIZE(try_list); i++) {
		if(try_list[i] != NULL && setRegSpec(try_list[i], 0) == 0)
			break;
	}

	if(i >= ARRAY_SIZE(try_list)) {
		cprintf("## NO SKU suit for %s\n", location);
		return;
	}

	cprintf("using %s SKU for %s\n", try_list[i], location);
	gen_ra_sku(try_list[i]);
}
#endif	/* RA_SINGLE_SKU */


/*=============================================================================
 smp_affinity: 1 = CPU1, 2 = CPU2, 3 = CPU3, 4 = CPU4
 rps_cpus: wxyz = CPU3 CPU2 CPU1 CPU0 (ex:0xd = 0'b1101 = CPU1, CPU3, CPU4)
 interface: 0 = wifi, 1 = usb0
=============================================================================*/

#if defined(RTCONFIG_MT798X)
void setup_smp(int noop)
{
	// RFB call this script after every wifi NIC hotplugged
	eval("/sbin/smp.sh", NULL);
}
#else
void setup_smp(int interface)
{
	if (interface == 1)
	{
#if defined(RT4GAX56) || defined(RTAX53U) || defined(XD4S)
		eval("/sbin/smp.sh", "usb0", NULL);
#endif
	}
	else
	{
#if defined(RTAC1200GU) || defined(RTAC1200GA1) || defined(RPAC87) || defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750) || defined(RTACRH18) || defined(RT4GAC86U) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
		eval("/sbin/smp.sh", "wifi", NULL);
#endif
	}

#if 0
#if defined(RTAC1200GU) || defined(RTAC1200GA1) || defined(RPAC87)
	f_write_string("/proc/irq/3/smp_affinity", "2", 0, 0);  //GMAC
	f_write_string("/proc/irq/4/smp_affinity", "4", 0, 0);  //PCIe0
	f_write_string("/proc/irq/24/smp_affinity", "8", 0, 0); //PCIe1
	f_write_string("/proc/irq/25/smp_affinity", "8", 0, 0); //PCIe2
	f_write_string("/proc/irq/19/smp_affinity", "8", 0, 0); //VPN
	f_write_string("/proc/irq/20/smp_affinity", "8", 0, 0); //SDXC
	f_write_string("/proc/irq/22/smp_affinity", "8", 0, 0); //USB

	f_write_string("/sys/class/net/ra0/queues/rx-0/rps_cpus", "3", 0, 0);
	f_write_string("/sys/class/net/rai0/queues/rx-0/rps_cpus", "3", 0, 0);
	f_write_string("/sys/class/net/eth2/queues/rx-0/rps_cpus", "3", 0, 0);
	f_write_string("/sys/class/net/eth3/queues/rx-0/rps_cpus", "3", 0, 0);
#endif
#endif
}
#endif

#ifdef RTCONFIG_SWCONFIG
extern void default_LANWANPartition();
void config_switch()
{
	char *switch_wantag = NULL;
	int stbport = 0;

	rtkswitch_AllPort_linkDown();

	if (config_switch_for_first_time){
			config_switch_for_first_time = 0;
	}
	else
	{
		dbG("software reset\n");
		eval("swconfig", "dev", "switch0", "set", "reset");
	}

	if (is_routing_enabled())
	{
		switch_wantag = nvram_safe_get("switch_wantag");
		stbport = nvram_get_int("switch_stb_x");

	extern void config_esw_LANWANPartition(int type);
	config_esw_LANWANPartition(0);
//CPU PORT is 5, LAN4 is 0 LAN3 is 1

		switch(stbport) {
			case 1:
				eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "3 4 5"); // set wan port
				break;
			case 2:
				eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "2 4 5"); // set wan port
				break;
			case 3:
				if (!strcmp(switch_wantag, "m1_fiber") || !strcmp(switch_wantag, "maxis_fiber_sp")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					//PORT3 need to keep tag
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 1t");
				} else 
				if (!strcmp(switch_wantag, "maxis_fiber")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					eval("swconfig", "dev", "switch0", "vlan", "821", "set", "ports", "4t 1t");
					eval("swconfig", "dev", "switch0", "vlan", "822", "set", "ports", "4t 1t");
				} else if (!strcmp(switch_wantag, "manual")) {
					if((strlen(nvram_safe_get("switch_wan0tagid")) != 0)){
						if(nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan2tagid")) {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 0");
						} else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 0");
						}
					}else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 0");
							eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4 5");
					}

				} else
				if(!strcmp(switch_wantag, "vodafone")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", "1", "set", "ports", "2 3 6");
					//PORT4
					eval("swconfig", "dev", "switch0", "vlan", "100", "set", "ports", "4t 0t 5");
					eval("swconfig", "dev", "switch0", "vlan", "101", "set", "ports", "4t 0t");
					eval("swconfig", "dev", "switch0", "vlan", "105", "set", "ports", "4t 0t 1");
				} else {
					eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "1 4 5"); // set wan port
				}

				break;
			case 4://LAN4 only
				if(!strcmp(switch_wantag, "unifi_home") || !strcmp(switch_wantag, "singtel_others") || !strcmp(switch_wantag, "orange")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					//PORT4 need to untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
				} else 
				if(!strcmp(switch_wantag, "meo")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					//PORT4 need to keep tag
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0t");
					//PROT3 do nothing
				} else if (!strcmp(switch_wantag, "manual")) {
					if((strlen(nvram_safe_get("switch_wan0tagid")) != 0)){
						if(nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan1tagid")) {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
						} else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 0");
						}
					}else{
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
							eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4 5");
					}
				} else {
					eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "0 4 5"); // set wan port
				}

				break;
			case 5://LAN1+LAN2
				eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "2 3 4 5"); // set wan port
				break;
			case 6://LAN3+LAN4
				if(!strcmp(switch_wantag, "singtel_mio")) {
					//VLAN for internet, untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
					//PORT4 need to untag in switch
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
					//PORT3 need to keep tag
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 1t");
				} else
				 if (!strcmp(switch_wantag, "manual")) {
					if((strlen(nvram_safe_get("switch_wan0tagid")) != 0)){
						if(nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan1tagid") && nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan2tagid")
							&& nvram_get_int("switch_wan1tagid") != nvram_get_int("switch_wan2tagid")) {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 1");
						} else if(nvram_get_int("switch_wan0tagid") != nvram_get_int("switch_wan1tagid") && nvram_get_int("switch_wan1tagid") == nvram_get_int("switch_wan2tagid")){
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0 1");
						}else if(nvram_get_int("switch_wan0tagid") == nvram_get_int("switch_wan2tagid") && nvram_get_int("switch_wan1tagid") != nvram_get_int("switch_wan2tagid")){
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 1");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
						} else if(nvram_get_int("switch_wan0tagid") == nvram_get_int("switch_wan1tagid") && nvram_get_int("switch_wan1tagid") != nvram_get_int("switch_wan2tagid")){
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 0");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 1");
						} else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5 0 1");
						}
					}else{
						if(nvram_get_int("switch_wan1tagid") != nvram_get_int("switch_wan2tagid")) {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0");
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan2tagid"), "set", "ports", "4t 1");
							eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4 5");
						}else {
							eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan1tagid"), "set", "ports", "4t 0 1");
							eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4 5");
						}
					}

				} else
					eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "0 1 4 5"); // set wan port

				break;
			default:
				if(!strcmp(switch_wantag, "movistar")) {
					eval("swconfig", "dev", "switch0", "vlan", "6", "set", "ports", "4t 5");
					eval("swconfig", "dev", "switch0", "vlan", "3", "set", "ports", "4t 5t");
					eval("swconfig", "dev", "switch0", "vlan", "2", "set", "ports", "4t 5t");
#if defined(RT4GAC86U)
					set_wan_tag("eth1");
#endif
				}
				if(nvram_get_int("switch_wan0tagid"))
					eval("swconfig", "dev", "switch0", "vlan", nvram_safe_get("switch_wan0tagid"), "set", "ports", "4t 5");
				else
					eval("swconfig", "dev", "switch0", "vlan","2","set", "ports", "4 5"); // set wan port
				break;
		}

		eval("swconfig", "dev", "switch0", "port","4","set", "vlan_prio", nvram_safe_get("switch_wan0prio"));
		eval("swconfig", "dev", "switch0", "port","0","set", "vlan_prio", nvram_safe_get("switch_wan1prio"));
		eval("swconfig", "dev", "switch0", "port","1","set", "vlan_prio", nvram_safe_get("switch_wan2prio"));
	}
	else
	{
		default_LANWANPartition();
	}

	rtkswitch_AllPort_linkUp();
}
#endif	/* RTCONFIG_SWCONFIG */
