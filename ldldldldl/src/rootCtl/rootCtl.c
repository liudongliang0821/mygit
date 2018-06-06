#include"common.h"
#include"server.h"
#include"package.h"
#include"rootCtl.h"
#include"md5.h"

/**************  控制台首页  ***************/
void menu_head()
{
    printf("\t\33[35;1m******************************************\33[0m\n");
    printf("\t\33[35;1m******************************************\33[0m\n");
    printf("\t\33[35;1m****\33[0m   \33[32;1m 欢迎root登录护理诊疗系统\33[0m  \33[35;1m******\33[0m\n");
    printf("\t\33[35;1m**\33[0m  \33[31;1m Welcom to the nursing care system\33[0m \33[35;1m **\33[0m\n");
    printf("\t\33[35;1m******************************************\33[0m\n");
    printf("\t\33[35;1m******************************************\33[0m\n");
    printf("\n\n");
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
/******************  控制台登录验证 ***************/
int login()
{
    int i = 3,ret;
    char log_account[20],log_password[20];
    char log[20],pass[20];
    int opt;
   
    ret = getconf("log_account",log);
    assert(ret==0);

    ret = getconf("log_password",pass);
    assert(ret==0);
    while(1)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[34;1m1.登录\33[0m\n\n");
	printf("\t\t\33[30;1m2.退出\33[0m\n\n");
	printf("\t\t\33[33;1m请选择:\33[0m");
	scanf("%d",&opt);
	if(opt==1)
	    break;
	else if(opt==2)
	    return 0;
	else
	{
	    printf("\n");
	    printf("\t\t\33[31;1m输入有误,请重新输入\33[0m\n");
	    sleep(1);
	}

    }

    while(1)
    {
	printf("\n");
	printf("\t\t输入帐号:");
	scanf("%s",log_account);
	printf("\n\n");
	printf("\t\t输入密码:");
	scanf("%s",log_password);

	if(!strcmp(log,log_account) && !strcmp(pass,log_password))
	    return 1;
	else
	{
	    --i;
	    if(i<1)
	    {
		printf("\t\t未验证成功\n");
		getchar();
		break;
	    }
	    printf("\t\t输入错误,还有 %d 次机会\n",i);
	    sleep(1);
	}
    }


    return 0;

}


/***************  控制台操作界面  ************/
int root_menu(int sockfd)
{
    int opt;
    for(;;)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[31;1m1.管理医生\n\n");
	printf("\t\t\33[32;1m2.管理护士\n\n");
	printf("\t\t\33[33;1m3.挂号\n\n");
	printf("\t\t\33[34;1m4.收费\n\n");
	printf("\t\t\33[35;1m5.管理药品\n\n");
	printf("\t\t\33[36;1m6.化验项目\33[0m\n\n");
	printf("\t\t\33[30;1m0.退出\33[0m\n\n");
	printf("\t\t\33[33;1m请选择: \33[0m");
	scanf("%d",&opt);
	switch (opt)
	{
	    case 1:
		root_docmenu(sockfd);
		break;
	    case 2:
		root_nurmenu(sockfd);
		break;
	    case 3:
		guahao_menu(sockfd);
		break;
	    case 4:
		root_money(sockfd);
		break;
	    case 5:
		root_medmenu(sockfd);
		break;
	    case 6:
		root_assaymenu(sockfd);
		break;
	    case 0:
		close(sockfd);
		return 0;
	    default:
		printf("\t\t输入错误,请重新输入");
		getchar();
		while(!getchar());
		break;

	}
    }
}


/*************   控制台对患者收费   ************/
void root_money(int sockfd)
{
    int ret,id = 0,i=0;
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

	if(pack.dataopt == SHOW_OK)
	{
	    printf("\t\t***** 需交费患者如下  *****\n\n");
	    for(;;)
	    {
		ret = read(sockfd,&PA,sizeof(pat));
		if(ret<0)
		    return;
		if(PA.id == -1)
		    break;
		if(PA.flag == 6 || PA.flag == 7)
		{
		    i++;
		    printf("患者id: %d\t患者姓名: %s\t患者需交费用: %.2f\n",PA.id,PA.name,PA.money);
		}
	    }
	    if(!i)
	    {
		printf("\t\t**** 暂无要收费的患者 ****\n");
		getchar();
		while(!getchar());
		return;
	    }
	}
	else
	{
	    printf("\t\t当前无病人信息\n");
	    getchar();
	    while(!getchar());
	    return;
	}
    
	for(;;)
	{
	    int find_true = find_moneypat(sockfd,&PA);
	    if(find_true)
	    {
		if(PA.flag != 6 && PA.flag != 7)
		{
		    printf("\t\t当前选择患者还不能收费,是否重新选择?(y/n):");
		    if(!getchar_y())
			return;
		}
	    

		if(PA.flag == 6)
		{
		    printf("\t\t该患者需交费 %.2f 元,具有医保,可从医保中直接扣除\n",PA.money);
		    printf("\t\t是否确认交费?(y/n):");
		    if(getchar_y())
		    {
			PA.flag = 8;
			ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,MOD,sizeof(pat),&PA);
			if(ret<0)
			{
			    printf("\t发送数据失败\n");
			return;
			}

			ret = readpackage_head(sockfd,&pack);
			if(ret<0)
			   return;
			if(pack.dataopt == MOD_OK)
			{
			    printf("\t\t本次收费成功,谢谢\n");
			    printf("\t\t祝您生活愉快\n\n");
			    printf("\t\t是否继续收费?(y/n):");
			    if(!getchar_y())
				return;
			}
		    }
		    else
		    {
			printf("\t\t收费失败,是否重新收费?(y/n):");
			if(!getchar_y())
			    return;
		    }
		}
		else if(PA.flag == 7)
		{
		    printf("\t\t该患者需交费 %.2f 元\n",PA.money);
		

		    printf("\t\t是否确认交费?(y/n):");
		    if(getchar_y())
		    {
			PA.flag = 8;
			ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,MOD,sizeof(pat),&PA);
			if(ret<0)
			{
			    printf("\t发送数据失败\n");
				return;
			}

			ret = readpackage_head(sockfd,&pack);
			if(ret<0)
			    return;
			if(pack.dataopt == MOD_OK)
			{
			    printf("\t\t本次收费成功,谢谢\n");
			    printf("\t\t祝您生活愉快\n\n");
			    printf("\t\t是否继续收费?(y/n):");
			    if(!getchar_y())
				return;
			}
		    }
		    else
		    {
			printf("\t\t收费失败,是否重新收费?(y/n):");
			if(!getchar_y())
			    return;
		    }
		}
	    }
	    else
	    {
		printf("\t\t该患者不存在,是否选择其他患者收费?(y/n):");
		if(!getchar_y())
		    return;
	    }

	}
    }
}

