#include"common.h"
#include<signal.h>
#include"server.h"
#include"pool.h"
#include"rootCtl.h"




/******************* 线程运行时的执行函数  ***************/
void *server_func(void *argv)
{
    MYSQL mysql;
    mysql_connect(&mysql);  //数据库连接
    int ret,clientfd;
    struct epoll_event *events = malloc(sizeof(struct epoll_event));
    events = (struct epoll_event*)argv;
    user_data *current = events->data.ptr;
    clientfd = current->fd;
    package_t *cpack = (package_t *)(current->data);
    if(cpack->type == DOCTOR)   //对医生的操作
    {
	switch (cpack->dataopt)
	{
	    case LOG:
		doc_log(clientfd,cpack,mysql);//医生登录
		break;
	    case ADD:
		add_doc(clientfd,cpack,mysql); //添加医生
		break;
	    case DEL:
		del_doc(clientfd,cpack,mysql);//删除医生信息
		break;
	    case MOD:
		mod_doc(clientfd,cpack,mysql);//修改医生信息
		break;
	    case SHOW:
		show_doc(clientfd,cpack,mysql);//查看医生信息
		break;
	    default:
		printf("数据有误\n");
		break;
	}
    }
    else if(cpack->type==NURSE) //对护士的操作
    {
	switch (cpack->dataopt)
	{
	    case LOG:
		nur_log(clientfd,cpack,mysql);//护士登录
		break;
	    case ADD:
		add_nur(clientfd,cpack,mysql); //添加护士
		break;
	    case DEL:
		del_nur(clientfd,cpack,mysql);//删除护士信息
		break;
	   case MOD:
		mod_nur(clientfd,cpack,mysql);//修改护士信息
		break;
	    case SHOW:
	    show_nur(clientfd,cpack,mysql);//查看护士信息
		break;
	    default:
		printf("数据有误\n");
		break;
	}	
    }
    else if(cpack->type==MEDICINE)//对药品操作
    {
	switch (cpack->dataopt)
	{
	    case ADD:
		add_med(clientfd,cpack,mysql); //添加药品
		break;
	    case DEL:
		del_med(clientfd,cpack,mysql);  //删除药品
		break;
	    case MOD:
		mod_med(clientfd,cpack,mysql);  //修改药品
		break;
	    case SHOW:
		show_med(clientfd,cpack,mysql); //查看药品
		break;
	    case FIND:
		find_med(clientfd,cpack,mysql);  //医生开药时查找药品
		break;
	    default:
		printf("数据有误\n");
		break;
	}
    }

    else if(cpack->type==PATIENT) //对患者的操作
    {
	switch(cpack->dataopt)
	{
	    case ADD:
		guahao(clientfd,cpack,mysql);  //患者挂号
		break;
	    case SHOW:
		show_pat(clientfd,cpack,mysql); //查看患者
		break;
	    case CURE:
		cure_pat(clientfd,cpack,mysql);  //医生初步诊断患者	
		break;
	    case NUR:
		nur_pat(clientfd,cpack,mysql);  //护士对患者执行医嘱
		break;
            case MOD:
 		seller_pat(clientfd,cpack,mysql);//控制台收费
		break;
	    case FIND:
		find_pat(clientfd,cpack,mysql);  //搜索要诊断的患者id
		break;
	    case LOG:
		 pat_log(clientfd,cpack,mysql);
		 break;                        //患者登录
	    case UPD:
		 pat_upd(clientfd,cpack,mysql); //upd
		break;
	}
    }

    else if(cpack->type==ASSAY) //对化验的操作
    {
	switch(cpack->dataopt)
	{
	    case ADD:
		add_assay(clientfd,cpack,mysql); //增加化验项目
		break;
	    case DEL:
		del_assay(clientfd,cpack,mysql); //删除化验项目 
		break;
	    case MOD:
		mod_assay(clientfd,cpack,mysql); //修改化验项目
		break;
	    case SHOW:
		show_assay(clientfd,cpack,mysql); //查看化验项目
		break;
	    case FIND:
		find_assay(clientfd,cpack,mysql); //搜索要使用的化验项目
		break;
	}
    }
  
    else if(cpack->type==TABLE)   //诊断患者后对医疗表的操作
    {
	switch(cpack->dataopt)
	{
	    case ADD:
		add_table(clientfd,cpack,mysql);   //医疗表的增加
		break;
	    case SHOW:
		show_table(clientfd,cpack,mysql);  //医疗表查看
		break;
	    case DONE:
		do_table(clientfd,cpack,mysql); //护士执行医嘱后对医疗表的改变
		break;
	}
    }

    mysql_close(&mysql);
}

