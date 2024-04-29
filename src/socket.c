/*********************************************************************************
 *      Copyright:  (C) 2024 LiZhao<3299832490@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiZhao <3299832490@qq.com>
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
#include <netdb.h>


#include"temp.h"
#include"socket.h"
#include"sock_sqlite.h"
#include"logger.h"



int socket_init(socket_t *my_socket, char *host, int port)
{
	my_socket->conn_fd = -1;
	my_socket->cli_port = port;

	return 0;
}


int sock_close(socket_t *my_socket)
{

	if( my_socket->conn_fd > 0)
	{
		close(my_socket->conn_fd);
		my_socket->conn_fd = -1;

	}
	return 0;
}






int sock_connect(socket_t *my_socket)
{
	int						rv = 0;
	int						sockfd = 0;
	char					service[20];
	struct addrinfo			hints,*rp;
	struct addrinfo			*res = NULL;
	struct sockaddr_in		serv_addr;
	int						len = sizeof(serv_addr);
	struct in_addr      	inaddr;


	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;

	if( inet_aton(my_socket->host, &inaddr) )
	{
		hints.ai_flags |= AI_NUMERICHOST;
	}

	snprintf(service, sizeof(service), "%d", my_socket->cli_port);
	if( (rv = getaddrinfo(my_socket->host, service, &hints, &res)) )
	{
		log_error("getaddrinfo() parser [%s:%s] failed:%s\n",my_socket->host, service, gai_strerror(rv));
		return -3;
	}

	for(rp = res; rp != NULL; rp = rp->ai_next)
	{
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if( sockfd < 0 )
		{
			log_error("socket create error:%s\n",strerror(errno));
			rv = -3;
			continue;
		}

		rv = connect(sockfd, rp->ai_addr, len);

		if( 0 == rv )
		{
			my_socket->conn_fd = sockfd;
			log_info("connect to  server [%s:%d] successfully\n", my_socket->host, my_socket->cli_port);
			break;
		}

		else
		{
			close(sockfd);
			continue;
		}
	}

	freeaddrinfo(res);
	return rv;

}




int sock_write(socket_t *my_socket,data_t data, char *d_data, int bytes)
{
	int			rv = 0;
	int			i = 0;
	int			left_bytes = bytes;
	if( !my_socket || bytes<=0 || !d_data )
		return -1;


	while( left_bytes > 0 )
	{
		rv = write(my_socket->conn_fd, &d_data[i], left_bytes);

		if( rv < 0 )
		{
			log_info("Socket write failure : %s\n", strerror(errno));
			sock_close(my_socket);
			return -2;
		}


		else if( rv == left_bytes )
		{
			log_info("socket send %d bytes data over\n", bytes);
			return 0;
		}

		else
		{
			i += rv;
			left_bytes -= rv;
			continue;

		}
	}
	return 0;
}







