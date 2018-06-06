#include<stdio.h>
#include<string.h>

/*****************  读取配置文件信息  ***********/
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