/**********  控制台收费  *********/
void seller_pat(int clientfd,package_t *cpack,MYSQL mysql)
{
    pat *PA =(pat*)(cpack->data);
    char str[200];
    sprintf(str,"update patient set flag = %d where id = %d",PA->flag,PA->id);

    cpack->length=0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt=MOD_OK;
    else
	cpack->dataopt=MOD_FL;

    int ret= write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}

/**************  搜索要诊断的患者id  *****************/
void find_pat(int clientfd,package_t *cpack,MYSQL mysql)
{
    pat *PA = (pat *)(cpack->data);
    char str[200];
    sprintf(str,"select *from patient where id = %d",PA->id);
    mysql_select_patient(&mysql,clientfd,cpack,str);
}


/************ 医生要开的化验项目搜索  ****************/
void find_assay(int clientfd,package_t *cpack,MYSQL mysql)
{
    assay *AS=(assay *)(cpack->data);
    char str[200];
    sprintf(str,"select * from assay where id = %d",AS->id);
  //  printf("%s\n",str);
    mysql_select_assay(&mysql,clientfd,cpack,str);
}


/***********  医生开药时搜索需要的药  ****************/
void find_med(int clientfd,package_t *cpack,MYSQL mysql)
{
    med *ME=(med *)(cpack->data);
    char str[200];
    sprintf(str,"select * from medicine where id = %d",ME->id);
  //  printf("%s\n",str);
    mysql_select_medicine(&mysql,clientfd,cpack,str);
}
/******************  护士客户端请求登录服务器的回应 **********/
void nur_log(int clientfd,package_t *cpack,MYSQL mysql)
{
    log_t *log = (log_t *)(cpack->data);
    char str[200];
    sprintf(str,"select *from nurse where nur_log = '%s' and nur_pass = '%s'",log->name,log->password);
    cpack->length =0;
    if(mysql_log(str,&mysql))
	cpack->dataopt=LOG_OK;
    else
	cpack->dataopt=LOG_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("数据发送失败\n");
}


/******************* 护士执行医嘱服务器回应 **********/
void do_table(int clientfd,package_t *cpack,MYSQL mysql)
{
    cure_table *AS =(cure_table*)(cpack->data);
    char str[200];
    sprintf(str,"update cure_table set flag = %d where pid = %d",AS->flag,AS->pid);

    cpack->length=0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt=DONE_OK;
    else
	cpack->dataopt=DONE_FL;

    int ret= write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}


/******************** 查看医嘱服务器做的回应 ***************/
void show_table(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id =(int*)(cpack->data);
    char str[200];
    if((*id)==0)
	sprintf(str,"select *from cure_table");
    else
	sprintf(str,"select *from cure_table where pid = %d",*id);
    mysql_select_table(&mysql,clientfd,cpack,str);
}


/******************** 医生诊断患者时 开药或者化验项目 对医疗表信息的增加 *****/
void add_table(int clientfd,package_t *cpack,MYSQL mysql)
{
    cure_table *CU = (cure_table *)(cpack->data);
    char str[200];
    sprintf(str,"insert into cure_table values (%d,%d,%d,%f,%d)",CU->pid,CU->mid,CU->num,CU->price,CU->flag);
    cpack->length = 0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt = ADD_OK;
    else
	cpack->dataopt = ADD_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("数据发送失败\n");
}



