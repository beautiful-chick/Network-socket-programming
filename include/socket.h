/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/24 13:50:42"
 *                 
********************************************************************************/
#ifndef  _SOCKET_H_
#define  _SOCKET_H_

#define HOSTNAME_LEN   64

#include"temp.h"



typedef struct socket_s
{
	int			conn_fd;
	char		host[64];
	int			port;
} socket_t;

/* Description :  Initialize network socket related functions */

extern int socket_init(socket_t *sock, char *host, int port);

/* Description : Close network socket related functions */

extern int sock_close(socket_t *sock);

/* Description : Make a network socket connection */

extern int sock_connect(socket_t *sock);

/* Description : Sending data over a network socket */

extern int sock_write(socket_t *sock, char *d_data, int bytes);


#endif   /*  ----- #ifndef _SOCKET_H_  ----- */ 
