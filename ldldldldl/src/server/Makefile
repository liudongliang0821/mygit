OBJ:=server.o pool.o mysql.o getconf.o package.o

OPT:= -I ../../include  -c -g 

server_obj:$(OBJ)
	gcc $(OBJ) -o ../../bin/server  -lmysqlclient -pthread
server.o:server.c
	gcc $(OPT) server.c
pool.o:pool.c
	gcc $(OPT) pool.c
mysql.o:mysql.c
	gcc $(OPT) mysql.c 
package.o:package.c
	gcc $(OPT) package.c
