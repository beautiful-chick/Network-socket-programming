/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sock_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/24 20:39:19"
 *                 
********************************************************************************/
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<dirent.h>
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<getopt.h>
#include<libgen.h>
#include<sqlite3.h>
#include<netinet/tcp.h>


#include "socket.h"
#include "sock_sqlite.h"
#include "temp.h"
#include "packet.h"

extern struct DS18B20_DATA data;


socket_t 			my_socket;
sqlite3				*db = NULL;



static inline void print_usage(char *progname)
{
	printf("Usage: %s [OPTION]...\n", progname);
	printf("%s is a socket server program,which used to verify client and echo back string from it\n", progname);
	printf("\nMandatory arguments to long options are mandatory for short options too:\n");
	printf("-i[IP Address]\n");
	printf("-p[Port]\n");
	printf("-t[time]\n");
	return ;
}


int main(int argc, char **argv)
{


	int					timeout;
	int					opt;
	char				*progname = NULL;
	float 				dev_temp;
	char				dev_name[64];
	char				*dev_time;
	int					rv = -1;
	int					rc;
	float				temp;
	char				name_buf[64];
	int					flag;
	time_t				now_time,last_time;
	struct tcp_info		optval;
	socklen_t 			optlen = sizeof(optval);
	int					ret;
	int					rv1;
	int					row;
	double				time_diff;
	char     	        snd_buf[1024] = {0};


	progname = (char *)basename( argv[0] );
	struct option		long_options[] = 
	{
		{"port", required_argument, NULL, 'p'},
		{"IP Address", required_argument, NULL, 'i'},
		{"time", required_argument, NULL, 't'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}

	};


	while( (opt = getopt_long(argc,argv,"p:i:t:h",long_options,NULL)) != -1 )
	{
		switch(opt)
		{
			case 'p':
				my_socket.cli_port = atoi(optarg);
				break;
			case 'i':
				strcpy(my_socket.host, optarg);
				break;
			case 't':
				timeout = atoi(optarg);
				break;
			case 'h':
				print_usage(progname);
				return 0;
			default:
				break;
		}
	}


	dev_sqlite3();

	last_time = time(NULL);

	while(1)
	{
		flag = 0;
		now_time = time(NULL);
		time_diff = difftime(now_time,last_time);

		if( time_diff >= timeout)
		{
			printf("------------------Start get data--------------------\n");
			if( get_temperature(&dev_temp) == 0 )
			{
				printf("Temperature:%.2f\n",dev_temp);
			}
			else
			{
				printf("Temperature error:%s\n",strerror(errno));
			}
			if( (get_name(dev_name,sizeof(dev_name))) != 0 )
			{
				printf("Get name error:%s\n",strerror(errno));
			}
			else
			{
				printf("dev_name:%s\n",dev_name);
			}
			dev_time = get_time(NULL);
			if( dev_time  != NULL )
			{
				printf("Current time is:%s\n",dev_time);
			}
			else
			{
				printf("Get current time failure:%s\n",strerror(errno));
			}


			int m = assign_data(dev_name,dev_time,dev_temp,&data);
			if( m != 0)
			{
				printf("assign_data error\n");
			}
			else
			{
				printf("assign_data ok\n");
			}

			last_time = now_time;
			flag = 1;

			ret = getsockopt(my_socket.conn_fd, IPPROTO_TCP, TCP_INFO, &optval, &optlen);
		}
		sleep(2);


		ret = getsockopt(my_socket.conn_fd, IPPROTO_TCP, TCP_INFO, &optval, &optlen);
		printf("ret:%d\n",ret);

		if( ret != 0 )
		{
			rv = internet_connect(&my_socket);


			if( rv < 0 )
			{
				close(my_socket.conn_fd);

				if( flag == 1 )
				{
					insert_data(data);
					continue;

				}
			}
		}


		if( flag == 1 )
		{
			/* 发送当前采样的数据*/
			rv1 = internet_write(data, snd_buf,&my_socket);
			printf("Send the data ok\n");
			internet_read(&my_socket);


			/* 发送失败，将数据再存到数据库中*/
			if( rv1 < 0 )
			{
				dev_sqlite3(data);
				continue;
			}
		}

		/* 判断数据库中是否有数据*/
		row = get_row();
		if(row != 0)
		{
			/* 提取,发送一条数据*/
			extract_data(snd_buf,&my_socket);
			/* 删除一条数据*/
			del_database(snd_buf);

		}
	}
	sqlite3_close(db);
	return 0;
}