/******************  收费时寻找对用的患者 id **************/
int find_moneypat(int sockfd,pat *old_pat)
{
    package_t pack;
    pat new_pat;
    int ret, i=0;

    while(1)
    {
	printf("\t\t请选择要收费的患者id: ");
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

		if(new_pat.id==-1)
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

/***************  控制台对化验项目操作界面  ***********/
int root_assaymenu(int sockfd)
{
    int opt;
    int state = 0;
    for(;;)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[34;1m1.添加化验项目\n\n");
	printf("\t\t2.删除化验项目\n\n");
	printf("\t\t3.修改化验项目\n\n");
	printf("\t\t4.查看化验项目\n\n");
	printf("\t\t5.返回上一级\33[0m\n\n");
	printf("\t\t\33[30;1m0.退出\33[0m\n\n");
	printf("\t\t\33[33;1m请选择: \33[0m");
	scanf("%d",&opt);
	switch (opt)
	{
	    case 1:
		rootadd_assay(sockfd);
		//getchar();
		break;
	    case 2:
		rootdel_assay(sockfd);
		break;
	    case 3:
		rootmod_assay(sockfd);
		break;
	    case 4:
		rootshow_assay(sockfd);
		getchar();
		while(!getchar());
		break;
	    case 5:
		state=1;
		break;
	    case 0:
		close(sockfd);
		exit (0);
	    default:
		printf("\t\t输入有误,请重新选择");
		getchar();
		while(!getchar());
		break;
	}
	if(state==1)
	    break;
    }
    return 1;

}

/**************  添加化验项目   **********/
void rootadd_assay(int sockfd)
{
    int ret;
    package_t pack;
    assay AS;

    for(;;)
    {
	printf("\t\t输入化验项目编号:");
	scanf("%d",&AS.id);
	printf("\t\t输入化验项目名:");
	scanf("%s",AS.name);
	printf("\t\t输入化验价格:");
	scanf("%f",&AS.price);

	ret = sendpackage(sockfd,"m1.0","y1.0",ASSAY,ADD,sizeof(assay),&AS);
	if(ret<0)
	{
	    printf("发送失败\n");
	    getchar();
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<=0)
	{
	    return;
	}

	if(pack.dataopt == ADD_OK)
	{
	    printf("\t\t添加化验项目成功,是否继续添加?(y/n)\n");
	    if(!getchar_y())
		return;
	}
	else
	{
	    printf("\t\t化验项目编号已存在,是否重新添加?(y/n)\n");
	    if(!getchar_y())
		return;
	}
    }

}

/*****************  删除化验项目 *******************/
void rootdel_assay(int sockfd)
{
    int ret,id;
    package_t pack;
    assay AS;
    for(;;)
    {
	printf("\t\t请输入要删除化验项目编号:");
	scanf("%d",&id);
	ret = sendpackage(sockfd,"m1.0","y1.0",ASSAY,SHOW,sizeof(int),&id);
	if(ret<0)
	{
	    printf("发送数据失败\n");
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
		if(AS.id==-1)
		    break;
		printf("化验项目编号: %d\t化验项目名: %s\t化验项目价格: %.1f\n",AS.id,AS.name,AS.price);
	    }
	}
	else
	{
	    printf("\t\t没有该化验项目信息\n");
	    getchar();
	    return;
	}

	printf("\t\t是否确定删除(y/n):");
	if(getchar_y())
	{
	    ret = sendpackage(sockfd,"m1.0","y1.0",ASSAY,DEL,sizeof(int),&id);
	    if(ret<0)
	    {
		printf("发送数据错误\n");
		return;
	    }

	    ret = readpackage_head(sockfd,&pack);
	    if(ret<=0)
		return;
	    if(pack.dataopt == DEL_OK)
	    {
		printf("\t\t删除成功,是否删除其他药品信息(y/n):");
		if(!getchar_y())
		    break;
	    }

	    else
	    {
		printf("\t\t删除失败,是否继续(y/n):");
		if(!getchar_y())
		    return;
	    }
	}
	else
	{
	    printf("\t\t是否删除其它信息(y/n):");
	    if(!getchar())
		return;
	}
    }
    return;
}

/****************** 修改化验项目信息  ****************/
void rootmod_assay(int sockfd)
{
    int ret,id;
    package_t pack;
    assay AS;
    for(;;)
    {
	printf("\t\t请输入要修改化验项目编号:");
	scanf("%d",&id);
	getchar();
	ret = sendpackage(sockfd,"m1.0","y1.0",ASSAY,SHOW,sizeof(int),&id);
	if(ret <0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	    return;

	if(pack.dataopt == SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&AS,sizeof(assay));
		if(AS.id==-1)
		    break;
		printf("化验项目编号: %d\t化验项目名: %s\t化验项目价格: %.1f\n",AS.id,AS.name,AS.price);
	    }
	}
	else
	{
	    printf("\t\t没有该化验项目信息\n");
	    getchar();
	    return;
	}

	for(;;)
	{
	    AS.id = id;
	    printf("\t请选择修改选项: 1.化验项目名 2.化验项目价格\n");
	    int opt;
	    scanf("%d",&opt);
	    switch (opt)
	    {
		case 1:
		    printf("\t请输入修改化验项目名:");
		    scanf("%s",AS.name);
		    break;
		case 2:
		    printf("\t请输入修改化验项目价格:");
		    scanf("%f",&AS.price);
		    break;
		default:
		    printf("\n选择有误,重新选择\n");
		    break;

	    }
	    printf("\t是否还要修改其它项?(y/n):");
	    if(!getchar_y())
		break;
	}
	printf("\t是否确认修改?(y/n):");
	if(getchar_y())
	{
	    ret = sendpackage(sockfd,"m1.0","y1.0",ASSAY,MOD,sizeof(assay),&AS);
	    if(ret<0)
	    {
		printf("发送数据失败\n");
		return;
	    }

	    ret = readpackage_head(sockfd,&pack);
	    if(ret<0)
		return;
	    if(pack.dataopt == MOD_OK)
	    {
		printf("\t修改成功,是否修改其它?(y/n):");
		if(!getchar_y())
		    break;
	    }
	    else
	    {
		printf("\t修改失败,是否继续?(y/n):");
		if(!getchar_y())
		    return;
	    }
	}
	else
	{
	    printf("\t是否修改其它信息?(y/n):");
	    if(!getchar_y())
		return;
	}
    }

    return;
}


