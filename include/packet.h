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
#include"temp.h"

/* Description : get the devices name */

int get_name(char buf2[1024], size_t buf2_size);

/* Description : get the now time */

char *get_time(char *now_time);

/*Description : Packaging the temperature,device id and now time*/

//int assign_data(char dev_name[64], char *dev_time, float dev_temp,struct data_t *data);

#endif   /*  ----- #ifndef _PACKET_H_  ----- */ 
