/*********************************************************************************
 *      Copyright:  (C) 2024 LiZhao<3299832490@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sock_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiZhao <3299832490@qq.com>
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
#include "logger.h"
#include "proc.h"

int check_sample_time(time_t *last_time,int timeout);

static inline void print_usage(char *progname)
{
	printf("Usage: %s [OPTION]...\n", progname);
	printf("%s is a socket server program,which used to verify client and echo back string from it\n", progname);
	printf("\nMandatory arguments to long options are mandatory for short options too:\n");
	printf("-i[IP Address]\n");
	printf("-p[Port]\n");
	printf("-t[timeout]\n");
	printf("-d[debug]\n");
	return ;
}

static inline void msleep(unsigned long ms)
{
	struct timespec cSleep;
	unsigned long ulTmp;


	cSleep.tv_sec = ms/1000;
	if( cSleep.tv_sec == 0 )
	{
		ulTmp = ms*10000;
		cSleep.tv_nsec = ulTmp*100;
	}
	else
	{
		cSleep.tv_nsec = 0;
	}
	nanosleep(&cSleep, 0);
	return ;
}



int main(int argc, char **argv)
{
	int					timeout;
	struct tm 			Now;

	int					opt;
	char				*progname = NULL;


	int					rv = -1;
	int					rc;

	float				temp;
	char				name_buf[64];
	int					flag;
	double				time_diff;
	time_t				last_time = 0;
	

	socket_t 			my_socket;
	struct tcp_info		optval;
	socklen_t 			optlen = sizeof(optval);
	char     	        snd_buf[1024] = {0};
	char				*send_buf;
	int					ret;


	int					row;
	data_t				data;

	int					pack_bytes = 0;
	char				pack_buf[1024];
	pack_proc_t         pack_proc = packet_data;

	char                *logfile="sock_client.log";
	int                 loglevel=LOG_LEVEL_INFO;
	int                 logsize=10; /*  logfile size max to 10K */ 
	

	progname = (char *)basename(argv[0]);
	struct option long_options[] = 
	{
		{"port", required_argument, NULL, 'p'},
		{"IP Address", required_argument, NULL, 'i'},
		{"timeout", required_argument, NULL, 't'},
		{"help", no_argument, NULL, 'h'},
		{"debug", no_argument, NULL, 'd'},
		{NULL, 0, NULL, 0}

	};


	while( (opt = getopt_long(argc,argv,"p:i:t:dh",long_options,NULL)) != -1 )
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
			case 'd':
				logfile="console";/* set log_name:console*/
				loglevel=LOG_LEVEL_DEBUG;/*set level is record all debug information*/
				break;
			case 'h':
				print_usage(progname);
				return 0;
			default:
				break;
		}
	}


	if( log_open(logfile, loglevel, logsize, THREAD_LOCK_NONE) < 0 )
	{
		fprintf(stderr, "Initial log system failed\n");
		return 1;
	}

	install_default_signal();	
	

	rv = open_sqlite3();
	if( 0 == rv )
	{
		log_debug("Create table ok\n");
	}
	else
	{
		log_error("Create table error:%s\n",strerror(errno));
		return 2;
	}
	
	socket_init(&my_socket);


	while( ! g_signal.stop )
	{
		flag = 0;

		if( check_sample_time(&last_time, timeout) )
		{
			log_debug("------------------Start get data--------------------\n");
			if( get_temperature( &data.d_temp ) == 0 )
			{
				log_info("Temperature:%.2f\n", data.d_temp);
			}
			else
			{
				log_error("Temperature error:%s\n", strerror(errno));
				continue;
			}

			if( (get_name(data.d_name, sizeof(data.d_name), 88)) != 0 )
			{
				log_error("Get name error:%s\n", strerror(errno));
			}
			else
			{
				log_info("name:%s\n", data.d_name);
			}

			get_time(data.d_time);
			if( data.d_time  != NULL )
			{
				log_info("Current time is:%s\n", data.d_time);
			}
			else
			{
				log_error("Get current time failure:%s\n", strerror(errno));
			}

			log_debug("Get the important data ok\n");
			pack_bytes = packet_data(&data, pack_buf, sizeof(pack_buf));
			log_debug("pack_bytes:%d\n", pack_bytes);

			flag = 1;
		}
		

		if(my_socket.conn_fd < 0 )
		{
			sock_connect(&my_socket);
		}
		
		ret = getsockopt(my_socket.conn_fd, IPPROTO_TCP, TCP_INFO, &optval, &optlen);

		if( ret != 0 )
		{
			rv = sock_connect( &my_socket );
			
			if( rv < 0 )
			{
				sock_close(&my_socket);

				if( flag == 1 )
				{
					log_debug("Start to insert_data\n");
					sqlite_insert_data(data);

				}

				continue;
			}
		}


		if( flag == 1 )
		{
			/* 发送当前采样的数据*/
			rv = sock_write(&my_socket, data, pack_buf, pack_bytes);

			/* 发送失败，将数据再存到数据库中*/
			if( rv < 0 )
			{
				sock_close(&my_socket);
				sqlite_insert_data(data);
				continue;
			}

			log_info("Send the data ok\n");
		}

		/* 判断数据库中是否有数据*/
		row = sqlite_get_row();
		if(row != 0)
		{
			/* 提取,发送一条数据*/
			
			rv = sqlite_read_data();
			
			if( 0 == rv )
			{
				log_info("Extract data ok\n");
			}
			else
			{
				log_error("Error:%s\n", strerror(errno));
				continue;
			}
			
			rv = sock_write(&my_socket, data, pack_buf, pack_bytes);
			if( rv < 0 )
			{
				sock_close(&my_socket);

				log_error("Send error\n");
				continue;
			}
			
			/* 删除一条数据*/
			rv = sqlite_del_data(snd_buf);
			if( rv < 0 )
			{
				log_error("delete the data error:%s\n",strerror(errno));
				continue;
			}

		}
	}

	sock_close(&my_socket);
	msleep(5);
	sqlite_close_database();
	log_close();
	return 0;
}




int check_sample_time(time_t *last_time, int timeout)
{
	int 		time_sample = 0;
	time_t		now;
	time(&now);
	if( difftime(now, *last_time) > timeout)
	{
		time_sample = 1;
		*last_time = now;
	}
	return time_sample;

}