/***********************  查看化验项目信息  ****************/
void rootshow_assay(int sockfd)
{
    int ret,id =0;  //id为零,查看全部的信息
    package_t pack;
    assay AS;
    for(;;)
    {
	ret = sendpackage(sockfd,"m1.0","y1.0",ASSAY,SHOW,sizeof(id),&id);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<=0)
	    return;
	if(pack.dataopt==SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&AS,sizeof(assay));
		if(AS.id==-1)
		    return;
		printf("化验项目编号: %d\t化验项目名: %s\t化验项目价格(rmb): %.1f\n",AS.id,AS.name,AS.price);
	    }
	}
	else
	{
	    printf("\t没有信息,读取错误\n");
	    return;
	}
    }
    return;
}


/*********************  控制台对医生信息的操作界面  ************/
int root_docmenu(int sockfd)
{
    int opt;
    int state = 0;
    for(;;)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[34;1m1.添加医生\n\n");
	printf("\t\t2.删除医生\n\n");
	printf("\t\t3.修改医生信息\n\n");
	printf("\t\t4.查看医生信息\n\n");
	printf("\t\t5.返回上一级\33[0m\n\n");
	printf("\t\t\33[30;1m0.退出\33[0m\n\n");
	printf("\t\t\33[33;1m请选择: \33[0m");
	scanf("%d",&opt);
	switch (opt)
	{
	    case 1:
		rootadd_doc(sockfd);
		//getchar();
		break;
	    case 2:
		rootdel_doc(sockfd);
		break;
	    case 3:
		rootmod_doc(sockfd);
		break;
	    case 4:
		rootshow_doc(sockfd);
		getchar();
		while(!getchar());
		break;
	    case 5:
		state=1;
		break;
	    case 0:
		close(sockfd);
		exit (0);
	    default:
		printf("\t\t输入有误,请重新选择");
		getchar();
		while(!getchar());
		break;
	}
	if(state==1)
	    break;
    }
    return 1;
}