/***************  护士执行医嘱时对患者信息状态的改变   *****************/
void nur_pat(int clientfd,package_t *cpack,MYSQL mysql)
{
    nur_cur *NU =(nur_cur*)(cpack->data);
    char str[200];
    sprintf(str,"update patient set flag = %d where id = %d",NU->flag,NU->id);

    cpack->length=0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt=NUR_OK;
	//printf("%d\n",cpack->dataopt);
    else
	cpack->dataopt=NUR_FL;
    //strcpy(cpack->version, "hello"); 
	//printf("version = %s\n",cpack->version);   
    int ret= write(clientfd,cpack,sizeof(package_t));
	//printf("ret=%d,headopt = %d\n",ret, cpack->dataopt);
    if(ret<0)
	printf("发送数据失败\n");
}


/************  医生诊疗患者时对患者信息状态的改变  ***************/
void cure_pat(int clientfd,package_t *cpack,MYSQL mysql)
{
    doc_cur *DC =(doc_cur*)(cpack->data);
    char str[200];
    sprintf(str,"update patient set money = %f,pat_adv = '%s',flag = %d where id = %d",DC->money,DC->advice,DC->flag,DC->id);

    cpack->length=0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt=CURE_OK;
	//printf("%d\n",cpack->dataopt);
    else
	cpack->dataopt=CURE_FL;
    //strcpy(cpack->version, "hello"); 
	//printf("version = %s\n",cpack->version);   
    int ret= write(clientfd,cpack,sizeof(package_t));
	printf("ret=%d,dataoptopt = %d\n",ret, cpack->dataopt);
    if(ret<0)
	printf("发送数据失败\n");
}




/************** 查看病人 *********************/
void show_pat(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id =(int*)(cpack->data);
    char str[200];
    if((*id)==0)
	sprintf(str,"select *from patient");
    else
	sprintf(str,"select *from patient where id = %d",*id);
    mysql_select_patient(&mysql,clientfd,cpack,str);

    printf("show_pat cpack->dataopt = %d\n",cpack->dataopt);
}

/**********************  医生登录  *********************/
void doc_log(int clientfd,package_t *cpack,MYSQL mysql)
{
    log_t *log = (log_t *)(cpack->data);
    char str[200];
    sprintf(str,"select *from doctor where doc_log = '%s' and doc_pass = '%s'",log->name,log->password);
    cpack->length =0;
    if(mysql_log(str,&mysql))
	cpack->dataopt=LOG_OK;
    else
	cpack->dataopt=LOG_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("数据发送失败\n");
}

/**********************  患者登录  *********************/
void pat_log(int clientfd,package_t *cpack,MYSQL mysql)
{
    log_t *log = (log_t *)(cpack->data);
    char str[200];
    sprintf(str,"select * from patient where id = %d and pat_name = '%s'",atoi(log->name),log->password);
    cpack->length =0;
   // printf("%d\n",log->name);
   // printf("%s\n",log->password);
    if(mysql_log(str,&mysql))
	cpack->dataopt=LOG_OK;
    else
	cpack->dataopt=LOG_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("数据发送失败\n");
    return ;
}

/***************  添加化验项目  *******************/
void add_assay(int clientfd,package_t *cpack,MYSQL mysql)
{
    assay *AS = (assay *)(cpack->data);
    char str[200];
    sprintf(str,"insert into assay values (%d,'%s',%f)",AS->id,AS->name,AS->price);
    cpack->length = 0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt = ADD_OK;
    else
	cpack->dataopt = ADD_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("数据发送失败\n");
}


/****************** 删除化验项目  **************/
void del_assay(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id = (int*)(cpack->data);
    char str[200];
    sprintf(str,"delete from assay where id = %d",*id);
    cpack->length = 0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt = DEL_OK;
    else
	cpack->dataopt = DEL_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}

/*****************  修改化验项目  *****************/
void mod_assay(int clientfd,package_t *cpack,MYSQL mysql)
{
    assay *AS =(assay*)(cpack->data);
    char str[200];
    sprintf(str,"update assay set assay_name = '%s',assay_price = %f where id = %d",AS->name,AS->price,AS->id);

    cpack->length=0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt=MOD_OK;
    else
	cpack->dataopt=MOD_FL;

    int ret= write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}


