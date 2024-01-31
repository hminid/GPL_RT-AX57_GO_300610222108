#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shutils.h"
#include "shared.h"
#include <ralink.h>

#if defined(RTCONFIG_MT798X)
extern int iface_name_to_vport(const char *iface);
#endif


#if defined(RTCONFIG_AMAS_ETHDETECT)

#if defined(PRTAX57_GO)
#define PORT_UNITS 2
#elif defined(TUFAX4200) || defined(TUFAX6000)
#define PORT_UNITS 6
#elif defined(RTAX59U)
#define PORT_UNITS 4
#else
#define PORT_UNITS 6
#endif

//Aimesh RE: vport to eth name
static const char *query_ifname[PORT_UNITS] = { //Aimesh RE
//      L1      L2      L3      L4     WAN1    WAN2   
#if defined(PRTAX57_GO)
//      L1      WAN1    
        "eth0", "eth1"
#elif defined(TUFAX4200) || defined(TUFAX6000)
	"lan5", "lan4", "lan3", "lan2", "lan1", "eth1"
#elif defined(RTAX59U)
        "lan3", "lan2", "lan1", "eth1"	
#else
//      P0      P1      P2      P3      P4      P5
//        NULL,   NULL,   NULL,   NULL,   NULL,   NULL
#error define query_ifname
#endif
};
#endif


struct channel_info {
	unsigned char channel;
	unsigned char bandwidth;
	unsigned char extrach;
};

int wl_get_bw(int unit)
{
	struct iwreq wrq;
	struct channel_info info;

	memset(&info, 0, sizeof(struct channel_info));
	wrq.u.data.length = sizeof(struct channel_info);
	wrq.u.data.pointer = (caddr_t) &info;
	wrq.u.data.flags = ASUS_SUBCMD_GCHANNELINFO;

	if (wl_ioctl(get_staifname(unit), RTPRIV_IOCTL_ASUSCMD, &wrq) < 0) {
		dbg("wl_ioctl failed on %s (%d)\n", __FUNCTION__, __LINE__);
		return -1;
	}

	switch (info.bandwidth) {
		case 0:
			return 20;
			break;
		case 1:
			return 40;
			break;
		case 2:
			return 80;
			break;
		case 3:
			return 160;
			break;
		default:
			break;
	}

	return 20;
}

#ifdef RTCONFIG_AMAS
char *get_pap_bssid(int unit, char bssid_str[])
{
	const char *ifname;
	char data[12];
	struct iwreq wrq;

	ifname = get_staifname(unit);

	memset(data, 0x00, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = OID_802_11_BSSID;

	if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0) {
		dbg("errors in getting %s bssid\n", ifname);
		return "";
	}

	ether_etoa(data, bssid_str);
	return bssid_str;
}

/*
 * wl_get_bw_cap(unit, *bwcap)
 *
 * bwcap
 * 	0x01 = 20 MHz
 * 	0x02 = 40 MHz
 * 	0x04 = 80 MHz
 * 	0x08 = 160 MHz
 *
 * 	ex: 5G support 20,40,80
 * 	*bwcap = 0x01 | 0x02 | 0x04
 */
int wl_get_bw_cap(int unit, int *bwcap)
{
	if (bwcap == NULL)
		return -1;
	if (unit == 0)
		*bwcap = 0x01 | 0x02;		/* 40MHz */
	else if (unit == 1)
	{
		*bwcap = 0x01 | 0x02 | 0x04;	/* 11AC 80MHz */
#if defined(RTCONFIG_MT798X)
		*bwcap |= 0x08;
#endif
	}
#ifdef RTCONFIG_HAS_5G_2
	else if (unit == 2)
		*bwcap = 0x01 | 0x02 | 0x04;	/* 11AC 80MHz */
#endif
	else
		return -1;

	return 0;
}

int wl_set_ch_bw(const char *ifname, int channel, int bw, int nctrlsb)
{
	if (set_bw_nctrlsb(ifname, bw, nctrlsb) < 0) {
		dbg("set %s bw (%d) or nctrlsb (%d) failed", ifname, bw, nctrlsb);
		return -1;
	}

	if (set_channel(ifname, channel) < 0) {
		dbg("set %s channel (%d) failed", ifname, channel);
		return -1;
	}

	return 0;
}

void sync_control_channel(int unit, int channel, int bw, int nctrlsb)
{
	char ifname[IFNAMSIZ];
	int ret __attribute__ ((unused));

	if (unit < 0 || unit >= MAX_NR_WL_IF)
		return;

	__get_wlifname(unit, 0, ifname);
	ret = wl_set_ch_bw(ifname, channel, bw, nctrlsb);
}