/****************  查看医生信息  ******************/
void rootshow_doc(int sockfd)
{
    int ret,id,opt;  //id为零,查看全部的医生信息
    package_t pack;
    doc DO;
    printf("\t\t请选择查看类型:  输入非0按id查看,输入0查看所有医生信息:");
    printf("\n");
    printf("\t\t选择:");	
    scanf("%d",&id);
    for(;;)
    {
	ret = sendpackage(sockfd,"m1.0","y1.0",DOCTOR,SHOW,sizeof(id),&id);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<=0)
	    return;
	if(pack.dataopt==SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&DO,sizeof(doc));
		if(DO.id==-1)
		    return;
		printf("工号: %d\t姓名: %s\t性别: %s\t年龄: %d\t科室: %s\n",DO.id,DO.name,DO.sex,DO.age,DO.keshi);
	    }
	}
	else
	{
	    printf("\t没有该医生信息\n");
	    return;
	}
    }
    return;
}

/*****************************  添加医生信息  ***************/
void rootadd_doc(int sockfd)
{
    int ret;
    package_t pack;
    doc DO;
    char pass1[20];
    char pass2[20];

    for(;;)
    {
	printf("\t输入医生工号:");
	scanf("%d",&DO.id);
	printf("\t输入医生登录名:");
	scanf("%s",DO.log);
	md5(DO.log);
	printf("\t输入医生姓名:");
	scanf("%s",DO.name);
	printf("\t输入密码:");
	scanf("%s",pass1);
	printf("\t再次输入密码:");
	scanf("%s",pass2);
	if(!strcmp(pass1,pass2))
	{
	    md5(pass2);
	    strcpy(DO.password,pass2);
	    printf("\t输入性别:");
	    scanf("%s",DO.sex);
	    printf("\t输入年龄:");
	    scanf("%d",&DO.age);
	    printf("\t输入科室:");
	    scanf("%s",DO.keshi);
	}
	else
	{
	    printf("两次输入密码不一致,注册失败\n");
	    getchar();
	    continue;
	}
	
	//printf("%d %s %s %s %s  %d %s\n",DO.id,DO.log,DO.name,DO.password,DO.sex,DO.age,DO.keshi);
	ret = sendpackage(sockfd,"m1.0","y1.0",DOCTOR,ADD,sizeof(doc),&DO);
	if(ret<0)
	{
	    printf("发送失败\n");
	    getchar();
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<=0)
	{
	    return;
	}

	if(pack.dataopt == ADD_OK)
	{
	    printf("\t添加医生成功,是否继续添加?(y/n):");
	    if(!getchar_y())
		return;
	}
	else
	{
	    printf("\t工号已存在,是否重新添加?(y/n):");
	    if(!getchar_y())
		return;
	}
    }
}


/********************  删除医生信息  ******************/
void rootdel_doc(int sockfd)
{
    int ret,id;
    package_t pack;
    doc DO;
    for(;;)
    {
	printf("\t\t请输入要删除的医生工号:");
	scanf("%d",&id);
	ret = sendpackage(sockfd,"m1.0","y1.0",DOCTOR,SHOW,sizeof(int),&id);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	    return;

	if(pack.dataopt==SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&DO,sizeof(doc));
		if(DO.id==-1)
		    break;
		printf("工号: %d\t医生姓名: %s\t医生性别: %s\t医生年龄: %d\t医生科室: %s\n",DO.id,DO.name,DO.sex,DO.age,DO.keshi);
	    }
	}
	else
	{
	    printf("\t没有该医生信息\n");
	    getchar();
	    while(!getchar());
	    return;
	}

	printf("\t是否确定删除(y/n):");
	if(getchar_y())
	{
	    ret = sendpackage(sockfd,"m1.0","y1.0",DOCTOR,DEL,sizeof(int),&id);
	    if(ret<0)
	    {
		printf("发送数据错误\n");
		return;
	    }

	    ret = readpackage_head(sockfd,&pack);
	    if(ret<=0)
		return;
	    if(pack.dataopt == DEL_OK)
	    {
		printf("\t删除成功,是否删除其他医生信息(y/n):");
		if(!getchar_y())
		    break;
	    }

	    else
	    {
		printf("\t删除失败,是否继续(y/n):");
		if(!getchar_y())
		    return;
	    }
	}
	else
	{
	    printf("\t是否删除其它信息(y/n):");
	    if(!getchar())
		return;
	}
    }
    return;
}


