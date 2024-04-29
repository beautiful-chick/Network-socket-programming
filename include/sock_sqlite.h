/********************************************************************************
 *      Copyright:  (C) 2024 LiZhao<3299832490@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sock_sqlite.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(17/04/24)
 *         Author:  LiZhao <3299832490@qq.com>
 *      ChangeLog:  1, Release initial version on "17/04/24 14:34:29"
 *                 
 ********************************************************************************/
#include"socket.h"



/* Description : create database.db and create table to store data */

int open_sqlite3(void);

/* Description : Inserting structure data into the database */

int sqlite_insert_data(data_t data);

/* Description : read data from database*/

int sqlite_read_data(void);

/* Description :  delete the data sent to database*/

int sqlite_del_data();

/* Description :  get total database rows*/

int sqlite_get_row(void);

/* Description : Close database-related parameters */

int sqlite_close_database();
