/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  temp.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/24 13:05:42"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "temp.h"

float get_temperature(float *temp)
{


	int					fd;
	char				buf[1024];
	char				*ptr;
	DIR					*dirp = NULL;
	char				w1_path[64] = "/sys/bus/w1/devices/";
	char				ds18b20_path[164];
	struct dirent		*direntp = NULL;
	char				chip_sn[32];
	int					found = 0;


	dirp = opendir(w1_path);
	if( !dirp )
	{
		printf("Open the floder failure : %s\n",strerror(errno));
		return -1;
	}

	while( NULL != (direntp = readdir(dirp)) )
	{

		if( strstr(direntp -> d_name,"28-") )
		{
			strncpy(chip_sn, direntp -> d_name, sizeof(chip_sn));
			found = 1;
		}
	}
	closedir(dirp);

	if( !found )
	{
		printf("can not find ds18b20 chipset\n");
		return -2;
	}


	/* 获取全路径到ds18b20_path中去*/
	strncat(w1_path, chip_sn, sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path, "/w1_slave", sizeof(w1_path)-strlen(w1_path));
	fd = open(w1_path, O_RDONLY);
	if( fd < 0 )
	{
		printf("Open the file failure : %s\n",strerror(errno));
		return -1;
	}
	memset(buf, 0, sizeof(buf));


	if(read(fd, buf, sizeof(buf)) < 0 )
	{
		printf("Read data from fd = %d failure : %s\n",fd,strerror(errno));
		return -2;
	}
	ptr = strstr(buf, "t=");

	/* 判断buf是否为空*/

	if( NULL == ptr )
	{
		printf("Can not find t = String\n");
		return -1;
	}
	ptr += 2;
	*temp = atof(ptr)/1000;
	return 0;
}