/*************************   修改医生信息  **************/
void rootmod_doc(int sockfd)
{
    int ret,id;
    package_t pack;
    doc DO;
    for(;;)
    {
	printf("\t请输入要修改医生的工号:");
	scanf("%d",&id);
	getchar();
	ret = sendpackage(sockfd,"m1.0","y1.0",DOCTOR,SHOW,sizeof(int),&id);
	if(ret <0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	    return;

	if(pack.dataopt == SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&DO,sizeof(doc));
		if(DO.id==-1)
		    break;
		printf("工号: %d\t医生姓名: %s\t医生性别: %s\t医生年龄: %d\t医生科室: %s\n",DO.id,DO.name,DO.sex,DO.age,DO.keshi);

	    }
	}
	else
	{
	    printf("\t没有该医生信息\n");
	    getchar();
	    return;
	}

	for(;;)
	{
	    DO.id = id;
	    printf("\t请选择修改选项: 1.姓名 2. 登录名 3.密码 4.年龄 5.科室\n");
	    printf("\n");
	    printf("\t\t选择:");
	    int opt;
	    scanf("%d",&opt);
	    switch (opt)
	    {
		case 1:
		    printf("\t请输入修改姓名:");
		    scanf("%s",DO.name);
		    break;
		case 2:
		    printf("\t请输入修改登录名:");
		    scanf("%s",DO.log);
		    md5(DO.log);
		    break;
		case 3:
		    printf("\t请输入修改密码:");
		    scanf("%s",DO.password);
		    md5(DO.password);
		    break;
		case 4:
		    printf("\t请输入修改年龄:");
		    scanf("%d",&DO.age);
			getchar();
		    break;
		case 5:
		    printf("\t请输入修改科室:");
		    scanf("%s",DO.keshi);
		    break;
		default:
		    printf("\n选择有误,重新选择\n");
		    break;

	    }
	    printf("\t是否还要修改其它项?(y/n):");
	    if(!getchar_y())
		break;
	}
	printf("\t是否确认修改?(y/n):");
	if(getchar_y())
	{
	    ret = sendpackage(sockfd,"m1.0","y1.0",DOCTOR,MOD,sizeof(doc),&DO);
	    if(ret<0)
	    {
		printf("发送数据失败\n");
		return;
	    }

	    ret = readpackage_head(sockfd,&pack);
	    if(ret<0)
		return;
	    if(pack.dataopt == MOD_OK)
	    {
		printf("\t修改成功,是否修改其它?(y/n):");
		if(!getchar_y())
		    break;
	    }
	    else
	    {
		printf("\t修改失败,是否继续?(y/n):");
		if(!getchar_y())
		    return;
	    }
	}
	else
	{
	    printf("\t是否修改其它信息?(y/n):");
	    if(!getchar_y())
		return;
	}
    }

    return;
}


/********************  控制台对护士的操作界面  ***********************/
int root_nurmenu(int sockfd)
{


    int opt;
    int state = 0;
    for(;;)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[34;1m1.添加护士\n\n");
	printf("\t\t2.删除护士\n\n");
	printf("\t\t3.修改护士信息\n\n");
	printf("\t\t4.查看护士信息\n\n");
	printf("\t\t5.返回上一级\33[0m\n\n");
	printf("\t\t\33[30;1m0.退出\33[0m\n\n");
	printf("\t\t\33[33;1m请选择: \33[0m");
	scanf("%d",&opt);
	switch (opt)
	{
	    case 1:
		rootadd_nurse(sockfd);
		//getchar();
		break;
	    case 2:
		rootdel_nur(sockfd);
		break;
	    case 3:
		rootmod_nur(sockfd);
		break;
	    case 4:
		rootshow_nurse(sockfd);
		getchar();
		while(!getchar());
		break;
	    case 5:
		state=1;
		break;
	    case 0:
		close(sockfd);
		exit (0);
	    default:
		printf("\t\t输入有误,请重新选择");
		getchar();
		while(!getchar());
		break;
	}
	if(state==1)
	    break;
    }
    return 1;
}

/*****************  添加护士信息  ******************/
void rootadd_nurse(int sockfd)
{
    int ret;
    package_t pack;
    nur NU;
    
    char pass1[36];
    char pass2[36];
    
    for(;;)
    {
	printf("\t输入护士工号:");
	scanf("%d",&NU.id);
	printf("\t输入护士登录名:");
	scanf("%s",NU.log);
	md5(NU.log);
	printf("\t输入护士姓名:");
	scanf("%s",NU.name);
	printf("\t输入密码:");
	scanf("%s",pass1);
	printf("\t再次输入密码:");
	scanf("%s",pass2);
	if(!strcmp(pass1,pass2))
	{
	    md5(pass2);
	    strcpy(NU.password,pass2);
	    printf("\t输入性别:");
	    scanf("%s",NU.sex);
	    printf("\t输入年龄:");
	    scanf("%d",&NU.age);
	}
	else
	{
	    printf("两次输入密码不一致,注册失败\n");
	    getchar();
	    continue;
	}

	ret = sendpackage(sockfd,"m1.0","y1.0",NURSE,ADD,sizeof(nur),&NU);
	if(ret<0)
	{
	    printf("发送失败\n");
	    getchar();
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<=0)
	{
	    return;
	}

	if(pack.dataopt == ADD_OK)
	{
	    printf("\t添加护士成功,是否继续添加?(y/n):");
	    if(!getchar_y())
		return;
	}
	else
	{
	    printf("\t工号已存在,是否重新添加?(y/n):");
	    if(!getchar_y())
		return;
	}
    }
}

