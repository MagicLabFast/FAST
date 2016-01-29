/*
 * fast_api.h
 *
 *  Created on: Jan 11, 2016
 *      Author: xiong
 */
#include <sys/types.h>
#ifndef FAST_API_H_
#define FAST_API_H_

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



int hw_properties(int oxm_ofb_match_fields[40], int ofp_action_type[28], int *capacity);

int add_flow_entry(struct sw_flow *flow, int index);

//int delete_flow_entry(struct sw_flow *flow);
int delete_flow_entry(int index);

int clear_hw_data();

struct sw_flow* hw_data();

#endif /* FAST_API_H_ */
