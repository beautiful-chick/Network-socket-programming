/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sock_sqlite.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/24 14:26:52"
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
#include<arpa/inet.h>
#include<netinet/in.h>
#include<getopt.h>
#include<libgen.h>
#include<sqlite3.h>
#include<netinet/tcp.h>
#include<sqlite3.h>

#include"temp.h"
#include"socket.h"
#include"sock_sqlite.h"

//extern DS18B20_DATA data;

extern socket_t          *my_socket;


int dev_sqlite3()
{
	int 		rc;
	char 		*err_msg = 0;
	char		*sql = 0;


	rc = sqlite3_open("dev_database.db", &db);
	printf("db pointer(in the dev_sqlite3): %p\n", (void *)db);
	if( rc != SQLITE_OK)
	{
		fprintf(stderr,"Can not open database : %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}


	const char *sql_stmt = 
		"CREATE TABLE IF NOT EXISTS dev_mesg ("
		"de_name TEXT NOT NULL,"
		"de_time TEXT NOT NULL,"
		"de_temp REAL NOT NULL);";


	rc = sqlite3_exec(db, sql_stmt, 0, 0, &err_msg);
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "create table error:%s\n", err_msg);
		sqlite3_free(err_msg);
		return -1;
	}
	printf("Table create successfully\n");
	return 0;
}



int insert_data(struct DS18B20_DATA data)
{
	char			*sql;
	char			*err_msg = 0;
	int				rc;
	printf("data.d_name:%s------data.d_time:%s-----------data.d_temp:%.2f\n",
			data.d_name,data.d_time,data.d_temp);


	sql = sqlite3_mprintf("INSERT INTO dev_mesg (de_name,de_time,de_temp) VALUES ('%s','%s',%.2f);"
			,data.d_name,data.d_time,data.d_temp);


	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK) 
	{
		fprintf(stderr, "give values error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return -1;
	}
	printf("Record inserted successfully\n");


	return 0;
}




char  *read_data()

{
	char				read_buf[1024];
	char				*snd_buf  = read_buf;
	int					rc;
	static int			row_count; 
	char				*errmsg;
	sqlite3_stmt		*stmt;
	const char			*sql;

	sql = "SELECT * FROM dev_mesg LIMIT 1;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
	if( rc != SQLITE_OK)
	{
		fprintf(stderr, "Failed to prepare statement : %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return NULL;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)
	{
		const char *de_name = (const char *)sqlite3_column_text(stmt,0);
		const char *de_time = (const char *)sqlite3_column_text(stmt,1);
		double de_temp = sqlite3_column_double(stmt,2);
		printf("de_name: %s, de_time: %s, de_temp: %.2f\n", de_name, de_time, de_temp);
		memset(snd_buf,0,sizeof(read_buf));
		snprintf(read_buf,2048,"%s,%s,%.2f", de_time, de_name, de_temp);
		printf("%s", read_buf);

	}
	sqlite3_finalize(stmt);
	return snd_buf;

}


/*int extract_data(char *snd_buf)
{
	snd_buf = read_data();

	printf("In the extract_data() snd_buf:%s\n",snd_buf);
	write(my_socket->conn_fd, snd_buf, strlen(snd_buf));
	return 0;
}*/




int del_database(char *snd_buf)
{

	struct tcp_info				optval;
	socklen_t 					optlen = sizeof(optval);
	static int					rc;
	int							ret;
	int							m;
	char                		*err_msg;
	int							row;		
	char						*sql;

	row = get_row();
	printf("Now row:%d\n", row);

	/* 删除数据*/
	printf("Start to delete data\n");

	sql = "DELETE FROM dev_mesg WHERE ROWID IN (SELECT ROWID FROM dev_mesg LIMIT 1);";
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "delete data error : %s\n", sqlite3_errmsg(db));
		sqlite3_free(err_msg);
	}

	return 0;
}



int get_row()
{
	static int 			row_count;
	int					rc;
	sqlite3_stmt		*stmt;
	const char			*sql;

	sql = "SELECT COUNT(*) FROM dev_mesg;";


	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "Failed to prepare statement:%s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}


	rc = sqlite3_step(stmt);
	if( rc == SQLITE_ROW)
	{
		row_count = sqlite3_column_int(stmt,0);
	}
	else
	{
		fprintf(stderr,"Error executing query: %s\n",sqlite3_errmsg(db));
	}


	sqlite3_finalize(stmt);
	return row_count;	
}