/*********************  删除护士信息 ******************/
void rootdel_nur(int sockfd)
{
    int ret,id;
    package_t pack;
    nur NU;
    for(;;)
    {
	printf("\t\t请输入要删除的护士工号:");
	scanf("%d",&id);
	getchar();
	ret = sendpackage(sockfd,"m1.0","y1.0",NURSE,SHOW,sizeof(int),&id);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	    return;

	if(pack.dataopt==SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&NU,sizeof(nur));
		if(NU.id==-1)
		    break;
		printf("工号: %d\t护士姓名: %s\t护士性别: %s\t护士年龄: %d\n",NU.id,NU.name,NU.sex,NU.age);
	    }
	}
	else
	{
	    printf("\t没有该护士信息\n");
	    getchar();
	    return;
	}

	printf("\t是否确定删除(y/n):");
	if(getchar_y())
	{
	    ret = sendpackage(sockfd,"m1.0","y1.0",NURSE,DEL,sizeof(int),&id);
	    if(ret<0)
	    {
		printf("发送数据错误\n");
		return;
	    }

	    ret = readpackage_head(sockfd,&pack);
	    if(ret<=0)
		return;
	    if(pack.dataopt == DEL_OK)
	    {
		printf("\t删除成功,是否删除其他护士信息(y/n):");
		if(!getchar_y())
		    break;
	    }

	    else
	    {
		printf("\t删除失败,是否继续(y/n):");
		if(!getchar_y())
		    return;
	    }
	}
	else
	{
	    printf("\t是否删除其它信息(y/n):");
	    if(!getchar())
		return;
	}
    }
    return;
}

/*************************  修改护士信息  **********************/
void rootmod_nur(int sockfd)
{
    int ret,id;
    package_t pack;
    nur NU;
   
    for(;;)
    {
	printf("\t请输入要修改护士的工号:");
	scanf("%d",&id);
	getchar();
	ret = sendpackage(sockfd,"m1.0","y1.0",NURSE,SHOW,sizeof(int),&id);
	if(ret <0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	    return;

	if(pack.dataopt == SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&NU,sizeof(nur));
		if(NU.id==-1)
		    break;
		printf("工号: %d\t护士姓名: %s\t护士性别: %s\t医生年龄: %d\n",NU.id,NU.name,NU.sex,NU.age);

	    }
	}
	else
	{
	    printf("\t没有该护士信息\n");
		getchar();
	    return;
	}

	for(;;)
	{
	    NU.id = id;
	    printf("\t请选择修改选项: 1.姓名 2.登录名 3.密码 4.年龄\n\n");
	    printf("\t\t选择:");
	    int opt;
	    scanf("%d",&opt);
	    switch (opt)
	    {
		case 1:
		    printf("\t请输入修改姓名:");
		    scanf("%s",NU.name);
		    break;
		case 2:
		    printf("\t请输入登录名:");
		    scanf("%s",NU.log);
		    md5(NU.log);
		    break;
		case 3:
		    printf("\t请输入修改密码:");
		    scanf("%s",NU.password);
		    md5(NU.password);
		    break;
		case 4:
		    printf("\t请输入修改年龄:");
		    scanf("%d",&NU.age);
		    break;
		default:
		    printf("\n选择有误,重新选择\n");
		    break;

	    }
	    printf("\t是否还要修改其它项?(y/n):");
	    if(!getchar_y())
		break;
	}
	printf("\t是否确认修改?(y/n):");
	if(getchar_y())
	{
	    ret = sendpackage(sockfd,"m1.0","y1.0",NURSE,MOD,sizeof(nur),&NU);
	    if(ret<0)
	    {
		printf("发送数据失败\n");
		return;
	    }

	    ret = readpackage_head(sockfd,&pack);
	    if(ret<0)
		return;
	    if(pack.dataopt == MOD_OK)
	    {
		printf("\t修改成功,是否修改其它?(y/n):");
		if(!getchar_y())
		    break;
	    }
	    else
	    {
		printf("\t修改失败,是否继续?(y/n):");
		if(!getchar_y())
		    return;
	    }
	}
	else
	{
	    printf("\t是否修改其它信息?(y/n):");
	    if(!getchar_y())
		return;
	}
    }

    return;
}

/************************   查看护士信息  ***********************/
void rootshow_nurse(int sockfd)
{
    int ret,id;  //id为零,查看全部的护士信息
    package_t pack;
    nur NU;
    printf("\t\t请选择查看类型:  输入非0按id查看,输入0查看所有护士信息:");
    printf("\n\n");
    printf("\t\t请选择:");
    scanf("%d",&id);
    for(;;)
    {
	ret = sendpackage(sockfd,"m1.0","y1.0",NURSE,SHOW,sizeof(id),&id);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<=0)
	    return;
	if(pack.dataopt==SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&NU,sizeof(nur));
		if(NU.id==-1)
		    return;
		printf("工号: %d\t姓名: %s\t性别: %s\t年龄: %d\n",NU.id,NU.name,NU.sex,NU.age);
	    }
	}
	else
	{
	    printf("\t没有对应信息\n");
	    return;
	}
    }
    return;
}


/*************************  控制台对药品的操作界面  ********************/
int root_medmenu(int sockfd)
{
    int opt;
    int state = 0;
    for(;;)
    {
	system("clear");
	menu_head();
	printf("\t\t\33[34;1m1.添加药品\n\n");
	printf("\t\t2.删除药品\n\n");
	printf("\t\t3.修改药品\n\n");
	printf("\t\t4.查看药品\n\n");
	printf("\t\t5.返回上一级\33[0m\n\n");
	printf("\t\t\33[30;1m0.退出\33[0m\n\n");
	printf("\t\t\33[33;1m请选择: \33[0m");
	scanf("%d",&opt);
	switch (opt)
	{
	    case 1:
		rootadd_med(sockfd);
		break;
	    case 2:
		rootdel_med(sockfd);
		break;
	    case 3:
		rootmod_med(sockfd);
		break;
	    case 4:
		rootshow_med(sockfd);
		getchar();
		while(!getchar());
		break;
	    case 5:
		state = 1;
		break;
	    case 0:
		close(sockfd);
		exit(0);
	    default:
		printf("\t\t输入有误,请重新输入");
		getchar();
		while(!getchar());
		break;
		
	}
	if(state==1)
	    break;
    }
    return 1;
    
}