int get_psta_status(int unit)
{
	const char *ifname;
	char data[32];
	struct iwreq wrq;
	int status;

	ifname = get_staifname(unit);

	memset(data, 0x00, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = ASUS_SUBCMD_CONN_STATUS;

	if (wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0) {
		dbg("errors in getting %s CONN_STATUS result\n", ifname);
		return -1;
	}

	status = *(int*)wrq.u.data.pointer;

	if (status == 6)        // APCLI_CTRL_CONNECTED
		return WLC_STATE_CONNECTED;
	else if (status == 4)   // APCLI_CTRL_ASSOC
		return WLC_STATE_CONNECTING;

	return WLC_STATE_INITIALIZING;
}

enum {
    VSIE_BEACON = 0x1,
    VSIE_PROBE_REQ = 0x2,
    VSIE_PROBE_RESP = 0x4,
    VSIE_ASSOC_REQ = 0x8,
    VSIE_ASSOC_RESP = 0x10,
    VSIE_AUTH_REQ = 0x20,
    VSIE_AUTH_RESP = 0x40
};

void vsie_operation(int unit, int subunit, int flag, int opt, char *hexdata)
{
	struct iwreq wrq;
	char cmd_data[512], ifname[16];

	int len = 0;

	len = 3 + strlen(hexdata)/2;    /* 3 is oui's len */

	__get_wlifname(unit, subunit, ifname);

	snprintf(cmd_data, sizeof(cmd_data), "vie_op=%d-frm_map:%d-oui:%02X%02X%02X-length:%d-ctnt:%s",
		opt, flag, (uint8_t)OUI_ASUS[0], (uint8_t)OUI_ASUS[1], (uint8_t)OUI_ASUS[2], len, hexdata);

	wrq.u.data.length = strlen(cmd_data) + 1;
	wrq.u.data.pointer = cmd_data;
	wrq.u.data.flags = 0;

        if (wl_ioctl(ifname, RTPRIV_IOCTL_SET, &wrq) < 0)
                dbg("wl_ioctl failed on %s (%d)\n", __FUNCTION__, __LINE__);

    return;
}

/**
 * @brief add beacon vise by unit and subunit
 *
 * @param unit band index
 * @param subunit mssid index
 * @param hexdata vise string
 */
void add_beacon_vsie_by_unit(int unit, int subunit, char *hexdata)
{
    vsie_operation(unit, subunit, VSIE_BEACON | VSIE_PROBE_RESP, 1, hexdata);
}

/**
 * @brief add guest vsie
 *
 * @param hexdata vsie string
 */
void add_beacon_vsie_guest(char *hexdata)
{
    int unit = 0, subunit = 0;
    char word[100], *next;

    foreach (word, nvram_safe_get("wl_ifnames"), next) {
        if (nvram_get_int("re_mode") == 1)  // RE
            subunit = 3;
        else  // CAP/Router
            subunit = 2;
        for (; subunit <= num_of_mssid_support(unit); subunit++) {
            char ifname[16];
            __get_wlifname(unit, subunit, ifname);
            if (is_intf_up(ifname) != -1)  // interface exist
		    	vsie_operation(unit, subunit, VSIE_BEACON | VSIE_PROBE_RESP, 1, hexdata);
       }
        unit++;
    }
}

void add_beacon_vsie(char *hexdata)
{
#ifdef RTCONFIG_BHCOST_OPT
    int unit = 0;
    char word[100], *next;

    foreach (word, nvram_safe_get("wl_ifnames"), next) {
    	vsie_operation(unit, 0, VSIE_BEACON | VSIE_PROBE_RESP, 1, hexdata);
        unit++;
    }
#else
    vsie_operation(0, 0, VSIE_BEACON | VSIE_PROBE_RESP, 1, hexdata);
#endif
}

/**
 * @brief remove beacon vsie by unit and subunit
 *
 * @param unit band index
 * @param subunit mssid index
 * @param hexdata vsie string
 */
void del_beacon_vsie_by_unit(int unit, int subunit, char *hexdata)
{
	vsie_operation(unit, subunit, VSIE_BEACON | VSIE_PROBE_RESP, 3, hexdata);
}

/**
 * @brief remove guest beacon vsie
 *
 * @param hexdata vsie string
 */
void del_beacon_vsie_guest(char *hexdata)
{
    int unit = 0, subunit = 0;
    char word[100], *next;

    foreach (word, nvram_safe_get("wl_ifnames"), next) {
        if (nvram_get_int("re_mode") == 1)  // RE
            subunit = 3;
        else  // CAP/Router
            subunit = 2;
        for (; subunit <= num_of_mssid_support(unit); subunit++) {
            char ifname[16];
            __get_wlifname(unit, subunit, ifname);
            if (is_intf_up(ifname) != -1)  // interface exist
		    	vsie_operation(unit, subunit, VSIE_BEACON | VSIE_PROBE_RESP, 3, hexdata);
        }
        unit++;
    }
}

void del_beacon_vsie(char *hexdata)
{
#ifdef RTCONFIG_BHCOST_OPT
    int unit = 0;
    char word[100], *next;

    foreach (word, nvram_safe_get("wl_ifnames"), next) {
    	vsie_operation(unit, 0, VSIE_BEACON | VSIE_PROBE_RESP, 3, hexdata);
        unit++;
    }
#else
    vsie_operation(0, 0, VSIE_BEACON | VSIE_PROBE_RESP, 3, hexdata);
#endif
}

void add_probe_req_vsie(char *hexdata)
{
    vsie_operation(0, 0, VSIE_PROBE_REQ, 1, hexdata);
}

void del_probe_req_vsie(char *hexdata)
{
    vsie_operation(0, 0, VSIE_PROBE_REQ, 3, hexdata);
}

void wait_connection_finished(int band)
{
    int wait_time = 0;
    int conn_stat = 0;
    int wlc_conn_time = nvram_get_int("wlc_conn_time") ?: 10;

    while (wait_time++ < wlc_conn_time) {
        conn_stat = get_psta_status(band);
        if (conn_stat == WLC_STATE_CONNECTED) break;
        sleep(1);
    }
}

int get_wlan_service_status(int bssidx, int vifidx)
{
    if (nvram_get_int("wlready") == 0) return -1;

    char *ifname = NULL;
    char tmp[128] = {0}, prefix[] = "wlXXXXXXXXXX_";
    char wl_radio[] = "wlXXXX_radio";

    snprintf(wl_radio, sizeof(wl_radio), "wl%d_radio", bssidx);
    if (nvram_get_int(wl_radio) == 0)
        return -2;

    if (vifidx > 0)
        snprintf(prefix, sizeof(prefix), "wl%d.%d_", bssidx, vifidx);
    else
        snprintf(prefix, sizeof(prefix), "wl%d_", bssidx);

    ifname = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

    if (is_intf_up(ifname) > 0) return get_radio(bssidx, vifidx);

    return 0;
}

void set_wlan_service_status(int bssidx, int vifidx, int enabled)
{
    if (nvram_get_int("wlready") == 0) return;

    char *ifname = NULL;
    char tmp[128] = {0}, prefix[] = "wlXXXXXXXXXX_";
    char wl_radio[] = "wlXXXX_radio";

    snprintf(wl_radio, sizeof(wl_radio), "wl%d_radio", bssidx);
    if (nvram_get_int(wl_radio) == 0)
        return;

    if (vifidx > 0)
        snprintf(prefix, sizeof(prefix), "wl%d.%d_", bssidx, vifidx);
    else
        snprintf(prefix, sizeof(prefix), "wl%d_", bssidx);

    ifname = nvram_safe_get(strcat_r(prefix, "ifname", tmp));
    if (!enabled && bssidx)
        doSystem("iwpriv %s set DfsCacClean=1", ifname);
    eval("ifconfig", ifname, enabled? "up":"down");
}


#ifdef RTCONFIG_BHCOST_OPT
#ifdef RTCONFIG_AMAS_ETHDETECT
unsigned int get_uplinkports_linkrate(char *ifname)
{
        unsigned int link_rate = 0;
        int connect=0 ,speed=0;
        int vport = 0;

        for (vport = 0; vport < PORT_UNITS; vport++) 
	{
                if (vport >= ARRAY_SIZE(query_ifname)) {
                        dbg("%s: don't know vport %d\n", __func__, vport);
                        return 0;
                }
                if (query_ifname[vport] != NULL && strstr(query_ifname[vport],ifname)) 
		{
#if defined(RTCONFIG_MT798X)
			get_mt7986_mt7531_vport_info(iface_name_to_vport(ifname), &connect, &speed, NULL);
			if(connect)
			{	
				link_rate=speed;
				break;
			}
#else
#error port status and linkrate
#endif			
                }
        }
        return link_rate;
}
/**
 * @brief Get the uplinkports status
 *
 * @param ifname ethernet uplink ifname
 * @return int connnected(1) or not(0)
 */

int get_uplinkports_status(char *ifname)
{
        int vport = 0;
        int connect=0 ,speed=0;
        for (vport = 0; vport < PORT_UNITS; vport++) {
                if (vport >= ARRAY_SIZE(query_ifname)) {
                        dbg("%s: don't know vport %d\n", __func__, vport);
                        return 0;
                }
                if (query_ifname[vport] != NULL && strstr(query_ifname[vport],ifname)) 
		{
#if defined(RTCONFIG_MT798X)
			get_mt7986_mt7531_vport_info(iface_name_to_vport(ifname), &connect, &speed, NULL);
			if(connect)
				return 1;
#else
#error port status and linkrate
#endif			
                }
        }
        return 0;
}
#else
unsigned int get_uplinkports_linkrate(char *ifname)
{
	int speed;
	char *eth=NULL;
	speed=0;
	eth=nvram_safe_get("eth_ifnames");
	if(eth && strstr(eth,ifname))
		speed = rtkswitch_WanPort_phySpeed();
	return speed;
}

/**
 * @brief Get the uplinkports status
 *
 * @param ifname ethernet uplink ifname
 * @return int connnected(1) or not(0)
 */
int get_uplinkports_status(char *ifname)
{
        int wan_unit = wan_primary_ifunit();

        return get_wanports_status(wan_unit);
}
#endif  /* RTCONFIG_AMAS_ETHDETECT */
#endif	/* RTCONFIG_BHCOST_OPT */
#endif 

#ifdef RTCONFIG_CFGSYNC
void update_macfilter_relist(void)
{
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";
	char word[256], *next;
	char mac2g[32], mac5g[32], *next_mac;
	int unit = 0;
	char *wlif_name = NULL;
	char *nv, *nvp, *b;
	char *reMac, *maclist2g, *maclist5g, *timestamp;
	char stamac2g[18] = {0};
	char stamac5g[18] = {0};

	if (is_cfg_relist_exist())
	{
#ifdef RTCONFIG_AMAS
		if (nvram_get_int("re_mode") == 1) {
			nv = nvp = get_cfg_relist(0);
			if (nv) {
				while ((b = strsep(&nvp, "<")) != NULL) {
					if ((vstrsep(b, ">", &reMac, &maclist2g, &maclist5g, &timestamp) != 4))
						continue;
					/* first mac for sta 2g of dut */
					foreach_44 (mac2g, maclist2g, next_mac)
						break;
					/* first mac for sta 5g of dut */
					foreach_44 (mac5g, maclist5g, next_mac)
						break;

					if (strcmp(reMac, get_lan_hwaddr()) == 0) {
						snprintf(stamac2g, sizeof(stamac2g), "%s", mac2g);
						dbg("dut 2g sta (%s)\n", stamac2g);
						snprintf(stamac5g, sizeof(stamac5g), "%s", mac5g);
						dbg("dut 5g sta (%s)\n", stamac5g);
						break;
					}
				}
				free(nv);
			}
		}
#endif

		foreach (word, nvram_safe_get("wl_ifnames"), next) {
			SKIP_ABSENT_BAND_AND_INC_UNIT(unit);

#ifdef RTCONFIG_AMAS
			if (nvram_get_int("re_mode") == 1)
				snprintf(prefix, sizeof(prefix), "wl%d.1_", unit);
			else
#endif
				snprintf(prefix, sizeof(prefix), "wl%d_", unit);

			wlif_name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

			if (nvram_match(strcat_r(prefix, "macmode", tmp), "allow")) {
				nv = nvp = get_cfg_relist(0);
				if (nv) {
					while ((b = strsep(&nvp, "<")) != NULL) {
						if ((vstrsep(b, ">", &reMac, &maclist2g, &maclist5g, &timestamp) != 4))
							continue;

						if (strcmp(reMac, get_lan_hwaddr()) == 0)
							continue;

						if (unit == 0) {
							foreach_44 (mac2g, maclist2g, next_mac) {
								if (check_re_in_macfilter(unit, mac2g))
									continue;
								dbg("relist sta (%s) in %s\n", mac2g, wlif_name);
								set_acl_entry(wlif_name, mac2g);
							}
						}
						else
						{
							foreach_44 (mac5g, maclist5g, next_mac) {
								if (check_re_in_macfilter(unit, mac5g))
									continue;
								dbg("relist sta (%s) in %s\n", mac5g, wlif_name);
								set_acl_entry(wlif_name, mac5g);
							}
						}
					}
					free(nv);
				}
			}

			unit++;
		}
	}
}
#endif

#ifdef RTCONFIG_NEW_PHYMAP
extern int get_trunk_port_mapping(int trunk_port_value)
{
	return trunk_port_value;
}

#if defined(RTCONFIG_MT798X)
void mt798x_get_phy_port_mapping(phy_port_mapping *port_mapping);
#endif

/* phy port related start */
void get_phy_port_mapping(phy_port_mapping *port_mapping)
{
#if !defined(RTCONFIG_MT798X)
	static phy_port_mapping port_mapping_static = {
#if defined(RT4GAX56)
		.count = 6,
		.is_mobile_router = 1,
		.port[0] = { .phy_port_id = -1, .ext_port_id = 0, .label_name = "W0", .cap = PHY_PORT_CAP_WAN, .max_rate = 1000, .ifname = "eth1", .flag = 0 },
		.port[1] = { .phy_port_id = -1, .ext_port_id = 1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = "eth0", .flag = 0 },
		.port[2] = { .phy_port_id = -1, .ext_port_id = 2, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = "eth0", .flag = 0 },
		.port[3] = { .phy_port_id = -1, .ext_port_id = 3, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = "eth0", .flag = 0 },
		.port[4] = { .phy_port_id = -1, .ext_port_id = 4, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = "eth0", .flag = 0 },
		.port[5] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "M1", .cap = PHY_PORT_CAP_MOBILE, .max_rate = 480, .ifname = "usb0", .flag = 0 }
#elif defined(RT4GAC86U)
		.count = 7,
		.is_mobile_router = 1,
		.port[0] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[1] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[2] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[3] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[4] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[5] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "U1", .cap = PHY_PORT_CAP_USB, .max_rate = 480, .ifname = NULL, .flag = 0 },
		.port[6] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "M1", .cap = PHY_PORT_CAP_MOBILE, .max_rate = 480, .ifname = NULL, .flag = 0 }
