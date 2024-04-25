/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  packet.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(16/04/24)
 *         Author:  LiYi <1751425323@qq.com>
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


int	get_name(char buf2[1024],size_t buf2_size)
{
	int				fd2;
	char			w2_path[64] = "/sys/bus/w1/devices/";
	char			chip_sn1[256];
	DIR				*dirp1 = NULL;
	struct dirent	*direntp1 = NULL;
	int				found1 = 0;


	dirp1 = opendir(w2_path);
	if( !dirp1 )
	{
		log_error("Open the floder failure : %s\n",strerror(errno));
		return -1;
	}

	while( NULL != (direntp1 = readdir(dirp1)))
	{
		if(strstr(direntp1 -> d_name,"28-"))
		{
			strncpy(chip_sn1, direntp1->d_name, sizeof(chip_sn1));
			found1 = 1;
		}
	}

	closedir(dirp1);


	if(!found1)
	{
		log_error("Can not find name chipset\n");
		return -2;
	}


	strncat(w2_path, chip_sn1,sizeof(w2_path)-strlen(w2_path));
	strncat(w2_path, "/name",sizeof(w2_path)-strlen(w2_path));


	fd2 = open(w2_path, O_RDONLY);
	if( fd2 < 0 )
	{
		log_error("Open the file about name failure : %s\n",strerror(errno));
		return -3;
	}


	memset(buf2, 0, buf2_size);
	if( read(fd2, buf2, buf2_size) < 0 )
	{
		log_error("get devices number failure : %s\n",strerror(errno));
		return -4;
	}

	return 0;
}


char *get_time(char *now_time)
{
	time_t			timer;
	struct tm		*Now = NULL;


	setenv("TZ", "Asia/Shanghai", 1);
	tzset();
	time( &timer );
	Now = localtime( &timer );


	if( Now == NULL)
	{
		log_error("localtime() Error: %s\n",strerror(errno));
		return NULL;
	}


	now_time = asctime(Now);


	if(now_time == NULL)
	{
		log_error("asctime() Error : %s\n",strerror(errno));
		return NULL;
	}
	printf("In the get time now_time:%s\n",now_time);
	return now_time;
}















