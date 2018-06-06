OBJ:= package.o rootCtl.o getconf.o

OPT:= -I ../../include  -c -g

root_obj:$(OBJ)
	gcc $(OBJ) ../../lib/md5.a -o ../../bin/rootCtrl
package.o:package.c
	gcc $(OPT) package.c -o package.o
rootCtl.o:rootCtl.c
	gcc $(OPT) rootCtl.c -o rootCtl.o
getconf.o:getconf.c
	gcc $(OPT) getconf.c -o getconf.o
