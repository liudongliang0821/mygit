#ifndef _SERVER_H
#define _SERVER_H

#include"package.h"
#include"mysql.h"

void pat_upd(int clientfd,package_t *cpack,MYSQL mysql);

//医生登录
void doc_log(int clientfd,package_t *cpack,MYSQL mysql);

//护士登录
void nur_log(int clientfd,package_t *cpack,MYSQL mysql);

//患者登录
void pat_log(int clientfd,package_t *cpack,MYSQL mysql);

//读取套接字文件
int getconf(char *input,char *result);

//服务器执行的总函数
void *server_func(void *argv);
void add_doc(int clientfd,package_t *cpack,MYSQL mysql);//添加医生信息
void mod_doc(int clientfd,package_t *cpack,MYSQL mysql);//修改医生信息
void del_doc(int clientfd,package_t *cpack,MYSQL mysql);//删除医生信息
void show_doc(int clientfd,package_t *cpack,MYSQL mysql);//查看医生信息

void add_nur(int clientfd,package_t *cpack,MYSQL mysql); //添加护士
void del_nur(int clientfd,package_t *cpack,MYSQL mysql);//删除护士信息
void mod_nur(int clientfd,package_t *cpack,MYSQL mysql);//修改护士信息
void show_nur(int clientfd,package_t *cpack,MYSQL mysql);//查看护士信息

void add_med(int clientfd,package_t *cpack,MYSQL mysql);//添加药品
void del_med(int clientfd,package_t *cpack,MYSQL mysql);//删除药品信息
void mod_med(int clientfd,package_t *cpack,MYSQL mysql);//修改药品信息
void show_med(int clientfd,package_t *cpack,MYSQL mysql);//查看药品信息

void add_assay(int clientfd,package_t *cpack,MYSQL mysql);//添加化验项目
void del_assay(int clientfd,package_t *cpack,MYSQL mysql);//删除化验项目信息
void mod_assay(int clientfd,package_t *cpack,MYSQL mysql);//修改化验项目信息
void show_assay(int clientfd,package_t *cpack,MYSQL mysql);//查看化验项目信息

void guahao(int clientfd,package_t *cpack,MYSQL mysql);//患者挂号
void show_pat(int clientfd,package_t *cpack,MYSQL mysql);//查看病人信息

void cure_pat(int clientfd,package_t *cpack,MYSQL mysql);//医生诊断患者

void add_table(int clientfd,package_t *cpack,MYSQL mysql);//医疗表信息增加

void show_table(int clientfd,package_t *cpack,MYSQL mysql);//查看医嘱


void do_table(int clientfd,package_t *cpack,MYSQL mysql);//护士执行医嘱
void nur_pat(int clientfd,package_t *cpack,MYSQL mysql);//护士执行医嘱对患者信息的更新


void find_med(int clientfd,package_t *cpack,MYSQL mysql);//医生开药寻找药

void find_assay(int clientfd,package_t *cpack,MYSQL mysql);//医生开化验找化验醒目

void find_pat(int clientfd,package_t *cpack,MYSQL mysql);//医生寻找要诊疗的患者


void seller_pat(int clientfd,package_t *cpack,MYSQL mysql);  //控制台收费
#endif
