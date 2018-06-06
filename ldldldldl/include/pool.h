#ifndef _POOL_H
#define _POOL_H


#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<assert.h>
#include<pthread.h>

typedef struct work
{
    void *(*pfunc)(void*);
    void *argv;
    struct work *next;
}WORKER;

typedef struct
{
    int number;
    int shutdown;
    pthread_t *pth;
    WORKER *head;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}pool_t;

//初始化线程
int thread_pool_init(pool_t *pool,int number);
//void *(*pfunc)(void*)   返回值是void*  通过函数指针pfunc调用函数 入参是 void* 类型的
int thread_pool_addworker(pool_t *pool,void*(*pfunc)(void*),void *argv,size_t size);  

//销毁线程

int thread_pool_destroy(pool_t *pool);

#endif
