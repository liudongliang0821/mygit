OBJ:=user.o package.o getconf.o

OPT:= -I ../../include  -c -g

user_obj:$(OBJ)
	gcc $(OBJ) ../../lib/md5.a -o ../../bin/user
user.o:user.c
	gcc $(OPT) user.c -o user.o
package.o:package.c
	gcc $(OPT) package.c -o package.o
getconf.o:getconf.c
	gcc $(OPT) getconf.c -o getconf.o
