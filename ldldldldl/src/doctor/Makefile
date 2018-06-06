OBJ:=docter.o package.o getconf.o

OPT:= -I ../../include  -c -g

docter_obj:$(OBJ)
	gcc $(OBJ) ../../lib/md5.a -o ../../bin/docter
docter.o:docter.c
	gcc $(OPT) docter.c -o docter.o
package.o:package.c
	gcc $(OPT) package.c -o package.o
getconf.o:getconf.c
	gcc $(OPT) getconf.c -o getconf.o