#elif defined(RTAX54)
		.count = 5,
		.port[0] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[1] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[2] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[3] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[4] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
#elif defined(RTAX53U)
		.count = 5,
		.port[0] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[1] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[2] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[3] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[4] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "U1", .cap = PHY_PORT_CAP_USB, .max_rate = 480, .ifname = NULL, .flag = 0 },
#elif defined(RTACRH18)
		.count = 6,
		.port[0] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[1] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[2] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[3] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[4] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[5] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "U1", .cap = PHY_PORT_CAP_USB, .max_rate = 5000, .ifname = NULL, .flag = 0 },
#elif defined(XD4S)
		.count = 2,
		.port[0] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
		.port[1] = { .phy_port_id = -1, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0 },
#else
		#error "port_mapping is not defined."
#endif
	};

	if (!port_mapping)
		return;

	memcpy(port_mapping, &port_mapping_static, sizeof(phy_port_mapping));

	add_sw_cap(port_mapping);
	swap_wanlan(port_mapping);
	return;
#else // RTCONFIG_MT798X
	mt798x_get_phy_port_mapping(port_mapping);
	return;
#endif
}
#endif

#if !defined(RTCONFIG_WLMODULE_MT7915D_AP) && !defined(RTCONFIG_MT798X)
int MCSMappingRateTable[] =
	{2,  4,   11,  22, // CCK
	12, 18,   24,  36, 48, 72, 96, 108, // OFDM
	13, 26,   39,  52,  78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260, // 20MHz, 800ns GI, MCS: 0 ~ 15
	39, 78,  117, 156, 234, 312, 351, 390,										  // 20MHz, 800ns GI, MCS: 16 ~ 23
	27, 54,   81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540, // 40MHz, 800ns GI, MCS: 0 ~ 15
	81, 162, 243, 324, 486, 648, 729, 810,										  // 40MHz, 800ns GI, MCS: 16 ~ 23
	14, 29,   43,  57,  87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288, // 20MHz, 400ns GI, MCS: 0 ~ 15
	43, 87,  130, 173, 260, 317, 390, 433,										  // 20MHz, 400ns GI, MCS: 16 ~ 23
	30, 60,   90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600, // 40MHz, 400ns GI, MCS: 0 ~ 15
	90, 180, 270, 360, 540, 720, 810, 900,
	13, 26,   39,  52,  78, 104, 117, 130, 156, /* 11ac: 20Mhz, 800ns GI, MCS: 0~8 */
	27, 54,   81, 108, 162, 216, 243, 270, 324, 360, /*11ac: 40Mhz, 800ns GI, MCS: 0~9 */
	59, 117, 176, 234, 351, 468, 527, 585, 702, 780, /*11ac: 80Mhz, 800ns GI, MCS: 0~9 */
	14, 29,   43,  57,  87, 115, 130, 144, 173, /* 11ac: 20Mhz, 400ns GI, MCS: 0~8 */
	30, 60,   90, 120, 180, 240, 270, 300, 360, 400, /*11ac: 40Mhz, 400ns GI, MCS: 0~9 */
	65, 130, 195, 260, 390, 520, 585, 650, 780, 867 /*11ac: 80Mhz, 400ns GI, MCS: 0~9 */
	};
