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




int sock_write(socket_t *my_socket,data_t data)
{
	char		snd_buf[64];
	snprintf(snd_buf, 64, "%s,%s,%.2f", data.d_time, data.d_name, data.d_temp);
	log_debug("internet_write snd_buf:%s\n", snd_buf);


	if( write(my_socket->conn_fd, snd_buf, sizeof(snd_buf)) < 0 )
	{
		log_error("Wirte data to server[%s:%d] failure : %s\n", my_socket->host[64], my_socket->cli_port, strerror(errno));	
		sock_close(my_socket);
	}
	return 0;
}





int send_data(char *snd_buf,socket_t *my_socket)
{

	write(my_socket->conn_fd, snd_buf, strlen(snd_buf));
	return 0;
}

