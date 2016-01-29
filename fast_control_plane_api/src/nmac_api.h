/*
 * nmac_api.h
 *
 *  Created on: Jan 11, 2016
 *      Author: frank.xiongzz
 */
#include <stdio.h>
#include <sys/types.h>
#ifndef NMAC_API_H_
#define NMAC_API_H_

/* if initialed, return 1. if not return 0*/
int nmac_ini(char *dev);

/* if connected, return 1. if not return 0*/
int nmac_con();

/* if write success, return 1. if not return 0
 * addr: vitual address
 * num: the number of date write to RAM
 * data: the data write to RAM
 * */
int nmac_wr(u_int32_t addr, int num, u_int32_t *data);

/* if read success, return 1. if not return 0
 * addr: vitual address
 * num: the number of date read RAM
 * */
u_char *nmac_rd(u_int32_t addr, int num);

#endif /* NMAC_API_H_ */