/******************** 添加药品信息  ****************/
void rootadd_med(int sockfd)
{
    int ret;
    package_t pack;
    med ME;
    char ch;
    for(;;)
    {
	printf("\n");
	printf("\t输入药的编号:");
	scanf("%d",&ME.id);
	printf("\t输入药名:");
	scanf("%s",ME.name);
	printf("\t输入药的价格:");
	scanf("%f",&ME.price);
	printf("\t输入药的类型:");
	scanf("%s",ME.type);
	while(1)
	{
	    printf("\t请设置优惠政策: a.不打折  b.打九折  c.打八折\n\n");
		getchar();
	    printf("\t请选择:");
	    ch = getchar();
	    if(ch == 'a')
	    {
		ME.discount = 1;
		break;
	    }
	    else if(ch == 'b')
	    {
		ME.discount = 0.9;
		break;
	    }
	    else if(ch == 'c')
	    {
		ME.discount = 0.8;
		break;
	    }
	    else
		printf("\t输入错误,请重新设置\n");
	}
	ret = sendpackage(sockfd,"m1.0","y1.0",MEDICINE,ADD,sizeof(med),&ME);
	if(ret<0)
	{
	    printf("发送失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	{
	    return;
	}

	if(pack.dataopt==ADD_OK)
	{
	    printf("\t药品添加成功,是否继续添加?(y/n):");
	    if(!getchar_y())
		return;

	}
	else
	{
	    printf("\t药品编号重复,是否重新添加?(y/n):");
	    if(!getchar_y())
		return;
	}

    }
}


/****************  删除药品信息  ****************/
void rootdel_med(int sockfd)
{
    int ret,id;
    package_t pack;
    med ME;
    for(;;)
    {
	printf("\t\t请输入要删除的药品编号:");
	scanf("%d",&id);
	ret = sendpackage(sockfd,"m1.0","y1.0",MEDICINE,SHOW,sizeof(int),&id);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	    return;

	if(pack.dataopt==SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&ME,sizeof(med));
		if(ME.id==-1)
		    break;
		printf("药编号: %d\t药名: %s\t药价(rmb): %.1f\t药类型: %s",ME.id,ME.name,ME.price,ME.type);
		if(ME.discount==1)
		    printf("\t折扣: 不打折\n");
		else
		    printf("\t折扣: 打 %.1f折\n",ME.discount*10);
		//printf("药编号: %d\t药名: %s\t药价(rmb): %.1f\t药类型: %s\t折扣: %.1f\n",ME.id,ME.name,ME.price,ME.type,ME.discount);
	    }
	}
	else
	{
	    printf("\t没有该药品信息\n");
	    getchar();
	    while(!getchar());
	    return;
	}

	printf("\t是否确定删除(y/n):");
	if(getchar_y())
	{
	    ret = sendpackage(sockfd,"m1.0","y1.0",MEDICINE,DEL,sizeof(int),&id);
	    if(ret<0)
	    {
		printf("发送数据错误\n");
		return;
	    }

	    ret = readpackage_head(sockfd,&pack);
	    if(ret<=0)
		return;
	    if(pack.dataopt == DEL_OK)
	    {
		printf("\t删除成功,是否删除其他药品信息(y/n):");
		if(!getchar_y())
		    break;
	    }

	    else
	    {
		printf("\t删除失败,是否继续(y/n):");
		if(!getchar_y())
		    return;
	    }
	}
	else
	{
	    printf("\t是否删除其它信息(y/n):");
	    if(!getchar())
		return;
	}
    }
    return;
}


