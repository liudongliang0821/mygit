#include"common.h"
#include"server.h"
#include"package.h"
#include"rootCtl.h"
#include"doctor.h"
#include"md5.h"


/*************  首页  ****************/
void menu_head()
{
    printf("\t\33[35;1m******************************************\n");
    printf("\t******************************************\n");
    printf("\t****\33[0m    \33[32;1m 欢迎医生登录护理诊疗系统\33[0m  \33[35;1m******\n");
    printf("\t**\33[0m  \33[31;1m Welcom to the nursing care system\33[0m  \33[35;1m**\n");
    printf("\t******************************************\n");
    printf("\t******************************************\33[0m\n");
    printf("\n\n");
}


/***************  doctor选择登录  *************************/
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


/******************** doctor登录验证  *************/
int login(int sockfd)
{
    int i=3,ret;
    package_t pack;
    log_t log;

    while(1)
    {
	printf("\n");
	printf("\t\t输入用户名:");
	scanf("%s",log.name);
        md5(log.name);
	printf("\t\t输入密码:");
	scanf("%s",log.password);
	md5(log.password);
	ret = sendpackage(sockfd,"m1.0","y1.0",DOCTOR,LOG,sizeof(log_t),&log);
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
	    printf("\n");
	    printf("\t\t登录成功\n");
	    sleep(1);
	    doc_menu(sockfd);

	    //return 1;
	}
	else
	{
	    --i;
	    if(i<1)
	    {
		printf("\t\t未验证成功,登录失败\n");
		sleep(1);
		exit(0);
		//break;
	    }
	    printf("\t\t帐号或密码错误,还有 %d 次机会\n",i);
	    sleep(1);
	}
    }
    return 0;
}


