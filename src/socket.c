/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/24 13:41:41"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include <stdlib.h>
#include <errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include <unistd.h>
#include<sqlite3.h>

#include"temp.h"
#include"socket.h"
#include"sock_sqlite.h"




int sock_connect(socket_t *my_socket)
{
	struct sockaddr_in		serv_addr;

	my_socket->conn_fd = socket(AF_INET,SOCK_STREAM,0);	

	if( my_socket->conn_fd < 0 )
	{
		printf("Create socket failure : %s\n",strerror(errno));
		close(my_socket->conn_fd);
		return -1;
	}
	printf("create socket ok\n");


	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(my_socket->cli_port);
	if( inet_aton(my_socket->host, &serv_addr.sin_addr) == 0 )
	{
		printf("Invalid IP address: %s\n", my_socket->host);
		close(my_socket->conn_fd);
		return -1;
	}


	if( connect(my_socket->conn_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Connect to server[%s:%d] failure : %s\n",my_socket->host, my_socket->cli_port, strerror(errno));
		return -1;
	}

	return 0;
}




int sock_write(socket_t *my_socket,data_t data, char *snd_buf)
{
	snprintf(snd_buf, 2048, "%s,%s,%.2f", data.d_time, data.d_name, data.d_temp);
	printf("internet_write snd_buf:%s\n", snd_buf);


	if( write(my_socket->conn_fd, snd_buf, strlen(snd_buf)) < 0 )
	{
		printf("Wirte data to server[%s:%d] failure : %s\n", my_socket->host[64], my_socket->cli_port, strerror(errno));	
	}
	return 0;
}





int send_data(char *snd_buf,socket_t *my_socket)
{
	
	write(my_socket->conn_fd, snd_buf, strlen(snd_buf));
	return 0;
}

