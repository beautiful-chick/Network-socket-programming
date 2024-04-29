/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  packet.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(16/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "16/04/24 22:42:33"
 *                 
 ********************************************************************************/
#ifndef		_PACKET_H_
#define		_PACKET_H_

#define DEVID_LEN          8

#include"temp.h"

/* Description : get the devices name */

int get_name(char buf2[1024], size_t buf2_size);

/* Description : get the now time */

int get_time(char *now_time);



extern int packet_data(data_t *data, uint8_t * pack_buf, int size);

typedef int (*pack_proc_t)(data_t *data, uint8_t *pack_buf, int size);




#endif   /*  ----- #ifndef _PACKET_H_  ----- */ 
