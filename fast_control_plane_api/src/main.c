/*
 * main.c
 *
 *  Created on: Jan 12, 2016
 *      Author: xiong
 */

#include "fast_api.h"
#include "nmac_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main() {
	int oxm_ofb_match_fields[40];
	int ofp_action_type[28];
	int capacity;

	if (hw_properties(oxm_ofb_match_fields, ofp_action_type, &capacity) == 0) {
		printf("init suc\n");
	}


	u_int8_t dst[6] = { 0x68, 0xf7, 0x28, 0xd9, 0xbf, 0x71 };
	u_int8_t src[6] = { 0x68, 0xf7, 0x28, 0xd9, 0xbf, 0x72 };
	struct sw_flow *flow = malloc(sizeof(struct sw_flow));
	flow->valid = 1;
	flow->input_port = 1;
	flow->action = 3;
	flow->frame_type = htons(0x0800);
	memcpy(flow->src_mac, src , sizeof(src));
	memcpy(flow->dst_mac, dst , sizeof(dst));
	if ( add_flow_entry(flow, 1) == 0){
		printf("add succ\n");
	}
	free(flow);


	u_int8_t src1[6] = { 0x68, 0xf7, 0x28, 0xd9, 0xbf, 0x71 };
	u_int8_t dst1[6] = { 0x68, 0xf7, 0x28, 0xd9, 0xbf, 0x72 };
	struct sw_flow *flow1 = malloc(sizeof(struct sw_flow));
	flow1->valid = 1;
	flow1->input_port = 3;
	flow1->action = 1;
	flow1->frame_type = htons(0x0800);
	memcpy(flow->src_mac, src1 , sizeof(src));
	memcpy(flow->dst_mac, dst1 , sizeof(dst));
	if ( add_flow_entry(flow1, 2) == 0){
		printf("add succ\n");
	}
	free(flow1);

	return 0;
}
