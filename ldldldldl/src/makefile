all:docter_obj nurser_obj root_obj server_obj user_obj


docter_obj:
	make -C ./doctor
nurser_obj:
	make -C ./nurse
root_obj:
	make -C ./rootCtl
server_obj:
	make -C ./server
user_obj:
	make -C ./user

clean:
	rm -rf ./doctor/*.o
	rm -rf ./nurse/*.o
	rm -rf ./rootCtl/*.o
	rm -rf ./server/*.o
	rm -rf ./user/*.o
