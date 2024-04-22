#定义头文件的路径
HEADPATH=`pwd`/include/

#定义库文件的路径
LIBPATH= `pwd`/lib/

HEAD+=-I${HEADPATH}
LIB+=-L${LIBPATH}

CC=gcc

all:
	${CC} sock_client.c -g -o sock_client ${HEAD} ${LIB} -I `pwd`/include/ -L `pwd`/../../sqlite3/lib/ -lmyclient -lsqlite3    

clean:
	rm client.db
	rm client
	rm client.log

client_run:
	export LD_LIBRARY_PATH=/home/iot24/lizhao/Socket_Project/Fir_Pro_last/lib/ && ./sock_client -i 127.0.0.1 -p 8787
