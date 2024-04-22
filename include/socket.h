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

struct DS18B20_DATA;



typedef struct socket_s
{
	int			conn_fd;
	char		host[64];
	int			cli_port;
} socket_t;
//socket_t			so;
//socket_t			*my_socket = &so;

int internet_connect(socket_t *my_socket);


int internet_write(struct DS18B20_DATA data, char *snd_buf,socket_t *my_socket);


int internet_read(socket_t *my_socket);


int extract_data(char *snd_buf,socket_t *my_socket);
#endif   /*  ----- #ifndef _SOCKET_H_  ----- */ 
