/*********************************************************************************
 *      Copyright:  (C) 2024 LiZhao<3299832490@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  packet.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(16/04/24)
 *         Author:  LiZhao <3299832490@qq.com>
 *      ChangeLog:  1, Release initial version on "16/04/24 11:19:03"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sqlite3.h>
#include"temp.h"

#include "packet.h"
#include"logger.h"


int get_name(char *nameid, int size, int sn)
{
	if( !nameid )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	memset(nameid, 0, size);
	snprintf(nameid, size, "rpi#%04d", sn);
	return 0;
}


int get_time(struct tm *ptm)
{
	time_t		now;

	if( !ptm )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	now = time(NULL);
	localtime_r(&now, ptm);

	return 0;

}


int get_devid(char *devid, int size, int sn)
{
	if( !devid || size<DEVID_LEN )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	memset(devid, 0, size);
	snprintf(devid, size, "rpi#%04d", sn);
	return 0;
}






int packet_data(data_t *data, uint8_t *pack_buf, int size)
{
	char              	strtime[128] = {'\0'};
	char				*buf = (char *)pack_buf;
	struct tm        	*ptm;

	if( !data || !buf ||size <= 0 )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	ptm = &data->time_str;
	snprintf(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d",
			ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	
	memset(buf, 0, size);
	snprintf(buf, size, "%s,%s,%.2f", data->d_name, strtime, data->d_temp);
	strcpy(strtime, data->d_time);
	return strlen(buf);

}










