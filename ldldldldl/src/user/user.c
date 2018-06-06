#include"common.h"
#include"server.h"
#include"package.h"
#include"user.h"
#include"md5.h"
#include<stdlib.h>


static int id_id;

void menu_head()
{
    printf("\t\33[35;1m******************************************\n");
    printf("\t******************************************\n");
    printf("\t****\33[0m    \33[32;1m 欢迎患者登录\33[0m  \33[35;1m******\n");
    printf("\t**\33[0m  \33[31;1m Welcom to the nursing care system\33[0m  \33[35;1m**\n");
    printf("\t******************************************\n");
    printf("\t******************************************\33[0m\n");
    printf("\n\n");
}

int login_user(int sockfd)
{
    int opt;
    while(1)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[34;1m1.登录\33[0m\n\n");
	printf("\t\t\33[30;1m2.退出\33[0m\n\n");
	printf("select:");
	scanf("%d",&opt);
	if(opt==1)
	    return login(sockfd);
	else if(opt ==2)
	    return 0;
	else
	{
	    printf("\n");
	    printf("\t\t输入有误,请再次选择\n");
	    sleep(1);
	}
    }
}

//user 登录验证
int login(int sockfd)
{	
    int i=3,ret;
    package_t pack;
    log_t log;
    while(1)
    {
	printf("\n");
	printf("\t\t输入编号:");
	scanf("%s",log.name);
	//md5(log.name);
	printf("\t\t输入姓名:");
	scanf("%s",log.password);
	//md5(log.password);
	ret=sendpackage(sockfd,"m1.0","y1.0",PATIENT,LOG,sizeof(log_t),&log);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return -1;
	}
	ret = readpackage_head(sockfd,&pack);
	if(ret<=0)
	    return -1;
	if(pack.dataopt==LOG_OK)
	{
	    id_id = atoi(log.name);
		//printf("%d\n",id_id);
	    printf("\n");
	    printf("\t\t登录成功\n");
	    sleep(1);
	    user_menu(sockfd);
	}
	else
	{
	    --i;
	    if(i<1)
	    {
		printf("\t\t未验证成功,登录失败\n");
		sleep(1);
		exit(0);
	    }
	    printf("\t\t帐号或密码错误,还有 %d 次机会\n",i);
	    sleep(1);
	}
    }
    return 0;
}
//登录后界面
int user_menu(int sockfd)
{
    int opt;
    for(;;)
    {
	//setbuf(stdin,NULL);
	system("clear");
	menu_head();
	printf("11.show info\n\n");
	printf("12.修改密码(姓名即密码)\n\n");
	printf("13.查看已执行医嘱\n\n");
	printf("14.查看未执行医嘱\n\n");
	printf("15.退出\n\n");
	printf("select:");
	scanf("%d",&opt);
	switch(opt)
	{
		case 11:user_info(sockfd);
			getchar();
			while(!getchar());
			break;	
		case 12: user_updata(sockfd); break;
		case 13: user_show_doneadv(sockfd); 
			getchar();
			while(!getchar());break;
		case 14: user_show_unadv(sockfd); 
			getchar();
			while(!getchar());break;
		case 15: 
			close(sockfd);
			exit(0);
		default:
			printf("\t错误的输入\n");
			sleep(1);
		 	break;
	}
    }
	return 1;
}

void user_info(int sockfd)
{

	pat pa;
	int ret;
	package_t pack;
	int id = id_id;
	ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,SHOW,sizeof(id),&id);
	if(ret<0)
	{
		printf("发送数据失败\n");
		return ;
	}
	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	     return;
	//printf("%d\n",pack.dataopt);
	if(pack.dataopt == SHOW_OK)
	{
		for(;;)
		{
			ret = read(sockfd,&pa,sizeof(pat));
			if(pa.id==-1)
				break;
			printf("编号 %d,姓名 %s,性别 %s,年龄 %d,condition %s,advice %s,monet %.2f\n",
				pa.id,pa.name,pa.sex,pa.age,pa.condition,pa.advice,pa.money);
		}
	}
	else
	{
	    printf("show_fl\n");
	}
	
}
void user_updata(int sockfd)
{
	pat pa;
	int ret;
	package_t pack;
	pa.id = id_id;
	printf("ipuet name(pw):");
	scanf("%s",pa.name);
	ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,UPD,sizeof(pat),&pa);

	if(ret<0)
	{
		printf("发送数据失败\n");
		return ;
	}
	ret = readpackage_head(sockfd,&pack);
	//printf("%d\n",pack.dataopt);
	if(ret<0)
		return;
	if(pack.dataopt==UPD_OK)
	{
		printf("修改成功！强制退出\n");
		exit(0);
	}
	else if(pack.dataopt==UPD_FL)
	{
		printf("修改失败\n");
		return;
	}
	else
	{
		printf("cuowushuju\n");
	}
}   

void user_show_unadv(int sockfd)
{
	int ret,i = 0;
	int id = id_id;
	cure_table CU;
	package_t pack;

	ret = sendpackage(sockfd,"m1.0","y1.0",TABLE,SHOW,sizeof(id),&id);
	if(ret<0)
	{
		printf("\t发送数据失败\n");
		return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
		return;
	if(pack.dataopt == SHOW_OK)
	{
		printf("\t\t*********  未完成医嘱如下所示  *******\n\n");
		for(;;)
		{
			ret = read(sockfd,&CU,sizeof(cure_table));
			if(ret<0)
				return;
			if(CU.pid == -1)
				break;
			if(CU.flag == 0)
			{	

				i++;
				printf("患者id: %d\t药名或化验项目id: %d\t数量: %d\t价格: %.2f\n",CU.pid,CU.mid,CU.num,CU.price);
			}
		}
		if(!i)
		{
			printf("\t\t当前没有未完成的医嘱\n");
			return;
		}
	}
	else
	{
		printf("\t\t暂无医嘱信息\n");
		return;
	}

}
void user_show_doneadv(int sockfd)
{
	int ret,i = 0;
	int id=id_id;
	cure_table CU;
	package_t pack;

	ret = sendpackage(sockfd,"m1.0","y1.0",TABLE,SHOW,sizeof(id),&id);
	if(ret<0)
	{
		printf("\t发送数据失败\n");
		return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
		return;
	if(pack.dataopt == SHOW_OK)
	{
	    printf("\t\t*********  已执行医嘱如下所示  *******\n\n");
	    for(;;)
	    {
		    ret = read(sockfd,&CU,sizeof(cure_table));
		    if(ret<0)
			return;
		    if(CU.pid == -1)
			break;
		    if(CU.flag == 1)
		    {	
			
			i++;
			printf("患者id: %d\t药名或化验项目id: %d\t数量: %d\t价格: %.2f\n",CU.pid,CU.mid,CU.num,CU.price);
		    }
	    }
	    if(!i)
	    {
		printf("\t\t当前没有待执行的医嘱\n");
		    return;
	    }
	}
	else
	{
	    printf("\t\t暂无医嘱信息\n");
	    return;
	}
  
}
// user   患者客户端
int main()
{
    int sockfd,ret;
    struct sockaddr_in serveraddr;
    socklen_t len = sizeof(serveraddr);

    char ip[20],port[20];

    ret = getconf("ip",ip);
    assert(ret==0);
    ret = getconf("port",port);
    assert(ret==0);

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert(sockfd>=0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(port));
    inet_pton(AF_INET,ip,&serveraddr.sin_addr.s_addr);

    ret = connect(sockfd,(struct sockaddr*)&serveraddr,len);
    assert(ret==0);
    login_user(sockfd);
}
