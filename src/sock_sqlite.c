/*********************************************************************************
 *      Copyright:  (C) 2024 LiZhao<3299832490@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sock_sqlite.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiZhao <3299832490@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/24 14:26:52"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<libgen.h>
#include<sqlite3.h>
#include <netinet/tcp.h>


#include"temp.h"
#include"socket.h"
#include"sock_sqlite.h"
#include"logger.h"


static sqlite3			 *db = NULL;


int open_sqlite3( const char *db_file )
{
	int 		rc;
	char 		*err_msg = 0;
	char		*sql = 0;

	rc = sqlite3_open(db_file, &db);
	log_debug("db pointer(in the dev_sqlite3): %p\n", (void *)db);

	if( rc != SQLITE_OK)
	{
		log_error("Can not open database : %s\n", sqlite3_errmsg(db));
		sqlite_close_database();
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
		log_error("create table error:%s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite_close_database();
		return -1;
	}
	printf("Table create successfully\n");
	return 0;
}


int sqlite_insert_data(data_t data)
{
	char			sql[256];
	char			*err_msg = 0;
	int				rc;
	log_debug("data.d_name:%s------data.d_time:%s-----------data.d_temp:%.2f\n",
			data.d_name,data.d_time,data.d_temp);

	snprintf(sql, sizeof(sql), "INSERT INTO dev_mesg (de_name, de_time, de_temp) VALUES ('%s', '%s', %.2f);", data.d_name, data.d_time, data.d_temp);


	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK) 
	{
		log_error("give values error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite_close_database();
		return -1;
	}
	log_info("Record inserted successfully\n");


	return 0;
}


int sqlite_read_data(data_t data)

{
	
	int					rc;
	static int			row_count; 
	char				*errmsg;
	sqlite3_stmt		*stmt;
	const char			*sql;

	sql = "SELECT * FROM dev_mesg LIMIT 1;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
	if( rc != SQLITE_OK)
	{
		log_error("Failed to prepare statement : %s\n", sqlite3_errmsg(db));
		sqlite_close_database();
		return -1;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)
	{
		const char *de_name = (const char *)sqlite3_column_text(stmt,0);
		const char *de_time = (const char *)sqlite3_column_text(stmt,1);
		double de_temp = sqlite3_column_double(stmt,2);
		log_debug("de_name: %s, de_time: %s, de_temp: %.2f\n", de_name, de_time, de_temp);
		strcpy(data.d_name, de_name);
		strcpy(data.d_time, de_time);
		data.d_temp = de_temp;

	}
	sqlite3_finalize(stmt);
	return 0;

}


int sqlite_del_data(void)
{

	struct tcp_info				optval;
	socklen_t 					optlen = sizeof(optval);
	static int					rc;
	int							ret;
	char                		*err_msg;
	int							row;		
	char						*sql;

	row = sqlite_get_row();
	log_info("Now row:%d\n", row);

	/* 删除数据*/
	log_debug("Start to delete data\n");

	sql = "DELETE FROM dev_mesg WHERE ROWID IN (SELECT ROWID FROM dev_mesg LIMIT 1);";
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if(rc != SQLITE_OK)
	{
		log_error("delete data error : %s\n", sqlite3_errmsg(db));
		sqlite3_free(err_msg);
		sqlite_close_database();
		return -1;
	}

	return 0;
}


int sqlite_get_row(void)
{
	static int 			row_count;
	int					rc;
	sqlite3_stmt		*stmt;
	const char			*sql;

	sql = "SELECT COUNT(*) FROM dev_mesg;";


	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if( rc != SQLITE_OK )
	{
		log_error("Failed to prepare statement:%s\n", sqlite3_errmsg(db));
		sqlite_close_database();
		return -1;
	}


	rc = sqlite3_step(stmt);
	if( rc == SQLITE_ROW)
	{
		row_count = sqlite3_column_int(stmt,0);
	}
	else
	{
		log_error("Error executing query: %s\n",sqlite3_errmsg(db));
		sqlite_close_database();
	}


	sqlite3_finalize(stmt);
	return row_count;	
}


int sqlite_close_database(void)
{
	
	sqlite3_close(db);
}