/***************  doctor登录后界面  *********/
int doc_menu(int sockfd)
{
    int opt;
    for(;;)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[34;1m1.查看等待患者\n\n");
	printf("\t\t2.诊疗\n\n");
	printf("\t\t3.查看所有病人\33[0m\n\n");
	printf("\t\t\33[30;1m0.退出\33[0m\n\n");
	printf("\t\t\33[33;1m请选择:\33[0m");
	scanf("%d",&opt);
	switch (opt)
	{
	    case 1:
		doc_show_unpat(sockfd);//医生查看等待患者
		getchar();
		while(!getchar());
		break;
	    case 2:
		doc_cur_pat(sockfd);//医生诊断病人
		break;
	    case 3:
		doc_show_allpat(sockfd);//医生查看所有的病人
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

/********  医生查看等待患者  *********/
void doc_show_unpat(int sockfd)
{
    int ret,id=0,i=0;
    pat PA;
    package_t pack;
    
	ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,SHOW,sizeof(id),&id);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	    return;
	if(pack.dataopt == SHOW_OK)
	{ 
	    printf("\t\t*****  等待患者信息如下  *****\n\n");
	    printf("患者id    姓名      性别     年龄    病情   医生建议\n");
	    for(;;)
	    {
		ret = read(sockfd,&PA,sizeof(pat));
		if(PA.id==-1)
		    break;
		if(PA.flag == 1|| PA.flag == 2 || PA.flag == 3)
		{
		   i++;
		   printf("  %d%10s%10s%10d%6s%13s\n",PA.id,PA.name,PA.sex,PA.age,PA.condition,PA.advice);
		}
	    }
	    if(!i)
	    {
		printf("\t\t暂无等待患者信息\n");
		return;
            }
	}
	else
	{
		printf("\t\t当前没有患者信息\n");
		return;
	}

}
/***************   医生查看所有的病人  ***********/
void doc_show_allpat(int sockfd)
{
    int ret,id=0;
    pat PA;
    package_t pack;
    for(;;)
    {
	ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,SHOW,sizeof(id),&id);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	    return;

	//printf("*pack->dataopt = %d\n",pack.dataopt);

	if(pack.dataopt == SHOW_OK)
	{ 
	    printf("\t\t*****  病人信息如下  *****\n\n");
	    printf("患者id    姓名      性别     年龄    病情   医生建议\n");
	    for(;;)
	    {
		ret = read(sockfd,&PA,sizeof(pat));
		if(PA.id==-1)
		    return;
		printf("  %d%10s%10s%10d%6s%13s\n",PA.id,PA.name,PA.sex,PA.age,PA.condition,PA.advice);
		if(PA.flag == 1|| PA.flag == 2 || PA.flag == 3)
			printf(" --> 未诊疗患者\n\n");
		else if(PA.flag == 4 || PA.flag == 5)
			printf(" --> 已诊疗但未被执行医嘱的患者\n\n");
		else if(PA.flag == 6 || PA.flag ==7 )
			printf(" --> 已被执行过医嘱的患者\n\n");
		else if(PA.flag == 8)
			printf(" --> 已经缴费患者\n\n");

		//printf("患者id: %d\t患者姓名: %s\t患者性别: %s\t患者年龄: %d\t患者的治疗医生: %s\n患者的护士: %s\t患者病情: %s\t患者医嘱: %s\n\n",PA.id,PA.name,PA.sex,PA.age,PA.docname,PA.nurname,PA.condition,PA.advice);

	    }
	}
	else
	{
	    printf("\t\t暂无患者信息\n");
	    return;
	}
    }
    return;

}

/*   医生诊疗病人  
 *   可以开药方,选择化验项目
 */

void doc_cur_pat(int sockfd)
{
    int ret,id=0,i=0;
    int pid;
    int aid,num;
    float money =0;
    package_t pack;
    package_t pack1;
    doc_cur DC;
    pat PA;
    assay AS;
    med ME;
    cure_table CU;

	/****************  找出需要诊断的患者信息  ************/
	ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,SHOW,sizeof(id),&id);
	if(ret<0)
	{
	    printf("\t发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<=0)
	    return;

	//printf("pack->dataopt = %d\n",pack.dataopt);
	if(pack.dataopt==SHOW_OK)
	{
	    printf("\t\t******** 待诊疗患者如下 *********\n\n");
	    for(;;)
	    {
		ret = read(sockfd,&PA,sizeof(pat));
		if(PA.id==-1)
			break;
		if(PA.flag==2 || PA.flag==3)
		{
		    if(ret<0)
		    {
			printf("\t读取病人信息失败\n");

			return;
		    }

		    i++;
		    printf("患者id: %d\t患者姓名: %s\t患者性别: %s\t患者年龄: %d\t患者病情: %s\n\n",PA.id,PA.name,PA.sex,PA.age,PA.condition);
		}
	    }
		if(!i)
		{
			printf("\t\t当前没有需要诊疗的患者\n");
			getchar();
			while(!getchar());
			return;
		}
	}
	else
	{
	    printf("\t暂无患者信息\n");
	    getchar();
	    while(!getchar());
	    return;
	}
    
	for(;;)
	{
	    //printf("\t\t输入要诊断的患者id:");
	    //scanf("%d",&id);
	    //getchar();
	    int find_true = find_curepat(sockfd,&PA);
	    if(find_true)
	    {
	
		printf("\t\t对患者的建议:");
		scanf("%s",DC.advice);
		DC.id = PA.id;
		if(PA.flag == 2)
		    DC.flag = 5;
		else if(PA.flag == 3)
		    DC.flag = 4;

		ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,CURE,sizeof(doc_cur),&DC);
		if(ret<0)
		{
		    printf("\t发送数据失败\n");
		    return;
		}
		ret = readpackage_head(sockfd,&pack1);

		//printf("pack1->dataopt = %d\n",pack1.dataopt);
		if(ret<0)
		    return;
		if(pack1.dataopt == CURE_OK)
		{
		    for(;;)
		    {
			//system("clear");
			printf("\n");
			money =0;
			printf("\t\t是否要给患者开药?(y/n):");
			if(!getchar_y())
			    break;
			ME.id = 0;
			ret = sendpackage(sockfd,"m1.0","y1.0",MEDICINE,SHOW,sizeof(med),&ME);
			if(ret<0)
			{
			    printf("\t发送数据失败\n");
			    return;
			}
			ret = readpackage_head(sockfd,&pack);
			if(ret<0)
			    return;

			//printf("pack->dataopt = %d\n",pack.dataopt);


			for(;;)
			{
			    ret = read(sockfd,&ME,sizeof(med));
			    if(ret<0)
				return;
			    if(ME.id == -1)
				break;
			    printf("药品编号: %d\t药品名: %s\t药价格: %.1f\t药类型:%s",ME.id,ME.name,ME.price,ME.type);

			    if(ME.discount==1)
				printf("\t打折情况: 不打折\n");
			    else
				printf("\t打折情况: 打 %.1f折\n",ME.discount*10);

			    //printf("药品编号: %d\t药品名: %s\t药价格: %.1f\t药类型: %s\t打折情况: %.1f折\n",ME.id,ME.name,ME.price,ME.type,ME.discount*10);
			}

			//getchar();
			//printf("aaa\n");
			int find_true = find_needmed(sockfd,&ME);
			if(find_true)
			{
			    printf("\t\t药数量:");
			    scanf("%d",&num);

			    CU.pid = PA.id;
			    CU.mid = ME.id;
			    CU.num = num;
			    CU.flag = 0;

			    CU.price = ME.price*ME.discount;
			    money +=(num*ME.price*ME.discount);

			    ret = sendpackage(sockfd,"m1.0","y1.0",TABLE,ADD,sizeof(cure_table),&CU);
			    if(ret<0)
			    {
				printf("\t发送数据失败\n");
				return;
			    }

			    ret = readpackage_head(sockfd,&pack);
			    if(ret<0)
				return;

			    if(pack.dataopt == ADD_OK)
			    {
				printf("\n");
				printf("\t\t本次开药成功,是否继续开药?(y/n):");
				if(!getchar_y())
				    break;
			    }
			    else
			    {
				printf("\n");
				printf("\t\t本次开药失败,是否重新开药?(y/n):");
				if(!getchar_y())
				    break;
			    }
			}

			else
			{
			    printf("\t\t未找到此药,是否重新选择?(y/n):");
			    if(!getchar_y())
				break;
			}
		    }

		    for(;;)
		    {
			//system("clear");
			printf("\n");
			printf("\t\t是否要给患者化验?(y/n):");
			if(!getchar_y())
			    break;
			AS.id = 0;

			ret = sendpackage(sockfd,"m1.0","y1.0",ASSAY,SHOW,sizeof(assay),&AS);
			if(ret<0)
			{
			    printf("\t发送数据失败\n");
				return;
			}

			ret = readpackage_head(sockfd,&pack);
			if(ret<0)
				return;
			if(pack.dataopt==SHOW_OK)
			{
			    for(;;)
			    {
				ret = read(sockfd,&AS,sizeof(assay));
				if(ret<0)
				    return;
				if(AS.id == -1)
				    break;

				printf("化验项目编号: %d\t化验项目名: %s\t化验项目价格: %.1f\n",AS.id,AS.name,AS.price);
			    }

			    int find_true = find_needassay(sockfd,&AS);
			    if(find_true)
			    {
				CU.pid = PA.id;
				CU.mid = AS.id;
				CU.num = 1;
				CU.flag = 0;
	    
				CU.price = AS.price;
				money += AS.price;
			    

				ret = sendpackage(sockfd,"m1.0","y1.0",TABLE,ADD,sizeof(cure_table),&CU);
				if(ret<0)
				{
				    printf("\t发送数据失败\n");
				    return;
				}
			  
				ret = readpackage_head(sockfd,&pack);
				if(ret<0)
				    return;
				    //printf("\t*******pack.dataopt = %d\n",pack.dataopt);
				if(pack.dataopt == ADD_OK)
				{
				    printf("\n");
				    printf("\t本次化验项目预定成功,是否继续进行化验项目选择?(y/n):");
				    if(!getchar_y())
					break;
				}
				else
				{
				    printf("\n");
				    printf("\t本次化验项目预定失败,是否重新进行化验项目选择?(y/n):");
				    if(!getchar_y())
					break;
				}

			    }
			    else
			    {
				printf("\t未找到此化验项目,是否重新选择?(y/n):");
				if(!getchar_y())
				    break;
			    }
			}

			else
			{
			    printf("\t当前没有化验项目信息,无法进行化验\n");
			    getchar();
			    break;
			}

		    }

		    if(PA.flag == 2)
		    {
			
			DC.id = PA.id;
			DC.flag = 5;
			DC.money = money;
			ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,CURE,sizeof(doc_cur),&DC);
			if(ret<0)
				return;
			ret = readpackage_head(sockfd,&pack);
			if(pack.dataopt == CURE_OK)
			{
				printf("\n");
				printf("\t\t本次诊断成功,患者需付费用为 %.2f 元\n\n",money);

				printf("\t\t谢谢光临本医院,祝早日康复\n\n");
		    		printf("\t\t是否继续诊断其他患者?(y/n):");
		    		if(!getchar_y())
				break;
			}
			else
			{
				printf("\t\t诊断失败,是否重新诊断?(y/n):");
				if(!getchar_y())
					return;
			}
		    } 

		    else if(PA.flag == 3)
		    {
			DC.id = PA.id;
			DC.flag = 4;
			DC.money = money;
			ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,CURE,sizeof(doc_cur),&DC);
			if(ret<0)
				return;
			ret = readpackage_head(sockfd,&pack);
			if(pack.dataopt == CURE_OK)
			{
				printf("\n");
				printf("\t\t本次诊断成功,患者需付费用为 %.2f 元\n\n",money);
				printf("\t\t此患者有医保,费用可直接从医保扣除\n\n");
		   
				
				printf("\t\t谢谢光临本医院,祝早日康复\n\n");
		    		printf("\t\t是否继续诊断其他患者?(y/n):");
		    		if(!getchar_y())
				break;
			}
			else
			{
				printf("\t\t诊断失败,是否重新诊断?(y/n):");
				if(!getchar_y())
					return;
			}
			
		    }
		    
		

		}
		else
		{
		    printf("\t\t患者诊断失败,是否重新诊断?(y/n):");
		    if(!getchar_y())
			return;
		}


		
	    }
	    else
	    {
		printf("\t\t未找到该患者,是否诊断其他患者?(y/n):");
		if(!getchar_y())
		    break;
	    }
	
	
	        
	}
	    
	
}

