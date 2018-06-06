#include"package.h"

/*****************  初始化包头结构体信息  *********************/
void *package(char *version,char *protocol,int type,int dataopt,int length,void *data)
{
    package_t *pack = malloc(sizeof(package_t)+length);
    if(pack==NULL)
	return NULL;

    strcpy(pack->version,version);
    strcpy(pack->protocol,protocol);
    pack->type = type;
    pack->dataopt = dataopt;
    pack->length = length;

    memcpy(pack->data,data,length);
    return (void*)pack;
}


/*************  初始化  ***************/
void *package_head(package_t head,void *data)
{
    package_t *pack = malloc(sizeof(package_t)+head.length);
    if(pack==NULL)
	return NULL;

    memcpy(pack,&head,sizeof(package_t));
    memcpy(pack->data,data,head.length);
    return (void*)pack;
}


/************  发包操作  ********************/
int sendpackage(int sockfd,char *version,char *protocol,int type,int dataopt,int length,void *data)
{
    int ret;
    package_t *pack = package(version,protocol,type,dataopt,length,data);
    if(pack==NULL)
    {
	printf("申请空间失败\n");
	return -1;
    }

    ret = write(sockfd,pack,sizeof(package_t)+length);
    if(ret <0)
    {
	printf("发送失败\n");
	ret = -1;
    }
    else
	ret = 0;

    free(pack);

    return ret;
}

/***************  发包头操作  *************************/
int sendpackage_head(int sockfd,package_t head,void *data)
{
    int ret;
    package_t *pack = package_head(head,data);
    if(pack == NULL)
	return -1;

    ret = write(sockfd,pack,sizeof(package_t)+head.length);
    if(ret<0)
	ret = -1;
    else
	ret = 0;
    free(pack);

    return ret;
}
/*
int readpackage_head(int sockfd,package_t *head)
{
	printf("********************\n\n");    
int ret;
    ret = read(sockfd,head,sizeof(package_t));
	printf("%d\n",head->dataopt);
    if(ret<0)
    {
	printf("read server error\n");
	return -1;
    }
    else if(ret==0)
    {
	printf("断开连接\n");
	return 0;
    }

    return 1;
}
*/

/*****************  选择yes or  no 操作  **********/
int getchar_y()
{
    char ch;
    getchar();
    while(1)
    {
	scanf("%c",&ch);
	if(ch=='y'||ch=='Y')
	    return 1;
	else if(ch=='n'|| ch=='N')
	    return 0;
	else
	    printf("选择有误,请重新输入\n");
    }
}
