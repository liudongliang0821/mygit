#ifndef _MYSQL_H
#define _MYSQL_H
#include"common.h"
#include"rootCtl.h"
#include"package.h"
#include<mysql/mysql.h>

/**************  数据库的连接  ***********/
void mysql_connect(MYSQL *mysql);

/*************  数据库操作  *************/
int mysql_insert(char *str,MYSQL *mysql);

/************  登录验证  **************/
int mysql_log(char *str,MYSQL *mysql);

/************  从数据库读取医生信息  ************/
int mysql_select_doctor(MYSQL *mysql,int clientfd,package_t *cpack,char *str);

/************  从数据库读取护士信息  ************/
int mysql_select_nurse(MYSQL *mysql,int clientfd,package_t *cpack,char *str);

/************  从数据库读取药品信息  ************/
int mysql_select_medicine(MYSQL *mysql,int clientfd,package_t *cpack,char *str);


/************  从数据库读取化验项目信息  ************/
int mysql_select_assay(MYSQL *mysql,int clientfd,package_t *cpack,char *str);

/************  从数据库读取病人信息  ************/
int mysql_select_patient(MYSQL *mysql,int clientfd,package_t *cpack,char *str);


/************  从数据库读取医疗表信息  ************/
int mysql_select_table(MYSQL *mysql,int clientfd,package_t *cpack,char *str);
#endif
