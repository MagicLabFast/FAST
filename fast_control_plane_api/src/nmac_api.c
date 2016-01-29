/*
 ============================================================================
 Name        : nmac_api.c
 Author      : frank.xiongzz
 Version     :
 Copyright   : frank.xiongzz@qq.com
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <libnet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include "nmac_api.h"

#define ETH_LEN 14
#define IP_LEN 20


#pragma pack(push)
#pragma pack(1)
struct nmac_hdr{
	u_int8_t count;
	u_int8_t reserve8_A;
	u_int16_t seq;
	u_int16_t reserve16_B;
 	u_int8_t nmac_type;
	u_int16_t parameter;
	u_int8_t reserve8_C;
};
#pragma pack(pop)

enum NMAC_PKT_TYPE {
	NMAC_CON = 0x01,
	NMAC_RD = 0x03,
	NMAC_WR = 0x04,
	NMAC_RD_REP = 0x05,
	NMAC_WR_REP = 0x06,
};

struct libnet_ether_addr *my_mac_addr;
u_int32_t my_ip_addr, des_ip_addr;

libnet_t *libnet_handle;
pcap_t *pcap_handle;
u_int8_t netmagic_mac[6] = { 0x88, 0x88, 0x88, 0x88, 0x88, 0x88 };
int write_seq = 0;
int read_seq = 0;
int nmac_connected_flag = 0;
int read_success_flag = 0;
int write_success_flag = 0;
u_char *globe_pkt;

void parsing_callback(u_char *useless, const struct pcap_pkthdr* pkthdr,
		const u_char* packet);


int nmac_ini(char *dev){
	bpf_u_int32 net_ip;
	bpf_u_int32 net_mask;
	struct bpf_program bpf_filter;
	char bpf_filter_string[50] = "ip proto 253 and ip dst ";
	char errbuf[255];

	pcap_lookupnet(dev, &net_ip, &net_mask, errbuf);
	pcap_handle = pcap_open_live(dev, BUFSIZ, 0, -1, errbuf);
	if (pcap_handle == NULL) {
		printf("pcap error!pcap_open_live(): %s\n", errbuf);
		return 0;
	}
	libnet_handle = libnet_init(LIBNET_LINK, dev, errbuf);
	if (libnet_handle == NULL) {
		printf("libnet_error!libnet_init(): %s\n", errbuf);
		return 0;
	}
	my_mac_addr = libnet_get_hwaddr(libnet_handle);
	my_ip_addr = libnet_get_ipaddr4(libnet_handle);
	des_ip_addr = libnet_name2addr4(libnet_handle, "136.136.136.136",
	LIBNET_DONT_RESOLVE);

	char *my_ip = libnet_addr2name4(my_ip_addr, LIBNET_DONT_RESOLVE);
	strcat(bpf_filter_string,my_ip);

	pcap_compile(pcap_handle, &bpf_filter, bpf_filter_string, 0, net_ip);
	pcap_setfilter(pcap_handle, &bpf_filter);

	return 1;
}

int nmac_con(){
	u_char *payload;
	payload = (u_char*) malloc(1480 * sizeof(u_char));
	struct timeval start, end;
	libnet_ptag_t ip_protocol_tag = 0;
	libnet_ptag_t ether_protocol_tag = 0;
	u_int16_t payload_size;
	//连接NetMagic
	u_int16_t nmac_seq = 0;
	struct nmac_hdr nmac_head = { 1, 0, htons(0), 0, NMAC_CON, htons(1), 0 };
	memcpy(payload, &nmac_head, sizeof(struct nmac_hdr));
	payload_size = sizeof(struct nmac_hdr);
	ip_protocol_tag = libnet_build_ipv4(
			LIBNET_IPV4_H + payload_size,
			0,
			nmac_seq,
			0,
			64,
			253,
			0,
			my_ip_addr,
			des_ip_addr,
			payload,
			payload_size,
			libnet_handle,
			ip_protocol_tag);
	ether_protocol_tag = libnet_build_ethernet(
			netmagic_mac,
			my_mac_addr->ether_addr_octet,
			ETHERTYPE_IP,
			NULL,
			0,
			libnet_handle,
			ether_protocol_tag);
	payload_size = libnet_write(libnet_handle);
	libnet_clear_packet(libnet_handle);
	gettimeofday(&start, NULL);
	while (1) {
		gettimeofday(&end, NULL);
		pcap_dispatch(pcap_handle, 1, parsing_callback, NULL);
		if ((float) ((1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)) / 1000000) > 2.5) {
			printf("connect timeout!\n");
			return 0;
			break;
		}
		if (nmac_connected_flag == 1) {
			return 1;
			break;
		}
	}
}

int nmac_wr(u_int32_t addr, int num, u_int32_t *data){
	u_char *payload;
	payload = (u_char*) malloc(1480 * sizeof(u_char));
	write_seq++;
	struct timeval start, end;
	libnet_ptag_t ip_protocol_tag = 0;
	libnet_ptag_t ether_protocol_tag = 0;
	u_int32_t w_addr;
	w_addr = htonl(addr);
	int i;
	u_int32_t * data_net;
	data_net = (u_int32_t*) malloc(num * sizeof(u_int32_t));
	for (i = 0; i < num; i++){
		data_net[i] = data[i];
	}
	u_int16_t payload_size;
	struct nmac_hdr write_request = { 1, 0, htons(write_seq), 0, NMAC_WR, htons(num), 0 };
	memcpy(payload, &write_request, sizeof(struct nmac_hdr));
	memcpy(payload + sizeof(struct nmac_hdr), &w_addr, sizeof(u_int32_t));
	memcpy(payload + sizeof(struct nmac_hdr) + sizeof(u_int32_t), data_net, num * sizeof(u_int32_t));
	payload_size = sizeof(struct nmac_hdr) + sizeof(u_int32_t) + num * sizeof(u_int32_t);

	ip_protocol_tag = libnet_build_ipv4(
			LIBNET_IPV4_H + payload_size,
			0,
			write_seq,
			0,
			64,
			253,
			0,
			my_ip_addr,
			des_ip_addr,
			payload,
			payload_size,
			libnet_handle,
			ip_protocol_tag);
	ether_protocol_tag = libnet_build_ethernet(
			netmagic_mac,
			my_mac_addr->ether_addr_octet,
			ETHERTYPE_IP,
			NULL,
			0,
			libnet_handle,
			ether_protocol_tag);
	libnet_write(libnet_handle);
	libnet_clear_packet(libnet_handle);
	gettimeofday(&start, NULL);

	while (1) {
		gettimeofday(&end, NULL);
		pcap_dispatch(pcap_handle, 1, parsing_callback, NULL);
		if ((float) ((1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)) / 1000000) > 2.5) {
			printf("addr:%02x write failed!\n", addr);
			return 0;
			break;
		}
		if (write_success_flag == 1) {
			return 1;
			break;
		}
	}
}

u_char *nmac_rd(u_int32_t addr,int num){
	u_char *payload;
	payload = (u_char*) malloc(1480 * sizeof(u_char));
	read_seq++;
	struct timeval start, end;
	libnet_ptag_t ip_protocol_tag = 0;
	libnet_ptag_t ether_protocol_tag = 0;
	u_int16_t payload_size;
	u_int32_t w_addr;
	w_addr = htonl(addr);
	struct nmac_hdr read_request = { 1, 0, htons(read_seq), 0, NMAC_RD, htons(num), 0 };
	memcpy(payload, &read_request, sizeof(struct nmac_hdr));
	memcpy(payload + sizeof(struct nmac_hdr), &w_addr,sizeof(u_int32_t));
	payload_size = sizeof(struct nmac_hdr) + sizeof(u_int32_t);
	ip_protocol_tag = libnet_build_ipv4(
			LIBNET_IPV4_H + payload_size,
			0,
			read_seq,
			0,
			64,
			253,
			0,
			my_ip_addr,
			des_ip_addr,
			payload,
			payload_size,
			libnet_handle,
			ip_protocol_tag
			);
	ether_protocol_tag = libnet_build_ethernet(
			netmagic_mac,
			my_mac_addr->ether_addr_octet,
			ETHERTYPE_IP,
			NULL,
			0,
			libnet_handle,
			ether_protocol_tag
			);
	payload_size = libnet_write(libnet_handle);
	gettimeofday(&start, NULL);
	libnet_clear_packet(libnet_handle);
	while (1) {
		gettimeofday(&end, NULL);
		pcap_dispatch(pcap_handle, 1, parsing_callback, NULL);
		if ((float) ((1000000 * (end.tv_sec - start.tv_sec)  +  (end.tv_usec - start.tv_usec)) / 1000000) > 2.5) {
			printf("addr:%02x read failed! \n", addr);
			return NULL;
			break;
		}
		if (read_success_flag == 1) {
			read_success_flag = 0;
			return globe_pkt;
			break;
		}
	}
}


void parsing_callback(u_char *useless, const struct pcap_pkthdr* pkthdr, const u_char* packet){
	struct nmac_hdr *Nmac_hdr = (struct nmac_hdr*) (packet + ETH_LEN + IP_LEN);
	switch (Nmac_hdr->nmac_type) {
		//connect response
		case NMAC_CON:
		{
			nmac_connected_flag = 1;
			break;
		}
		//write response
		case NMAC_WR_REP:
		{
			if(write_seq ==ntohs(Nmac_hdr->seq)){
				write_success_flag = 1;
				printf("write success\n");
			}
			break;
		}
		//read response
		case NMAC_RD_REP:
		{
			if (read_seq == ntohs(Nmac_hdr->seq)) {
				read_success_flag = 1;
				printf("read success\n");
				globe_pkt = packet;
/* example of parsing a packet
				struct libnet_ipv4_hdr *ip_headr = (struct libnet_ipv4_hdr*)(packet + 14);
				int read_data_len = ntohs(ip_headr->ip_len) - 20 -10;
				u_int32_t *read_data = (u_int32_t*)malloc(read_data_len/4 * sizeof(u_int32_t));
				u_int32_t *read_data_tmp = (u_int32_t*)malloc(read_data_len/4 * sizeof(u_int32_t));
				memcpy(read_data_tmp,packet+44,read_data_len);
				int i;
				printf("read success:\n");
				for(i=0;i<read_data_len/4;i++){
					read_data[i] = ntohl(read_data_tmp[i]);
					printf("%08x\n",read_data[i]);
				}
*/
			}
			break;
		}
	}
}
