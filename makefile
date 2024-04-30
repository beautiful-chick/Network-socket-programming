#定义头文件的路径
INCPATH=`pwd`/include/

#定义库文件的路径
LIBPATH= `pwd`/lib/

INC+=-I${INCPATH}
INC+=-I `pwd`/include/
LIB+=-L${LIBPATH}
LIB+=-L `pwd`/../../sqlite3/lib

CC=gcc

all:
	${CC} sock_client.c -g -o sock_client ${INC} ${LIB}  -lmyclient -lsqlite3    

clean:
	rm client.db
	rm client
	rm client.log

client_run:
	export LD_LIBRARY_PATH=./lib/ && ./sock_client -i 127.0.0.1 -p 12121 -t 2 -d	