#endif

#if defined(RTCONFIG_WLMODULE_MT7663E_AP) || defined(RTCONFIG_WLMODULE_MT7629_AP) || defined(RTCONFIG_WLMODULE_MT7622_AP)
int MCSMappingRateTable_5G[] = {
	2,  4, 11, 22, 12,  18,  24,  36, 48,  72,  96, 108, 109, 110, 111, 112,/* CCK and OFDM */
	13, 26, 39, 52, 78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260,
	39, 78, 117, 156, 234, 312, 351, 390, /* BW 20, 800ns GI, MCS 0~23 */
	27, 54, 81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540,
	81, 162, 243, 324, 486, 648, 729, 810, /* BW 40, 800ns GI, MCS 0~23 */
	14, 29, 43, 57, 87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288,
	43, 87, 130, 173, 260, 317, 390, 433, /* BW 20, 400ns GI, MCS 0~23 */
	30, 60, 90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600,
	90, 180, 270, 360, 540, 720, 810, 900, /* BW 40, 400ns GI, MCS 0~23 */

	/*for 11ac:20 Mhz 800ns GI*/
	6,  13, 19, 26,  39,  52,  58,  65,  78,  0,     /*1ss mcs 0~8*/
	13, 26, 39, 52,  78,  104, 117, 130, 156, 0,     /*2ss mcs 0~8*/
	19, 39, 58, 78,  117, 156, 175, 195, 234, 260,   /*3ss mcs 0~9*/
	26, 52, 78, 104, 156, 208, 234, 260, 312, 0,     /*4ss mcs 0~8*/

	/*for 11ac:40 Mhz 800ns GI*/
	13,	27,	40,	54,	 81,  108, 121, 135, 162, 180,   /*1ss mcs 0~9*/
	27,	54,	81,	108, 162, 216, 243, 270, 324, 360,   /*2ss mcs 0~9*/
	40,	81,	121, 162, 243, 324, 364, 405, 486, 540,  /*3ss mcs 0~9*/
	54,	108, 162, 216, 324, 432, 486, 540, 648, 720, /*4ss mcs 0~9*/

	/*for 11ac:80 Mhz 800ns GI*/
	29,	58,	87,	117, 175, 234, 263, 292, 351, 390,   /*1ss mcs 0~9*/
	58,	117, 175, 243, 351, 468, 526, 585, 702, 780, /*2ss mcs 0~9*/
	87,	175, 263, 351, 526, 702, 0,	877, 1053, 1170, /*3ss mcs 0~9*/
	117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560, /*4ss mcs 0~9*/

	/*for 11ac:160 Mhz 800ns GI*/
	58,	117, 175, 234, 351, 468, 526, 585, 702, 780, /*1ss mcs 0~9*/
	117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560, /*2ss mcs 0~9*/
	175, 351, 526, 702, 1053, 1404, 1579, 1755, 2160, 0, /*3ss mcs 0~8*/
	234, 468, 702, 936, 1404, 1872, 2106, 2340, 2808, 3120, /*4ss mcs 0~9*/

	/*for 11ac:20 Mhz 400ns GI*/
	7,	14,	21,	28,  43,  57,   65,	 72,  86,  0,    /*1ss mcs 0~8*/
	14,	28,	43,	57,	 86,  115,  130, 144, 173, 0,    /*2ss mcs 0~8*/
	21,	43,	65,	86,	 130, 173,  195, 216, 260, 288,  /*3ss mcs 0~9*/
	28,	57,	86,	115, 173, 231,  260, 288, 346, 0,    /*4ss mcs 0~8*/

	/*for 11ac:40 Mhz 400ns GI*/
	15,	30,	45,	60,	 90,  120,  135, 150, 180, 200,  /*1ss mcs 0~9*/
	30,	60,	90,	120, 180, 240,  270, 300, 360, 400,  /*2ss mcs 0~9*/
	45,	90,	135, 180, 270, 360,  405, 450, 540, 600, /*3ss mcs 0~9*/
	60,	120, 180, 240, 360, 480,  540, 600, 720, 800, /*4ss mcs 0~9*/

	/*for 11ac:80 Mhz 400ns GI*/
	32,	65,	97,	130, 195, 260,  292, 325, 390, 433,  /*1ss mcs 0~9*/
	65,	130, 195, 260, 390, 520,  585, 650, 780, 866, /*2ss mcs 0~9*/
	97,	195, 292, 390, 585, 780,  0,	 975, 1170, 1300, /*3ss mcs 0~9*/
	130, 260, 390, 520, 780, 1040,	1170, 1300, 1560, 1733, /*4ss mcs 0~9*/

	/*for 11ac:160 Mhz 400ns GI*/
	65,	130, 195, 260, 390, 520,  585, 650, 780, 866, /*1ss mcs 0~9*/
	130, 260, 390, 520, 780, 1040,	1170, 1300, 1560, 1733, /*2ss mcs 0~9*/
	195, 390, 585, 780, 1170, 1560,	1755, 1950, 2340, 0, /*3ss mcs 0~8*/
	260, 520, 780, 1040, 1560, 2080,	2340, 2600, 3120, 3466, /*4ss mcs 0~9*/

	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37
}; /* 3*3 */