/*************  doctor找要治疗的患者 ******************/

int find_curepat(int sockfd,pat *old_pat)
{
    package_t pack;
    pat new_pat;
    int ret,i=0;
    while(1)
    {
	printf("\t请选择要诊断的患者id:");
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

	if(pack.dataopt==SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&new_pat,sizeof(pat));
		if(ret<0)
		{
		    printf("\t读取数据失败\n");
		    return -1;
		}

		if(new_pat.id == -1)
		    return 1;

		memcpy(old_pat,&new_pat,sizeof(pat));
	    }

	}
	else
	{
	    printf("\t输入的患者id有误\n");
	    printf("\t是否重新输入?(y/n):");
	    if(!getchar_y())
		return 0;
	}
    }
    return 0;
}



/********************  doctor找需要的化验项目  *********/
int find_needassay(int sockfd,assay *old_assay)
{
    package_t pack;
    assay new_assay;
    int ret,i=0;
    while(1)
    {
    
	    printf("\t请选择化验项目的id:");
	    scanf("%d",&new_assay.id);

	    ret = sendpackage(sockfd,"m1.0","y1.0",ASSAY,FIND,sizeof(assay),&new_assay);
	    if(ret<0)
	    {
		printf("\t发送数据失败\n");
		return -1;
	    }
	    ret = readpackage_head(sockfd,&pack);
	    if(ret<=0)
		return -1;

	    if(pack.dataopt==SHOW_OK)
	    {
		for(;;)
		{
		    ret = read(sockfd,&new_assay,sizeof(assay));
		    if(ret<0)
		    {
			printf("\t读取数据失败\n");
			return -1;
		    }

		    if(new_assay.id==-1)
			return 1;
		    memcpy(old_assay,&new_assay,sizeof(assay));
		}
	    }

	    else
	    {
		printf("\t输入化验项目的id有误\n");
		printf("\t是否重新输入?(y/n):");
		if(!getchar_y())
		    return 0;
	    }
    }
    return 0;

}


