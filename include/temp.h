/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  temp.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/24 13:16:43"
 *                 
 ********************************************************************************/
#ifndef			 _DS18B20_H_
#define			_DS18B20_H_



struct DS18B20_DATA
{
	char            d_time[64];
	float           d_temp;
	char            d_name[64];
};


/* Description : get the device temperature */


extern float get_temperature(float *temp);

#endif		/*  ----- #ifndef _DS18B20_H_  ----- */ 
