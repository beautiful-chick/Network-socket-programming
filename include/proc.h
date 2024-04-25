/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  proc.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(25/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "25/04/24 13:28:25"
 *                 
 ********************************************************************************/

#ifndef __PROC_H_
#define __PROC_H_

typedef struct proc_signal_s
{
	int       		signal;
	unsigned		stop;
}  proc_signal_t;


extern proc_signal_t  g_signal;

extern void proc_default_sighandler(int sig);

extern void install_default_signal(void);

#endif
