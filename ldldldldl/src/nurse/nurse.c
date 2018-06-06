#include"common.h"
#include"server.h"
#include"package.h"
#include"rootCtl.h"
#include"nurse.h"
#include"md5.h"


/**************  护士首页   *****************/
void menu_head()
{
    printf("\t\33[35;1m******************************************\n");
    printf("\t******************************************\n");
    printf("\t****\33[0m    \33[32;1m 欢迎护士登录护理诊疗系统\33[0m  \33[35;1m******\n");
    printf("\t**\33[0m  \33[31;1m Welcom to the nursing care system\33[0m \33[35;1m **\n");
    printf("\t******************************************\n");
    printf("\t******************************************\33[0m\n");
    printf("\n\n");
}


/*************  护士登录选择   *************/
int login_menu(int sockfd)
{
    int opt;

    while(1)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[34;1m1.登录\33[0m\n\n");
	printf("\t\t\33[30;1m2.退出\33[0m\n\n");
	printf("\t\t\33[33;1m请选择: \33[0m");

	scanf("%d",&opt);
	if(opt==1)
	    return login(sockfd);
	else if(opt==2)
	    return 0;
	else
	{
	    printf("\n");
	    printf("\t\t输入有误,请重新选择\n");
	    sleep(1);
	}
    }

}


/***************  护士登录验证  ****************/
int login(int sockfd)
{
    int i=3,ret;
    package_t pack;
    log_t log;
    while(1)
    {
	printf("\t输入用户名:");
	scanf("%s",log.name);
	md5(log.name);
	printf("\t输入密码:");
	scanf("%s",log.password);
	md5(log.password);
	ret = sendpackage(sockfd,"m1.0","y1.0",NURSE,LOG,sizeof(log_t),&log);
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
	    printf("\t登录成功\n");
	    sleep(1);
	    nur_menu(sockfd);

	    //return 1;
	}
	else
	{
	    --i;
	    if(i<1)
	    {
		printf("\t未验证成功,登录失败\n");
		sleep(1);
		exit(0);
		//break;
	    }
	    printf("\t帐号或密码错误,还有 %d 次机会\n",i);
	    sleep(1);
	}
    }
    return 0;
}


/*******************  护士登录成功后界面  **********/
int nur_menu(int sockfd)
{
    int opt;
    for(;;)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[34;1m1.查看未执行医嘱\n\n");
	printf("\t\t2.执行医嘱\n\n");
	printf("\t\t3.查看已执行医嘱\33[0m\n\n");
	printf("\t\t\33[30;1m0.退出\33[0m\n\n");
	printf("\t\t\33[33;1m请选择: \33[0m");
	scanf("%d",&opt);
	switch (opt)
	{
	    case 1:
		nur_show_unadv(sockfd);//护士查看未完成医嘱
		getchar();
		while(!getchar());
		break;
	    case 2:
		nur_do_adv(sockfd);//护士执行医嘱
		break;
	    case 3:
		nur_show_doneadv(sockfd);//护士查看已执行医嘱
		getchar();
		while(!getchar());
		break;
	    case 0:
		close(sockfd);
		exit(0);
	    default:
		printf("\t\t输入有误,请重新选择\n");
		sleep(1);
		break;
	}
    }

    return 1;
}
/******************  护士查看未完成医嘱  **************/
void nur_show_unadv(int sockfd)
{
	int ret,id = 0,i = 0;
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
/******************  护士查看已执行医嘱  **************/
void nur_show_doneadv(int sockfd)
{
	int ret,id = 0,i = 0;
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

/******************  护士执行医嘱  ***********/
void nur_do_adv(int sockfd)
{
    int ret,id=0,i=0;
    nur_cur NU;
    cure_table CU;
    package_t pack;
    pat PA;
    
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
	    printf("\t\t******* 待执行医嘱如下所示 ********\n\n");
	    for(;;)
	    {
		ret = read(sockfd,&CU,sizeof(cure_table));
		if(ret<0)
		    return;
		if(CU.pid == -1)
		    break;
		if(CU.flag==0)
		{
		    i++;
		    printf("患者id: %d\t药名或化验项目id: %d\t数量: %d\t价格: %.2f\n",CU.pid,CU.mid,CU.num,CU.price);
		}
	    }
	    if(!i)
	    {
		printf("\t\t当前没有待执行的医嘱\n");
		getchar();
		while(!getchar());
		return;
	    }
	}
	else
	{
	    printf("\t\t暂无医嘱信息\n");
	    getchar();
		while(!getchar());
	    return;
	}
   


    for(;;)
    {
	int find_true = find_dopat(sockfd,&PA);
	if(find_true)
	{

	    if(PA.flag == 4)
		NU.flag = 6;
	    else if(PA.flag == 5)
		NU.flag = 7;


	//printf("\t\t输入要执行医嘱患者的id:");
	//scanf("%d",&id);	
	    NU.id = PA.id;

	    ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,NUR,sizeof(nur_cur),&NU);
	    if(ret<0)
	    {
		printf("\t发送数据失败\n");
		return;
	    }
	    
	    ret = readpackage_head(sockfd,&pack);
	    if(ret<0)	
		return;
	    if(pack.dataopt == NUR_OK)
	    {
		printf("\t\t是否确定执行该患者医嘱?(y/n):");
		if(!getchar_y())
		    break;
		CU.flag = 1;
		CU.pid = PA.id;

		ret = sendpackage(sockfd,"m1.0","y1.0",TABLE,DONE,sizeof(cure_table),&CU);
		if(ret<0)
		{
		    printf("\t发送数据失败\n");
		    return;
		}

		ret = readpackage_head(sockfd,&pack);
		if(pack.dataopt==DONE_OK)
		{
		    printf("\t\t医嘱执行中....\n");
		    sleep(2);
		    printf("\t\t医嘱执行完毕,是否还要执行其他人的医嘱?(y/n):");
		    if(!getchar_y())
			break;
		}
		else
		{
		    printf("\t\t医嘱执行失败,是否重新执行?(y/n):");
		    if(!getchar_y())
			break;
		}
	    }

	    else
	    {
		printf("\t\t失败,是否重新执行?(y/n) %s : %d\n:",__FILE__,__LINE__);
		if(!getchar())
		    break;
	    }

	}
	else
	{
	    printf("\t\t患者不存在,是否执行其他患者医嘱?(y/n):");
	    if(!getchar_y())
		break;
	}
	    
	
    }
}


/***********  护士查找要执行医嘱的患者id   *******/
int find_dopat(int sockfd,pat *old_pat)
{
    package_t pack;
    pat new_pat;
    int ret;
    while(1)
    {
	printf("\t\t请输入要执行医嘱的患者id: ");
	scanf("%d",&new_pat.id);

	ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,FIND,sizeof(pat),&new_pat);
	if(ret<0)
	{
	    printf("\t发送数据失败\n");
	    return -1;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<=0)
	    return -1;

	if(pack.dataopt == SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&new_pat,sizeof(pat));
		if(ret<0)
		    return -1;
		if(new_pat.id == -1)
		    return 1;

		memcpy(old_pat,&new_pat,sizeof(pat));
	    }
	}

	else
	{
	    printf("\t\t输入的患者id有误\n");
	    printf("\t\t是否重新选择?(y/n):");
	    if(!getchar_y())
		return 0;
	}
    }
    return 0;
}

/*********************  护士客户端搭建  **************/
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

    login_menu(sockfd);
}

