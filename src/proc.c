/*********************************************************************************
 *      Copyright:  (C) 2024 LiZhao<3299832490@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  proc.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(25/04/24)
 *         Author:  LiZhao <3299832490@qq.com>
 *      ChangeLog:  1, Release initial version on "25/04/24 13:03:55"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>


#include "proc.h"
#include "logger.h"


proc_signal_t  g_signal = {0};

void proc_default_sighandler(int sig)
{


	switch(sig)
	{
		case SIGINT:
			log_warn("SIGINT - stopping\n");
			g_signal.stop = 1;
			break;

		case SIGTERM:
			log_warn("SIGTERM - stopping\n");
			g_signal.stop = 1;
			break;

		case SIGSEGV:
			log_warn("SIGSEGV - stopping\n");
			g_signal.stop = 1;
			break;

		case SIGPIPE:
			log_warn("SIGPIPE - warnning\n");
			g_signal.stop = 1;
			break;

		default:
			break;
	}
}



void install_default_signal(void)
{
	struct sigaction sigact,sigign;


	log_info("Start to install signal\n");

	/*Start to initialize signal structure*/

	sigemptyset(&sigact.sa_mask);
	sigign.sa_flags = 0;
	sigign.sa_handler = proc_default_sighandler;

	/* Set up ignore signal*/
	sigemptyset(&sigign.sa_mask);
	sigign.sa_flags = 0;
	sigign.sa_handler = SIG_IGN;
	sigaction(SIGTERM, &sigact, 0);

	/* catch terminate signal "kill"*/
	sigaction(SIGINT, &sigact, 0);

	/* catch segmentation faults */ 

	sigaction(SIGSEGV, &sigact, 0);

	/* catch broken pipe */

	sigaction(SIGPIPE, &sigact, 0);

}
