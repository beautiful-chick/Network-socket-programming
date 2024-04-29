/********************************************************************************
 *      Copyright:  (C) 2024 LiZhao<3299832490@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  packet.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(16/04/24)
 *         Author:  LiZhao <3299832490@qq.com>
 *      ChangeLog:  1, Release initial version on "16/04/24 22:42:33"
 *                 
 ********************************************************************************/
#ifndef		_PACKET_H_
#define		_PACKET_H_

#define DEVID_LEN          8

#include"temp.h"

/* Description : get the devices name */

extern int get_name(char *nameid, int size, int sn);

/* Description : get the now time */

extern int get_time(char *now_time);

/* Description : Package the data and calculate the data size */

extern int packet_data(data_t *data, uint8_t * pack_buf, int size);


typedef int (*pack_proc_t)(data_t *data, uint8_t *pack_buf, int size);




#endif   /*  ----- #ifndef _PACKET_H_  ----- */ 
