/********************************************************************************
 *      Copyright:  (C) 2024 LiZhao<3299832490@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  temp.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiZhao <3299832490@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/24 13:16:43"
 *                 
 ********************************************************************************/
#ifndef			 _DS18B20_H_
#define			_DS18B20_H_

#include<time.h>

typedef struct ds18b20
{
	struct tm      	time_str;
	float           d_temp;
	char            d_name[32];
	char			d_time[128];
}data_t;


/* Description : get the device temperature */


extern float get_temperature(float *temp);

#endif		/*  ----- #ifndef _DS18B20_H_  ----- */ 