/********************  doctor找要开的药  ************/
int find_needmed(int sockfd,med *old_med)
{
    package_t pack;
    med new_med;
    int ret,i=0;
    while(1)
    {
    
	    printf("\t请选择药的id:");
	    scanf("%d",&new_med.id);

	    ret = sendpackage(sockfd,"m1.0","y1.0",MEDICINE,FIND,sizeof(med),&new_med);
	    if(ret<0)
	    {
		printf("\t发送数据失败\n");
		return -1;
	    }
	    ret = readpackage_head(sockfd,&pack);
	    if(ret<=0)
		return -1;

	    if(pack.dataopt==SHOW_OK)
	    {
		for(;;)
		{
		    ret = read(sockfd,&new_med,sizeof(med));
		    if(ret<0)
		    {
			printf("\t读取数据失败\n");
			return -1;
		    }

		    if(new_med.id==-1)
			return 1;
		    memcpy(old_med,&new_med,sizeof(med));
		}
	    }

	    else
	    {
		printf("\t输入药的id有误\n");
		printf("\t是否重新输入?(y/n):");
		if(!getchar_y())
		    return 0;
	    }
    }
    return 0;

}


/*******************  doctor客户端搭建  **************/
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
    
    /*
    if(!login_menu(sockfd))
    {
	printf("\t登录失败\n");
	return -1;
    }
    else
    {
	printf("\t登录成功\n");
	//doc_menu(sockfd);
    }

    return 0;
    */

}
