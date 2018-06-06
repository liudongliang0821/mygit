#ifndef _ROOTCTL_H
#define _ROOTCTL_H
#include"package.h"

/*******************  控制台操作  ***************/

int root_docmenu(int socfd);//对医生的操作界面

void rootadd_doc(int sockfd);//添加医生信息
void rootdel_doc(int sockfd);//删除医生信息
void rootmod_doc(int sockfd);//修改医生信息
void rootshow_doc(int sockfd);//查看医生信息

int root_nurmenu(int sockfd);//对护士的操作界面

void rootadd_nurse(int sockfd);//添加护士信息
void rootdel_nur(int sockfd);//删除护士信息
void rootmod_nur(int sockfd);//修改护士信息
void rootshow_nurse(int sockfd);//查看护士信息

int root_medmenu(int sockfd);//对药品的操作界面

void rootadd_med(int sockfd);//添加药品信息
void rootdel_med(int sockfd);//删除药品信息
void rootmod_med(int sockfd);//修改药品信息

void rootshow_med(int sockfd);//查看药品信息

void guahao_menu(int sockfd); //患者挂号

int root_assaymenu(int sockfd);//对化验项目的操作界面

void rootadd_assay(int sockfd);//添加化验项目
void rootdel_assay(int sockfd);//删除化验项目
void rootmod_assay(int sockfd);//修改化验项目
void rootshow_assay(int sockfd);//查看化验项目

int find_moneypat(int sockfd,pat *old_pat);//寻找收费患者

void md5(char *str);//获取md5密码

void root_money(int sockfd);//控制台收费


#define ROOTCTL 1
#define DOCTOR 2
#define NURSE 3
#define PATIENT 4
#define MEDICINE 5
#define ASSAY 6
#define TABLE 7


#define NUR 8
#define NUR_OK 88
#define NUR_FL 87

#define DONE 9
#define DONE_OK 99
#define DONE_FL 98

#define LOG 10
#define LOG_OK 101
#define LOG_FL 102 

#define ADD 20
#define ADD_OK 201
#define ADD_FL 202

#define SHOW 30
#define SHOW_OK 301
#define SHOW_FL 302

#define DEL 40
#define DEL_OK 401
#define DEL_FL 402


#define FIND 50
#define FIND_OK 501
#define FIND_FL 502

#define MOD 60
#define MOD_OK 601
#define MOD_FL 602

#define CURE 70
#define CURE_OK 701
#define CURE_FL 702

#define UPD  80
#define UPD_OK  801
#define UPD_FL  802

#define NOINFO 9999

#endif