/*****************   查看化验项目  ******************/
void show_assay(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id =(int*)(cpack->data);
    char str[200];
    if((*id)==0)
	sprintf(str,"select *from assay");
    else
	sprintf(str,"select *from assay where id = %d",*id);
    mysql_select_assay(&mysql,clientfd,cpack,str);
}


/****************  查看医生信息  **********************/
void show_doc(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id =(int*)(cpack->data);
    char str[200];
    if((*id)==0)
	sprintf(str,"select *from doctor");
    else
	sprintf(str,"select *from doctor where id = %d",*id);
    mysql_select_doctor(&mysql,clientfd,cpack,str);
}


/*****************  添加医生信息  **********************/
void add_doc(int clientfd,package_t *cpack,MYSQL mysql)
{
    doc *DO = (doc *)(cpack->data);
	//printf("%d   %s  %s   %s  %s  %d  %s\n",DO->id,DO->log,DO->name,DO->password,DO->sex,DO->age,DO->keshi);
    char str[200];
    sprintf(str,"insert into doctor values (%d,'%s','%s','%s','%s',%d,'%s')",DO->id,DO->log,DO->name,DO->password,DO->sex,DO->age,DO->keshi);
    cpack->length = 0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt = ADD_OK;
    else
	cpack->dataopt = ADD_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("数据发送失败\n");
}


/*******************  删除医生信息  ********************/
void del_doc(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id = (int*)(cpack->data);
    char str[200];
    sprintf(str,"delete from doctor where id = %d",*id);
    cpack->length = 0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt = DEL_OK;
    else
	cpack->dataopt = DEL_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}


/********************  修改医生信息  *********************/
void mod_doc(int clientfd,package_t *cpack,MYSQL mysql)
{
    doc *DO =(doc*)(cpack->data);
    char str[200];
    sprintf(str,"update doctor set doc_name = '%s',doc_log = '%s',doc_pass = '%s',doc_age = %d,doc_keshi ='%s' where id = %d",DO->name,DO->log,DO->password,DO->age,DO->keshi,DO->id);

    cpack->length=0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt=MOD_OK;
    else
	cpack->dataopt=MOD_FL;

    int ret= write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}

void pat_upd(int clientfd,package_t *cpack,MYSQL mysql)
{
	pat *PT =(pat*)(cpack->data);
	char str[200];
	sprintf(str,"update patient set pat_name = '%s' where id = %d",PT->name,PT->id);
	cpack->length=0;
	if(mysql_insert(str,&mysql))
	    cpack->dataopt=UPD_OK;
	else
	    cpack->dataopt=UPD_FL;
	int ret= write(clientfd,cpack,sizeof(package_t));
	printf("%d\n",cpack->dataopt);
	if(ret<0)
		printf("发送数据失败\n");

}

/******************   添加护士信息  *********************/
void add_nur(int clientfd,package_t *cpack,MYSQL mysql)
{
    nur *NU = (nur *)(cpack->data);
    char str[200];
    sprintf(str,"insert into nurse values (%d,'%s','%s','%s','%s',%d)",NU->id,NU->log,NU->name,NU->password,NU->sex,NU->age);
    cpack->length = 0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt = ADD_OK;
    else
	cpack->dataopt = ADD_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("数据发送失败\n");
}


/*******************  删除护士信息  *************/
void del_nur(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id = (int*)(cpack->data);
    char str[200];
    sprintf(str,"delete from nurse where id = %d",*id);
    cpack->length = 0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt = DEL_OK;
    else
	cpack->dataopt = DEL_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}

/*********************  修改护士信息  *************/
void mod_nur(int clientfd,package_t *cpack,MYSQL mysql)
{
    nur *NU =(nur*)(cpack->data);
    char str[200];
    sprintf(str,"update nurse set nur_name = '%s',nur_log = '%s',nur_pass = '%s',nur_age = %d where id = %d",NU->name,NU->log,NU->password,NU->age,NU->id);

    cpack->length=0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt=MOD_OK;
    else
	cpack->dataopt=MOD_FL;

    int ret= write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}

/**************  查看护士信息  **********************/
void show_nur(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id =(int*)(cpack->data);
    char str[200];
    if((*id)==0)
	sprintf(str,"select *from nurse");
    else
	sprintf(str,"select *from nurse where id = %d",*id);
    mysql_select_nurse(&mysql,clientfd,cpack,str);
}


