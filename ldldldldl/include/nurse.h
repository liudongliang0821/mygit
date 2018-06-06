#ifndef _NURSE_H
#define _NURSE_H

#include"package.h"
/**********  护士客户端首页  **********/
void menu_head();

/**********  护士登录选择  ***********/
int login_menu(int sockfd);

/**********  护士登录验证  **********/
int login(int sockfd);

/********** 护士操作界面  ***********/
int nur_menu(int sockfd);

/********** 护士执行医嘱  ***********/
void nur_do_adv(int sockfd);


/********* 执行医嘱时寻找指定患者id  ****/
int find_dopat(int sockfd,pat *old_pat);

/*********  护士查看已执行医嘱  *******/
void nur_show_doneadv(int sockfd);

/******************  护士查看未完成医嘱  **************/
void nur_show_unadv(int sockfd);
#endif