#define FN_GETRATE(_fn_, _st_, _if, _mcstbl)						\
_fn_(_st_ HTSetting)							\
{									\
	unsigned char Antenna = 0;	\
	unsigned char MCS = HTSetting.field.MCS;	\
	int rate_count = sizeof(_mcstbl)/sizeof(int);	\
	int rate_index = 0;						\
	int value = 0;	\
									\
	if (HTSetting.field.MODE >= MODE_VHT)				\
	{								\
		if(_if == 1) {	\
			MCS = HTSetting.field.MCS & 0xf;	\
			Antenna = (HTSetting.field.MCS >> 4) + 1;	\
														\
			if (HTSetting.field.BW == BW_20) {	\
				rate_index = 112 + ((Antenna - 1) * 10) + ((unsigned char)HTSetting.field.ShortGI * 160) + ((unsigned char)MCS);	\
			} else if (HTSetting.field.BW == BW_40) {	\
				rate_index = 152 + ((Antenna - 1) * 10) + ((unsigned char)HTSetting.field.ShortGI * 160) + ((unsigned char)MCS);	\
			} else if (HTSetting.field.BW == BW_80) {	\
				rate_index = 192 + ((Antenna - 1) * 10) + ((unsigned char)HTSetting.field.ShortGI * 160) + ((unsigned char)MCS);	\
			} else if (HTSetting.field.BW == BW_160) {	\
				rate_index = 232 + ((Antenna - 1) * 10) + ((unsigned char)HTSetting.field.ShortGI * 160) + ((unsigned char)MCS);	\
			}	\
		}	\
		else	\
		if (HTSetting.field.BW == BW_20) {			\
			rate_index = 108 +				\
			((unsigned char)HTSetting.field.ShortGI * 29) +	\
			((unsigned char)HTSetting.field.MCS);		\
		}							\
		else if (HTSetting.field.BW == BW_40) {			\
			rate_index = 117 +				\
			((unsigned char)HTSetting.field.ShortGI * 29) +	\
			((unsigned char)HTSetting.field.MCS);		\
		}							\
		else if (HTSetting.field.BW == BW_80) {			\
			rate_index = 127 +				\
			((unsigned char)HTSetting.field.ShortGI * 29) +	\
			((unsigned char)HTSetting.field.MCS);		\
		}							\
	}								\
	else								\
	if (HTSetting.field.MODE >= MODE_HTMIX)				\
	{								\
		if(_if == 1)	\
		{	\
			MCS = HTSetting.field.MCS;	\
			\
			if ((HTSetting.field.MODE == MODE_HTMIX) || (HTSetting.field.MODE == MODE_HTGREENFIELD))	\
				Antenna = (MCS >> 3) + 1;	\
			\
			/* map back to 1SS MCS , multiply by antenna numbers later */		\
			if (MCS > 7)		\
				MCS %= 8;		\
			\
			rate_index = 16 + ((unsigned char)HTSetting.field.BW * 24) + ((unsigned char)HTSetting.field.ShortGI * 48) + ((unsigned char)MCS);		\
		}else	\
			rate_index = 12 + ((unsigned char)HTSetting.field.BW *24) + ((unsigned char)HTSetting.field.ShortGI *48) + ((unsigned char)HTSetting.field.MCS);	\
	}								\
	else								\
		if (HTSetting.field.MODE == MODE_OFDM)				\
			rate_index = (unsigned char)(HTSetting.field.MCS) + 4;	\
		else if (HTSetting.field.MODE == MODE_CCK)			\
			rate_index = (unsigned char)(HTSetting.field.MCS);	\
									\
	if (rate_index < 0)						\
		rate_index = 0;						\
									\
	if (rate_index >= rate_count)					\
		rate_index = rate_count-1;				\
	\
	if(_if == 1)	{		\
		if (HTSetting.field.MODE != MODE_VHT)	\
			value = (_mcstbl[rate_index] * 5) / 10;	\
		else	\
			value =  _mcstbl[rate_index];	\
	}else		\
		value = (_mcstbl[rate_index] * 5) / 10;	\
	\
	return value;		\
}
#elif defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
/* mt_wifi/embadded/common/cmm_info */
int MCSMappingRateTable[] = {
	2,  4, 11, 22, 12,  18,  24,  36, 48,  72,  96, 108, 109, 110, 111, 112,/* CCK and OFDM */
	13, 26, 39, 52, 78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260,
	39, 78, 117, 156, 234, 312, 351, 390, /* BW 20, 800ns GI, MCS 0~23 */
	27, 54, 81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540,
	81, 162, 243, 324, 486, 648, 729, 810, /* BW 40, 800ns GI, MCS 0~23 */
	14, 29, 43, 57, 87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288,
	43, 87, 130, 173, 260, 317, 390, 433, /* BW 20, 400ns GI, MCS 0~23 */
	30, 60, 90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600,
	90, 180, 270, 360, 540, 720, 810, 900, /* BW 40, 400ns GI, MCS 0~23 */

	/*for 11ac:20 Mhz 800ns GI*/
	6,  13, 19, 26,  39,  52,  58,  65,  78,  90,     /*1ss mcs 0~8*/
	13, 26, 39, 52,  78,  104, 117, 130, 156, 180,     /*2ss mcs 0~8*/
	19, 39, 58, 78,  117, 156, 175, 195, 234, 260,   /*3ss mcs 0~9*/
	26, 52, 78, 104, 156, 208, 234, 260, 312, 360,     /*4ss mcs 0~8*/

	/*for 11ac:40 Mhz 800ns GI*/
	13,	27,	40,	54,	 81,  108, 121, 135, 162, 180,   /*1ss mcs 0~9*/
	27,	54,	81,	108, 162, 216, 243, 270, 324, 360,   /*2ss mcs 0~9*/
	40,	81,	121, 162, 243, 324, 364, 405, 486, 540,  /*3ss mcs 0~9*/
	54,	108, 162, 216, 324, 432, 486, 540, 648, 720, /*4ss mcs 0~9*/

	/*for 11ac:80 Mhz 800ns GI*/
	29,	58,	87,	117, 175, 234, 263, 292, 351, 390,   /*1ss mcs 0~9*/
	58,	117, 175, 243, 351, 468, 526, 585, 702, 780, /*2ss mcs 0~9*/
	87,	175, 263, 351, 526, 702, 0,	877, 1053, 1170, /*3ss mcs 0~9*/
	117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560, /*4ss mcs 0~9*/

	/*for 11ac:160 Mhz 800ns GI*/
	58,	117, 175, 234, 351, 468, 526, 585, 702, 780, /*1ss mcs 0~9*/
	117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560, /*2ss mcs 0~9*/
	175, 351, 526, 702, 1053, 1404, 1579, 1755, 2160, 0, /*3ss mcs 0~8*/
	234, 468, 702, 936, 1404, 1872, 2106, 2340, 2808, 3120, /*4ss mcs 0~9*/

	/*for 11ac:20 Mhz 400ns GI*/
	7,	14,	21,	28,  43,  57,   65,	 72,  86,  100,    /*1ss mcs 0~8*/
	14,	28,	43,	57,	 86,  115,  130, 144, 173, 200,    /*2ss mcs 0~8*/
	21,	43,	65,	86,	 130, 173,  195, 216, 260, 288,  /*3ss mcs 0~9*/
	28,	57,	86,	115, 173, 231,  260, 288, 346, 400,    /*4ss mcs 0~8*/

	/*for 11ac:40 Mhz 400ns GI*/
	15,	30,	45,	60,	 90,  120,  135, 150, 180, 200,  /*1ss mcs 0~9*/
	30,	60,	90,	120, 180, 240,  270, 300, 360, 400,  /*2ss mcs 0~9*/
	45,	90,	135, 180, 270, 360,  405, 450, 540, 600, /*3ss mcs 0~9*/
	60,	120, 180, 240, 360, 480,  540, 600, 720, 800, /*4ss mcs 0~9*/

	/*for 11ac:80 Mhz 400ns GI*/
	32,	65,	97,	130, 195, 260,  292, 325, 390, 433,  /*1ss mcs 0~9*/
	65,	130, 195, 260, 390, 520,  585, 650, 780, 866, /*2ss mcs 0~9*/
	97,	195, 292, 390, 585, 780,  0,	 975, 1170, 1300, /*3ss mcs 0~9*/
	130, 260, 390, 520, 780, 1040,	1170, 1300, 1560, 1733, /*4ss mcs 0~9*/

	/*for 11ac:160 Mhz 400ns GI*/
	65,	130, 195, 260, 390, 520,  585, 650, 780, 866, /*1ss mcs 0~9*/
	130, 260, 390, 520, 780, 1040,	1170, 1300, 1560, 1733, /*2ss mcs 0~9*/
	195, 390, 585, 780, 1170, 1560,	1755, 1950, 2340, 0, /*3ss mcs 0~8*/
	260, 520, 780, 1040, 1560, 2080,	2340, 2600, 3120, 3466, /*4ss mcs 0~9*/

	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37
}; /* 3*3 */

