#ifndef _USER_H
#define _USER_H
void user_show_doneadv(int sockfd);
void user_show_unadv(int sockfd);
void menu_head();
int login_user(int sockfd);
int login(int sockfd);
int user_menu(int sockfd);
void user_updata(int sockfd);
void user_info(int sockfd);
#endif
