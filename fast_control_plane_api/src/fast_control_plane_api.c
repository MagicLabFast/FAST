/*
 ============================================================================
 Name        : fast_control_plane_api.c
 Author      : frank.xiongzz
 Version     : 1.0
 Copyright   : frank.xiongzz@qq.com
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <libnet.h>
#include "fast_api.h"
#include "nmac_api.h"


struct sw_flow* hw_data() {
	u_int32_t f_addr = 0x14000000;
	int num = 64;
	u_char *packet = nmac_rd(f_addr, num);
	if (packet == NULL) {
		printf("read failed\n");
		return NULL;
	}
	else {
		struct libnet_ipv4_hdr *ip_headr = (struct libnet_ipv4_hdr*) (packet + 14);
		int read_data_len = ntohs(ip_headr->ip_len) - 20 - 10;
		u_int32_t *read_data = (u_int32_t*) malloc(read_data_len / 4 * sizeof(u_int32_t));
		u_int32_t *read_data_tmp = (u_int32_t*) malloc(read_data_len / 4 * sizeof(u_int32_t));
		memcpy(read_data_tmp, packet + 44, read_data_len);
		int i;
		for (i = 0; i < read_data_len / 4; i++) {
			read_data[i] = ntohl(read_data_tmp[i]);
		}
		struct sw_flow *flow = (struct sw_flow*)read_data_tmp;
		return flow;
	}
}


int hw_properties(int oxm_ofb_match_fields[40], int ofp_action_type[28], int *capacity) {
	/* device connect to netmagic */
	char *dev = "eth0";

	if (nmac_ini(dev) == 1) {
		if (nmac_con() == 0) {
			printf("cant connect to netmagic\n");
			return 1;
		} else {
			/* static */
			printf("netmagic connected!\n");
			int oomf[40] = { 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			memcpy(oxm_ofb_match_fields, oomf, sizeof(oxm_ofb_match_fields));
			int oat[28] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0 };
			memcpy(ofp_action_type , oat , sizeof(ofp_action_type));

			*capacity = 16;
		}
	}
	return 0;
}

int add_flow_entry(struct sw_flow *flow, int index) {
	u_int32_t f_addr = 0x14000000;
	u_int32_t addr = f_addr + index * 4;
	u_int32_t *data = (u_int32_t*) flow;
	if (nmac_wr(addr, 4, data) == 1) {
		return 0;
	} else {
		return 1;
	}
}

int delete_flow_entry(int index){
	u_int32_t f_addr = 0x14000000;
	u_int32_t addr = f_addr + index * 4;
	u_int32_t data[4];
	memset(data, 0, sizeof(data));
	if (nmac_wr(addr, 4, data) == 1) {
		return 0;
	} else {
		return 1;
	}
}

int clear_hw_data() {
	u_int32_t f_addr = 0x14000000;
	u_int32_t data[64];
	memset(data, 0, sizeof(data));
	if (nmac_wr(f_addr, 64, data) == 1) {
		return 0;
	} else {
		return 1;
	}
}