#define MAX_NUM_HE_BANDWIDTHS 4
#define MAX_NUM_HE_SPATIAL_STREAMS 4
#define MAX_NUM_HE_MCS_ENTRIES 12
unsigned short he_mcs_phyrate_mapping_table[MAX_NUM_HE_BANDWIDTHS][MAX_NUM_HE_SPATIAL_STREAMS][MAX_NUM_HE_MCS_ENTRIES] = {
	{ /*20 Mhz*/
		/* 1 SS */
		{
			/* DCM 0*/
			 8, 17, 25, 34, 51, 68, 77, 86, 103, 114, 129, 143
		},
		/* 2 SS */
		{
			/* DCM 0 */
			 17, 34, 51, 68, 103, 137, 154, 172, 206, 229, 258, 286
		},
		/* 3 SS */
		{
			/* DCM 0 */
			 25, 51, 77, 103, 154, 206, 232, 258, 309, 344, 387, 430
		},
		/* 4 SS */
		{
			/* DCM 0 */
			 34, 68, 103, 137, 206, 275, 309, 344, 412, 458, 516, 573
		}
	},
	{ /*40 Mhz*/
		/* 1 SS */
		{
			/* DCM 0*/
			 17, 34, 51, 68, 103, 137, 154, 172, 206, 229, 258, 286
		},
		/* 2 SS */
		{
			/* DCM 0 */
			 34, 68, 103, 137, 206, 275, 309, 344, 412, 458, 516, 573
		},
		/* 3 SS */
		{
			/* DCM 0 */
			 51, 103, 154, 206, 309, 412, 464, 516, 619, 688, 774, 860
		},
		/* 4 SS */
		{
			/* DCM 0 */
			 68, 137, 206, 275, 412, 550, 619, 688, 825, 917, 1032, 1147
		}
	},
	{ /*80 Mhz*/
		/* 1 SS */
		{
			/* DCM 0*/
			36, 72, 108, 144, 216, 288, 324, 360, 432, 480, 540, 600
		},
		/* 2 SS */
		{
			/* DCM 0 */
			 72, 144, 216, 288, 432, 576, 648, 720, 864, 960, 1080, 1201
		},
		/* 3 SS */
		{
			/* DCM 0 */
			 108, 216, 324, 432, 648, 864, 972, 1080, 1297, 1441, 1621, 1801
		},
		/* 4 SS */
		{
			/* DCM 0 */
			 144, 288, 432, 576, 864, 1152, 1297, 1141, 1729, 1921, 2161, 2401
		}
	},
	{ /*160 Mhz*/
		/* 1 SS */
		{
			/* DCM 0*/
			 72, 144, 216, 288, 432, 576, 648, 720, 864, 960, 1080, 1201
		},
		/* 2 SS */
		{
			/* DCM 0 */
			 144, 288, 432, 576, 864, 1152, 1297, 1441, 1729, 1921, 2161, 2401
		},
		/* 3 SS */
		{
			/* DCM 0 */
			 216, 432, 648, 864, 1297, 1729, 1945, 2161, 2594, 2882, 3242, 3602
		},
		/* 4 SS */
		{
			/* DCM 0 */
			 288, 576, 864, 1152, 1729, 2305, 2594, 2882, 3458, 3843, 4323, 4803
		},
	}
};

