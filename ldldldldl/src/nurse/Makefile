OBJ:=nurse.o getconf.o package.o

OPT:= -I ../../include  -c -g

nurser_obj:$(OBJ)
	gcc $(OBJ) ../../lib/md5.a -o ../../bin/nurse
nurse.o:nurse.c
	gcc $(OPT) nurse.c -o nurse.o
package.o:package.c
	gcc $(OPT) package.c -o package.o
getconf.o:
	gcc $(OPT) getconf.c -o getconf.o