/***********  添加药品  *******************/
void add_med(int clientfd,package_t *cpack,MYSQL mysql)
{
    med *ME = (med*)(cpack->data);
    char str[200];
    sprintf(str,"insert into medicine values (%d,'%s',%f,'%s',%f)",ME->id,ME->name,ME->price,ME->type,ME->discount);
    
    cpack->length=0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt = ADD_OK;
    else
	cpack->dataopt = ADD_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");

}

/****************  删除药品信息  **************/
void del_med(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id = (int*)(cpack->data);
    char str[200];
    sprintf(str,"delete from medicine where id = %d",*id);
    cpack->length = 0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt = DEL_OK;
    else
	cpack->dataopt = DEL_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}


/******************  修改药品信息  ********************/
void mod_med(int clientfd,package_t *cpack,MYSQL mysql)
{
    med *ME =(med*)(cpack->data);
    char str[200];
    sprintf(str,"update medicine set med_name = '%s',med_type = '%s',med_price = %.1f where id = %d",ME->name,ME->type,ME->price,ME->id);

    cpack->length=0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt=MOD_OK;
    else
	cpack->dataopt=MOD_FL;

    int ret= write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");
}

/******************  查看药品信息  ****************/
void show_med(int clientfd,package_t *cpack,MYSQL mysql)
{
    int *id = (int *)(cpack->data);
    char str[200];
    if((*id)==0)
	sprintf(str,"select *from medicine");
    else
	sprintf(str,"select *from medicine where id= %d",*id);
    mysql_select_medicine(&mysql,clientfd,cpack,str);
}


/*******************  患者挂号   ********************/
void guahao(int clientfd,package_t *cpack,MYSQL mysql)
{
    pat *PA = (pat *)(cpack->data);
    char str[200];
    sprintf(str,"insert into patient values (%d,'%s','%s',%d,'%s','%s',%f,%d)",PA->id,PA->name,PA->sex,PA->age,PA->condition,PA->advice,PA->money,PA->flag);
    cpack->length = 0;
    if(mysql_insert(str,&mysql))
	cpack->dataopt=ADD_OK;
    else
	cpack->dataopt=ADD_FL;
    int ret = write(clientfd,cpack,sizeof(package_t));
    if(ret<0)
	printf("发送数据失败\n");

}

/*******************  将套接字文件设置为非阻塞形式 ********/
void setNonBlock(int fd)
{
    int flags;
    flags = fcntl(fd,F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fd,F_SETFL,flags);
    return;
}