#define FN_GETRATE(_fn_, _st_, _if, _mcstbl)						\
_fn_(_st_ HTSetting)							\
{									\
	unsigned char Antenna = 0;	\
	unsigned char MCS = HTSetting.field.MCS;	\
	unsigned char BW = HTSetting.field.BW;	\
	unsigned char NSS = ((HTSetting.field.MCS >> 4) & 0x3) + 1;		\
	int rate_count = sizeof(_mcstbl)/sizeof(int);	\
	int rate_index = 0;						\
	int value = 0;	\
									\
	if (HTSetting.field.MODE >= MODE_HE)				\
	{								\
		NSS = ((HTSetting.field.MCS >> 4) & 0x3) + 1;		\
		MCS = HTSetting.field.MCS & 0xf;	\
		if (NSS == 0) {								\
			NSS = 1;								\
		}								\
								\
		if (MCS >= MAX_NUM_HE_MCS_ENTRIES)								\
			MCS = MAX_NUM_HE_MCS_ENTRIES - 1;								\
								\
		if (NSS > MAX_NUM_HE_SPATIAL_STREAMS)								\
			NSS = MAX_NUM_HE_SPATIAL_STREAMS;								\
								\
		if (BW >= MAX_NUM_HE_BANDWIDTHS)								\
			BW = MAX_NUM_HE_BANDWIDTHS - 1;								\
								\
		NSS--;								\
								\
		value = he_mcs_phyrate_mapping_table[BW][NSS][MCS];								\
	}								\
	else								\
	{								\
		if (HTSetting.field.MODE >= MODE_VHT)				\
		{				\
			MCS = HTSetting.field.MCS & 0xf;	\
			Antenna = (HTSetting.field.MCS >> 4) + 1;	\
														\
			if (HTSetting.field.BW == BW_20) {	\
				rate_index = 112 + ((Antenna - 1) * 10) + ((unsigned char)HTSetting.field.ShortGI * 160) + ((unsigned char)MCS);	\
			} else if (HTSetting.field.BW == BW_40) {	\
				rate_index = 152 + ((Antenna - 1) * 10) + ((unsigned char)HTSetting.field.ShortGI * 160) + ((unsigned char)MCS);	\
			} else if (HTSetting.field.BW == BW_80) {	\
				rate_index = 192 + ((Antenna - 1) * 10) + ((unsigned char)HTSetting.field.ShortGI * 160) + ((unsigned char)MCS);	\
			} else if (HTSetting.field.BW == BW_160) {	\
				rate_index = 232 + ((Antenna - 1) * 10) + ((unsigned char)HTSetting.field.ShortGI * 160) + ((unsigned char)MCS);	\
			}	\
		}								\
		else								\
		if (HTSetting.field.MODE >= MODE_HTMIX)				\
		{								\
			MCS = HTSetting.field.MCS;	\
			\
			if ((HTSetting.field.MODE == MODE_HTMIX) || (HTSetting.field.MODE == MODE_HTGREENFIELD))	\
				Antenna = (MCS >> 3) + 1;	\
			\
			/* map back to 1SS MCS , multiply by antenna numbers later */		\
			if (MCS > 7)		\
				MCS %= 8;		\
			\
			rate_index = 16 + ((unsigned char)HTSetting.field.BW * 24) + ((unsigned char)HTSetting.field.ShortGI * 48) + ((unsigned char)MCS);		\
		}								\
		else								\
			if (HTSetting.field.MODE == MODE_OFDM)				\
				rate_index = (unsigned char)(HTSetting.field.MCS) + 4;		\
			else if (HTSetting.field.MODE == MODE_CCK)			\
				rate_index = (unsigned char)(HTSetting.field.MCS);	\
									\
		if (rate_index < 0)						\
			rate_index = 0;						\
										\
		if (rate_index >= rate_count)					\
			rate_index = rate_count-1;				\
		\
		if (HTSetting.field.MODE < MODE_VHT)	\
			value = (_mcstbl[rate_index] * 5) / 10;		\
		else	\
			value =  _mcstbl[rate_index];	\
		\
		if (HTSetting.field.MODE >= MODE_HTMIX && HTSetting.field.MODE < MODE_VHT)	\
		value *= Antenna;	\
	}								\
	return value;		\
}
#else

