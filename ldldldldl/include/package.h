#ifndef _PACKAGE_H
#define _PACKAGE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

typedef struct
{
    char version[20];   //software version  软件版本
    char protocol[20];  //software protocol 软件协议
    int type;           //data type	    操作对象
    int dataopt;	//operate type	    操作类型
    int length;		//data length	    数据长度
    char data[0];	//pack end adddress 尾地址

}package_t;

typedef struct		//登录服务
{
    char name[36];
    char password[36];
}log_t;

typedef struct		//用户发送的数据
{
    int fd;
    void *data;
}user_data;


typedef struct		//病人信息结构体
{
    int id;
    char name[20];
    char sex[20];
    int age;
    char condition[30];
    char advice[30];
    float money;
    int flag;

}pat;

typedef struct		//医生信息结构体
{
    int id;
    char log[36];
    char name[32];
    char password[36];
    char sex[10];
    int age;
    char keshi[30];
}doc;

typedef struct		//护士信息结构体
{
    int id;
    char log[36];
    char name[20];
    char password[36];
    char sex[10];
    int age;
}nur;

typedef struct		//药品信息结构体
{
    int id;
    char name[20];
    float price;
    char type[20];
    float discount;
}med;

typedef struct		//化验项目信息结构体 
{
	int id;
	char name[20];
	float price;
}assay;


typedef struct		//医生诊疗病人时对病人信息的更新
{
    int id;
    float money;
    char advice[20];
    int flag;
}doc_cur;

typedef struct          //护士执行医嘱时对病人信息的更新
{
    int id;
    int flag;
}nur_cur;
typedef struct  	//医疗表信息结构体
{
	int pid;
	int mid;
	int num;
	float price;
	int flag;
}cure_table;



void *package(char *version,char *protocol,int type,int dataopt,int length,void *data);
void *package_head(package_t head,void *data);
int sendpackage(int sockfd,char *version,char *protocol,int type,int dataopt,int length,void *data);

int sendpackage_head(int sockfd,package_t head,void *data);
int readpackage_head(int sockfd,package_t *head);

int getchar_y();


#endif
