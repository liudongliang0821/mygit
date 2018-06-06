#include<stdio.h>
#include<string.h>
#include"md5.h"

int getconf(char *input,char *result)
{
    char line[200];
    char key[50];

    FILE *fp = fopen("data.cnf","r");
    if(fp==NULL)
    {
	return -1;
    }

    while(!feof(fp))
    {
	fgets(line,sizeof(line),fp);
	if(line[0] == '#')
	    continue;

	sscanf(line,"%s",key);
	if(strcmp(key,input)==0)
	{
	    sscanf(line,"%*s%*s%s",result);
	    fclose(fp);
	    return 0;
	}
    }

    fclose(fp);
    return -1;
}


void md5(char *str)
{
    unsigned char value[36] = {0};

    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5,str,strlen((char*)str));
    MD5Final(&md5,value);

    strcpy(str,value);
}
/*
int main()
{
    int ret;
    char ip[100];
    char port[60];

    ret = getconf("ip",ip);
    if(ret <0)
    {
	fprintf(stderr,"读取配置文件失败\n");
	return -1;
    }


    ret = getconf("port",port);
    if(ret < 0)
    {
	fprintf(stderr,"读取配置文件失败\n");
	return -1;
    }

    printf("ip = %s\n",ip);
    printf("prot = %s\n",port);

    return 0;
}
*/