#define FN_GETRATE(_fn_, _st_, _if, _mcstbl)						\
_fn_(_st_ HTSetting)							\
{									\
	int rate_count = sizeof(_mcstbl)/sizeof(int);	\
	int rate_index = 0;						\
	int value = 0;	\
									\
	if (HTSetting.field.MODE >= MODE_VHT)				\
	{								\
		if (HTSetting.field.BW == BW_20) {			\
			rate_index = 108 +				\
			((unsigned char)HTSetting.field.ShortGI * 29) +	\
			((unsigned char)HTSetting.field.MCS);		\
		}							\
		else if (HTSetting.field.BW == BW_40) {			\
			rate_index = 117 +				\
			((unsigned char)HTSetting.field.ShortGI * 29) +	\
			((unsigned char)HTSetting.field.MCS);		\
		}							\
		else if (HTSetting.field.BW == BW_80) {			\
			rate_index = 127 +				\
			((unsigned char)HTSetting.field.ShortGI * 29) +	\
			((unsigned char)HTSetting.field.MCS);		\
		}							\
	}								\
	else								\
	if (HTSetting.field.MODE >= MODE_HTMIX)				\
	{								\
		rate_index = 12 + ((unsigned char)HTSetting.field.BW *24) + ((unsigned char)HTSetting.field.ShortGI *48) + ((unsigned char)HTSetting.field.MCS);	\
	}								\
	else								\
	if (HTSetting.field.MODE == MODE_OFDM)				\
		rate_index = (unsigned char)(HTSetting.field.MCS) + 4;	\
	else if (HTSetting.field.MODE == MODE_CCK)			\
		rate_index = (unsigned char)(HTSetting.field.MCS);	\
									\
	if (rate_index < 0)						\
		rate_index = 0;						\
									\
	if (rate_index >= rate_count)					\
		rate_index = rate_count-1;				\
	\
	if (HTSetting.field.MODE != MODE_VHT)	\
		value = (_mcstbl[rate_index] * 5) / 10;	\
	else	\
		value =  _mcstbl[rate_index];	\
	return value;		\
}

#endif


#if defined(RTCONFIG_HAS_5G)
#if defined(RTCONFIG_WLMODULE_MT7663E_AP) || defined(RTCONFIG_WLMODULE_MT7629_AP) || defined(RTCONFIG_WLMODULE_MT7622_AP)
int FN_GETRATE(getRate,      MACHTTRANSMIT_SETTING_for_5G, 1, MCSMappingRateTable_5G)		//getRate   (MACHTTRANSMIT_SETTING_for_5G)
#else
int FN_GETRATE(getRate,      MACHTTRANSMIT_SETTING_for_5G, 1, MCSMappingRateTable)		//getRate   (MACHTTRANSMIT_SETTING_for_5G)
#endif
#endif	/* RTCONFIG_HAS_5G */
int FN_GETRATE(getRate_2g,   MACHTTRANSMIT_SETTING_for_2G, 0, MCSMappingRateTable)		//getRate_2g(MACHTTRANSMIT_SETTING_for_2G)



#ifdef RTCONFIG_AMAS
double get_wifi_maxpower(int band_type)
{
	return 0;
} 
double get_wifi_5G_maxpower()
{
	return 0;
}
double get_wifi_5GH_maxpower()
{
	return 0;
}
double get_wifi_6G_maxpower()
{
	return 0;
}
#endif

#ifdef RTCONFIG_MULTILAN_CFG
void apg_switch_vlan_set(int vid, unsigned int default_portmask, unsigned int trunk_portmask, unsigned int access_portmask)
{
	if (__apg_switch_vlan_set)
		__apg_switch_vlan_set(vid, default_portmask, trunk_portmask, access_portmask);
}

void apg_switch_vlan_unset(int vid, unsigned int portmask)
{
	if (__apg_switch_vlan_unset)
		__apg_switch_vlan_unset(vid, portmask);
}

void apg_switch_isolation(int enable, unsigned int portmask)
{
	if (__apg_switch_isolation)
		__apg_switch_isolation(enable, portmask);
}
#endif /* RTCONFIG_MULTILAN_CFG */
