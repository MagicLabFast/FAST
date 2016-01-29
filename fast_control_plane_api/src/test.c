/*
 * test.c
 *
 *  Created on: Jan 12, 2016
 *      Author: xiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#pragma pack(push)
#pragma pack(1)
struct sw_flow{
	u_int8_t valid;
	u_int8_t action:4;
	u_int8_t input_port:4;
	u_int16_t frame_type;
	u_int8_t src_mac[6];
	u_int8_t dst_mac[6];
};
#pragma pack(pop)

//void point(int a[2],int b[2]){
//	int c[2] = {1,2};
//	memcpy(a,c,sizeof(a));
//	b[0] = 1;
//	b[1] = 2;
//}

int main() {
	u_int8_t dst[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
	u_int8_t src[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
	struct sw_flow *flow = malloc(sizeof(struct sw_flow));
	flow->valid = 1;
	flow->input_port = 2;
	flow->action = 3;
	flow->frame_type = htons(4);

	printf("%08x\n", flow[0]);
	u_int32_t temp[4];
	memcpy(temp, flow , 4);
	printf("%08x\n", htonl(temp[0]));

	memcpy(flow->src_mac, src , sizeof(src));
	memcpy(flow->dst_mac, dst , sizeof(dst));

	memcpy(temp + 1,flow->src_mac, sizeof(src));
	printf("%08x\n", htonl(temp[1]));
//	int a[2] = { 3, 4};
//	int b[2] = { 5, 6};
//	point(a,b);
//	printf("%d,%d\n", a[1] , b[0]);
	return 0;

}