/******************  服务器的搭建   ***************/
int main()
{
    int listenfd,clientfd,epollfd,ret,nfound,port1;
    struct sockaddr_in serveraddr,clientaddr;
    struct epoll_event ev,events[200];
    socklen_t len = sizeof(clientfd);
    char cmd[50],temp[20];

    char ip[50];
    char port[50];

    ret = getconf("ip",ip);
    assert(ret==0);
	
    ret = getconf("port",port);
    assert(ret==0);

    listenfd = socket(AF_INET,SOCK_STREAM,0);
    assert(listenfd>=0);

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(port));
    inet_pton(AF_INET,ip,&serveraddr.sin_addr.s_addr);
    
    setNonBlock(listenfd);
    ret = bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    assert(ret==0);

    ret = listen(listenfd,20);
    assert(ret==0);

    epollfd = epoll_create(200);
    assert(epollfd>=0);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = STDIN_FILENO;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,STDIN_FILENO,&ev);

    if(signal(SIGPIPE,SIG_IGN)==SIG_ERR)
    {
	printf("\t\t管道信号忽略失败\n");
	return -1;
    }

    pool_t pool;
    package_t pack;
    ret = thread_pool_init(&pool,8);
    for(;;)
    {
	printf("epoll_wait......\n");
	nfound = epoll_wait(epollfd,events,100,-1);
	if(nfound<0)
	{
	    perror("epoll_wait");
	    continue;
	}

	for(int i =0;i<nfound;i++)
	{
	    if(events[i].data.fd == STDIN_FILENO)
	    {
		ret = read(STDIN_FILENO,cmd,sizeof(cmd)-1);
		if(ret < 0)
		{
		    perror("read");
		    continue;
		}
		cmd[ret-1] = 0;
		if(!strcmp(cmd,"exit") || !strcmp(cmd,"quit"))
		{
		    close(epollfd);
		    exit(0);
		}
		else
		    system(cmd);
	    }
	    else if(events[i].data.fd == listenfd)
	    {
		for(;;)
		{
		    clientfd = accept(listenfd,(struct sockaddr*)&clientaddr,&len);
		    if(clientfd<0)
		    {
			if(clientfd == EAGAIN || errno == EWOULDBLOCK)
			    break;
			perror("accept");
			continue;
		    }
		    port1 = ntohs(clientaddr.sin_port);
		    inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ip,sizeof(ip));
		    printf("客户端[%s:%d]建立连接\n",ip,port1);

		    setNonBlock(clientfd);
		    ev.events = EPOLLIN | EPOLLET;
		    ev.data.fd = clientfd;
		    epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);
		}
	    }
	    else if(events[i].events & EPOLLIN)
	    {
		clientfd = events[i].data.fd;

		getpeername(clientfd,(struct sockaddr*)&clientaddr,&len);
		inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,ip,sizeof(ip));
		port1 = ntohs(clientaddr.sin_addr.s_addr);
		printf("读取客户端[%s:%d]数据\n",ip,port1);

		ret = read(clientfd,&pack,sizeof(package_t));
		if(ret<0)
		{
		    printf("读取客户端数据失败%s%d\n",__FILE__,__LINE__);
		    continue;
		}
		else if(ret == 0)
		{
		    printf("客户端断开连接%s%d\n",__FILE__,__LINE__);
		    ev.events = EPOLLIN | EPOLLET;
		    ev.data.fd = clientfd;
		    epoll_ctl(epollfd,EPOLL_CTL_DEL,clientfd,&ev);
		    close(clientfd);
		    continue;
		}

		printf("软件版本: %s \t协议: %s\n",pack.version,pack.protocol);
		printf("type : %d \t dataopt: %d\n",pack.type,pack.dataopt);

		//printf("malloc sizeof .......size = %d  pack.length=%d \n",sizeof(pack),pack.length);

		package_t *ptr_pack = malloc(sizeof(pack)+pack.length);
		if(ptr_pack==NULL)
		{
		    printf("申请空间失败%s%d\n",__FILE__,__LINE__);
		    continue;
		}
		memcpy(ptr_pack,&pack,sizeof(pack));
		ret = read(clientfd,ptr_pack->data,pack.length);
		if(ret<0)
		{
		    printf("读取客户端数据失败%s%d\n",__FILE__,__LINE__);
		    continue;
		}

		else if(ret==0)
		{
		    printf("客户端断开连接%s%d\n",__FILE__,__LINE__);
		    ev.events = EPOLLIN | EPOLLET;
		    ev.data.fd = clientfd;
		    epoll_ctl(epollfd,EPOLL_CTL_DEL,STDIN_FILENO,&ev);
		    close(clientfd);
		    continue;
		}

		user_data *user = malloc(sizeof(user_data));
		if(user==NULL)
		{
		    printf("申请空间失败\n");
		    continue;
		}

		user->fd = clientfd;
		user->data = ptr_pack;

		ev.events = EPOLLOUT | EPOLLET;
		ev.data.ptr = user;
		epoll_ctl(epollfd,EPOLL_CTL_MOD,clientfd,&ev);
	    
	    }
	    else if(events[i].events&EPOLLOUT)
	    {
		thread_pool_addworker(&pool,server_func,&events[i],sizeof(struct epoll_event));
		clientfd = ((user_data*)events[i].data.ptr)->fd;

		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = clientfd;
		epoll_ctl(epollfd,EPOLL_CTL_MOD,clientfd,&ev);
	    }
	}
    }
    
    close(listenfd);
    close(epollfd);
    thread_pool_destroy(&pool);

    return 0;
}