/***********************  修改药品信息  ****************/
void rootmod_med(int sockfd)
{
    int ret,id;
    package_t pack;
    med ME;
    for(;;)
    {
	printf("\t请输入要修改药品的编号:");
	scanf("%d",&id);
	getchar();
	ret = sendpackage(sockfd,"m1.0","y1.0",MEDICINE,SHOW,sizeof(int),&id);
	if(ret <0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	    return;

	if(pack.dataopt == SHOW_OK)
	{
	    for(;;)
	    {
		ret = read(sockfd,&ME,sizeof(med));
		if(ME.id==-1)
		    break;
		printf("药编号: %d\t药名: %s\t药价(rmb): %.1f\t药类型: %s\t折扣: %.1f\n",ME.id,ME.name,ME.price,ME.type,ME.discount);
	    }
	}
	else
	{
	    printf("\t没有该药品信息\n");
		getchar();
	    return;
	}

	for(;;)
	{
	    ME.id = id;
	    printf("\t请选择修改选项: 1.药名 2.药价 3.药类型\n\n");
	    printf("\t\t选择:");
	    int opt;
	    scanf("%d",&opt);
	    switch (opt)
	    {
		case 1:
		    printf("\t请输入修改药名:");
		    scanf("%s",ME.name);
		    break;
		case 2:
		    printf("\t请输入修改密码:");
		    scanf("%f",&ME.price);
		    break;
		case 3:
		    printf("\t请输入修改药类型:");
		    scanf("%s",ME.type);
		    break;
		default:
		    printf("\n选择有误,重新选择\n");
		    break;

	    }
	    printf("\t是否还要修改其它项?(y/n):");
	    if(!getchar_y())
		break;
	}
	printf("\t是否确认修改?(y/n):");
	if(getchar_y())
	{
	    ret = sendpackage(sockfd,"m1.0","y1.0",MEDICINE,MOD,sizeof(med),&ME);
	    if(ret<0)
	    {
		printf("发送数据失败\n");
		return;
	    }

	    ret = readpackage_head(sockfd,&pack);
	    if(ret<0)
		return;
	    if(pack.dataopt == MOD_OK)
	    {
		printf("\t修改成功,是否修改其它?(y/n):");
		if(!getchar_y())
		    break;
	    }
	    else
	    {
		printf("\t修改失败,是否继续?(y/n):");
		if(!getchar_y())
		    return;
	    }
	}
	else
	{
	    printf("\t是否修改其它信息?(y/n):");
	    if(!getchar_y())
		return;
	}
    }

    return;
}

/********************  查看药品信息  ***********************/
void rootshow_med(int sockfd)
{
    int ret,id=0;
    package_t pack;
    med ME;
    for(;;)
    {
	ret = sendpackage(sockfd,"m1.0","y1.0",MEDICINE,SHOW,sizeof(id),&id);
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
	    for(;;)
	    {
		ret = read(sockfd,&ME,sizeof(med));
		if(ME.id==-1)
		    return;
		
		printf("药编号: %d\t药名: %s\t药价(rmb): %.1f\t药类型: %s",ME.id,ME.name,ME.price,ME.type);
		if(ME.discount==1)
		    printf("\t折扣: 不打折\n");
		else
		    printf("\t折扣: 打 %.1f折\n",ME.discount*10);
		
		//printf("药编号: %d\t药名: %s\t药价(rmb): %.1f\t药类型: %s\t折扣: %.1f\n",ME.id,ME.name,ME.price,ME.type,ME.discount);
	    }
	}
	else
	{
	    printf("没有信息,读取错误\n");
	    return;
	}
    }
    return;
}


/***********************  患者挂号  *******************/
void guahao_menu(int sockfd)
{
    int ret;
    int yibao;
    package_t pack;
    char ch;
    char ch1;
    pat PA;
    for(;;)
    {
	printf("\t输入患者id:");
	scanf("%d",&PA.id);
	getchar();
	while(1)
	{
	    printf("\t是否有医保?(y/n):");
	    ch=getchar();
	    if(ch == 'y')
	    {
		printf("\t输入医保帐号:");
		scanf("%d",&yibao);
		PA.flag = 3;
		break;
	    }
	    else if(ch =='n')
	    {
		PA.flag=1;
		break;
	    }
	    else
		printf("\t输入错误,请重新选择\n\n");

	}
	printf("\t输入患者姓名:");
	scanf("%s",PA.name);
	printf("\t输入患者性别:");
	scanf("%s",PA.sex);
	printf("\t输入患者年龄:");
	scanf("%d",&PA.age);
	getchar();
	printf("\t输入患者病情:");
	scanf("%s",PA.condition);
	strcpy(PA.advice,"noadvice");
	PA.money = 0;
	printf("\t收费项目:\n");
	if(PA.flag==3)
	{
	    printf("\t该患者有医保,无需缴费\n");
	}
	else if(PA.flag ==1)
	{
	    while(1)
	    {
		printf("\t患者是否缴费?(y/n):");
		getchar();
		ch1 = getchar();
		if(ch1=='y')
		{
		    printf("\t患者缴费成功\n");
		    PA.flag = 2;
		    break;
		}
		else if(ch1=='n')
		{
		    printf("\t患者未缴费\n");
		    PA.flag = 1;
		    break;
		}
		else
		    printf("\t输入错误,请重新选择\n");

	    }
	}
	ret = sendpackage(sockfd,"m1.0","y1.0",PATIENT,ADD,sizeof(pat),&PA);
	if(ret<0)
	{
	    printf("发送数据失败\n");
	    return;
	}

	ret = readpackage_head(sockfd,&pack);
	if(ret<0)
	{
	    return;
	}

	if(pack.dataopt == ADD_OK)
	{
	    printf("\t该患者挂号成功,是否继续挂号?(y/n):");
	    if(!getchar_y())
		return;
	}
	else
	{
	    printf("\t该患者id已经存在,是否重新挂号?(y/n):");
	    if(!getchar_y())
		return;
	}
    }
}


/*****************  控制台客户端搭建  ***************************/
int main()
{
    int sockfd,ret;
    struct sockaddr_in serveraddr;

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

    ret = connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    assert(ret==0);

    if(!login())
    {
	printf("登录失败\n");
	return -1;
    }

    else
    {
	printf("\n");
	printf("\t\t登录成功\n");
	sleep(1);
	root_menu(sockfd);

    }
    return 0;
}
