#include"mysql.h"
#include"server.h"



/************  连接数据库 **************/
void mysql_connect(MYSQL *mysql)
{
    mysql_init(mysql);
    char host[20];
    char database[20];
    int ret;
    ret = getconf("mysql_host",host); //获取数据库
    if(ret<0)
    {
	printf("读取数据库资料失败\n");
	return;
    }

    ret = getconf("mysql_database",database);
    if(ret<0)
    {
	printf("读取数据库资料失败\n");
	return;
    }

    if(mysql_init(mysql)==NULL)
    {
	printf("数据库初始化失败\n");
    }
    else
    {
	if(mysql_real_connect(mysql,host,"root","123456",database,0,NULL,0)==NULL)
	{
	    printf("数据库连接失败\n");
	    return;
	}
	else
	{
	    printf("数据库已连接\n");
	}
    }
    return;
}


/*****************  数据库中的表操作  *************/
int mysql_insert(char *str,MYSQL *mysql)
{
    if(!mysql_query(mysql,str))
    {
	printf("写入成功\n");
	return 1;
    }
    printf("失败\n");
    return 0;
}


/***************  登录验证  ***************/
int mysql_log(char *str,MYSQL *mysql)
{
    if(!mysql_query(mysql,str))
    {
	MYSQL_RES *result = NULL;
	result = mysql_store_result(mysql);
	if(result==NULL)
	    printf("没有数据\n");
	else
	{
	    int rows,field;
	    rows = mysql_num_rows(result); //行数
	    field = mysql_num_fields(result); // 列数
	    if(rows<1)
	    {
		printf("帐号或密码错误\n");
		return 0;
	    }
	    return -1;
	}

    }
    else
	printf("mysql fail...\n");

    return 0;
}

/*** 从数据库读取医生信息  
 *** 先发包头说明读取是否成功
 *** 若成功,把数据库中对应信息一条一条发给对应服务器
 */

int mysql_select_doctor(MYSQL *mysql,int clientfd,package_t *cpack,char *str)
{
    int ret;
    package_t pack= *cpack;

    if(!mysql_query(mysql,str))
    {
	MYSQL_RES *result = NULL;
	result = mysql_store_result(mysql);
	if(result==NULL)
	    printf("没有数据\n");
	else
	{
	    int rows,field;
	    rows = mysql_num_rows(result);
	    field = mysql_num_fields(result);
	    if(rows<1)
	    {
		printf("没有医生信息\n");
		pack.dataopt = NOINFO;
		pack.length = 0;
		sendpackage_head(clientfd,pack,NULL);
		return 0;
	    }
	    MYSQL_ROW row = NULL;
	    doc DO;
	    pack.dataopt = SHOW_OK;
	    pack.length = 0;

	    ret = sendpackage_head(clientfd,pack,NULL);
	    for(int i =0;i<rows;i++)
	    {
		row = mysql_fetch_row(result);

		DO.id = atoi(row[0]);
		strcpy(DO.log,row[1]);
		strcpy(DO.name,row[2]);
		strcpy(DO.password,row[3]);
		strcpy(DO.sex,row[4]);
		DO.age = atoi(row[5]);
		strcpy(DO.keshi,row[6]);

		ret = write(clientfd,&DO,sizeof(doc));
		if(ret<0)
		{
		    printf("发送数据错误\n");
		}
	    }

	    DO.id = -1;
	    ret = write(clientfd,&DO,sizeof(doc));
	    if(ret<0)
		printf("发送数据错误\n");
	    
	}
    }

    return 1;
}

/****************  从数据库读取护士信息  *******************/
int mysql_select_nurse(MYSQL *mysql,int clientfd,package_t *cpack,char *str)
{
    int ret;
    package_t pack = *cpack;

    if(!mysql_query(mysql,str))
    {
	MYSQL_RES *result=NULL;
	result = mysql_store_result(mysql);
	if(result==NULL)
	    printf("没有数据结果\n");
	else
	{
	    int rows,field;
	    rows = mysql_num_rows(result);
	    field = mysql_num_fields(result);
	    if(rows<1)
	    {
		printf("没有护士信息\n");
		pack.dataopt = NOINFO;
		pack.length = 0;
		sendpackage_head(clientfd,pack,NULL);
		return 0;
	    }
	    MYSQL_ROW row = NULL;
	    nur NU;
	    pack.dataopt = SHOW_OK;
	    pack.length = 0;

	    ret = sendpackage_head(clientfd,pack,NULL);
	    for(int i=0;i<rows;i++)
	    {
		row = mysql_fetch_row(result);
		NU.id = atoi(row[0]);
		strcpy(NU.log,row[1]);
		strcpy(NU.name,row[2]);
		strcpy(NU.password,row[3]);
		strcpy(NU.sex,row[4]);
		NU.age = atoi(row[5]);

		ret = write(clientfd,&NU,sizeof(nur));
		if(ret<0)
		    printf("发送数据失败\n");
	    }

	    NU.id = -1;
	    ret = write(clientfd,&NU,sizeof(nur));
	    if(ret<0)
		printf("发送数据失败\n");
    

	}
    }

    return 1;
}
/****************  从数据库读取患者信息  **********************/
int mysql_select_patient(MYSQL *mysql,int clientfd,package_t *cpack,char *str)
{
    int ret;
    package_t pack= *cpack;

    if(!mysql_query(mysql,str))
    {
	MYSQL_RES *result = NULL;
	result = mysql_store_result(mysql);
	if(result==NULL)
	    printf("没有数据\n");
	else
	{
	    int rows,field;
	    rows = mysql_num_rows(result);
	    field = mysql_num_fields(result);
	    if(rows<1)
	    {
		printf("没有病人信息\n");
		pack.dataopt = NOINFO;
		pack.length = 0;
		sendpackage_head(clientfd,pack,NULL);
		return 0;
	    }
	    MYSQL_ROW row = NULL;
	    pat PA;
	    pack.dataopt = SHOW_OK;
	    pack.length = 0;
	
	    ret = sendpackage_head(clientfd,pack,NULL);
		printf("%d\n",pack.dataopt);
	    for(int i =0;i<rows;i++)
	    {
		row = mysql_fetch_row(result);

		PA.id = atoi(row[0]);
		strcpy(PA.name,row[1]);
		strcpy(PA.sex,row[2]);
		PA.age = atoi(row[3]);
		strcpy(PA.condition,row[4]);
		strcpy(PA.advice,row[5]);
		PA.money = atof(row[6]);
		PA.flag = atoi(row[7]);
		ret = write(clientfd,&PA,sizeof(pat));
		if(ret<0)
		{
		    printf("发送数据错误\n");
		}
	    }

	    PA.id = -1;
	    ret = write(clientfd,&PA,sizeof(pat));
	    if(ret<0)
		printf("发送数据错误\n");
    
	}
    }


    return 1;
}



/****************  从数据库读取药品信息  ****************/
int mysql_select_medicine(MYSQL *mysql,int clientfd,package_t *cpack,char *str)
{
    int ret;
    package_t pack = *cpack;

    if(!mysql_query(mysql,str))
    {
	MYSQL_RES *result = NULL;
	result = mysql_store_result(mysql);
	if(result==NULL)
	    printf("没有数据\n");
	else
	{
	    int rows,field;
	    rows=mysql_num_rows(result);
	    field=mysql_num_fields(result);
	    if(rows<1)
	    {
		printf("没有药品信息\n");
		pack.dataopt = NOINFO;
		pack.length = 0;
		sendpackage_head(clientfd,pack,NULL);
		return 0;
	    }
	    MYSQL_ROW row = NULL;
	    med ME;
	    pack.dataopt = SHOW_OK;
	    pack.length = 0;

	    ret = sendpackage_head(clientfd,pack,NULL);
	    for(int i=0;i<rows;i++)
	    {
		row = mysql_fetch_row(result);

		ME.id = atoi(row[0]);
		strcpy(ME.name,row[1]);
		ME.price = atof(row[2]);
		strcpy(ME.type,row[3]);
		ME.discount = atof(row[4]);

		ret = write(clientfd,&ME,sizeof(med));
		if(ret<0)
		    printf("发送数据失败\n");
	    }
	    ME.id = -1;
	    ret= write(clientfd,&ME,sizeof(med));
	    if(ret<0)
		printf("发送数据失败\n");
	
	}
    }

    return 1;
}


/********************  从数据库读取化验项目信息  **********************/
int mysql_select_assay(MYSQL *mysql,int clientfd,package_t *cpack,char *str)
{
    int ret;
    package_t pack = *cpack;

    if(!mysql_query(mysql,str))
    {
	MYSQL_RES *result = NULL;
	result = mysql_store_result(mysql);
	if(result==NULL)
	    printf("没有数据\n");
	else
	{
	    int rows,field;
	    rows=mysql_num_rows(result);
	    field=mysql_num_fields(result);
	    if(rows<1)
	    {
		printf("没有药品信息\n");
		pack.dataopt = NOINFO;
		pack.length = 0;
		sendpackage_head(clientfd,pack,NULL);
		return 0;
	    }
	    MYSQL_ROW row = NULL;
	    assay AS;
	    pack.dataopt = SHOW_OK;
	    pack.length = 0;

	    ret = sendpackage_head(clientfd,pack,NULL);
	    for(int i=0;i<rows;i++)
	    {
		row = mysql_fetch_row(result);

		AS.id = atoi(row[0]);
		strcpy(AS.name,row[1]);
		AS.price = atof(row[2]);

		ret = write(clientfd,&AS,sizeof(assay));
		if(ret<0)
		    printf("发送数据失败\n");
	    }
	    AS.id = -1;
	    ret= write(clientfd,&AS,sizeof(assay));
	    if(ret<0)
		printf("发送数据失败\n");
	}
    }

    return 1;
}


/******************* 从数据库读取医嘱信息  ********************/
int mysql_select_table(MYSQL *mysql,int clientfd,package_t *cpack,char *str)
{
    int ret;
    package_t pack = *cpack;

    if(!mysql_query(mysql,str))
    {
	MYSQL_RES *result = NULL;
	result = mysql_store_result(mysql);
	if(result==NULL)
	    printf("没有数据\n");
	else
	{
	    int rows,field;
	    rows=mysql_num_rows(result);
	    field=mysql_num_fields(result);
	    if(rows<1)
	    {
		printf("没有药品信息\n");
		pack.dataopt = NOINFO;
		pack.length = 0;
		sendpackage_head(clientfd,pack,NULL);
		return 0;
	    }
	    MYSQL_ROW row = NULL;
	    cure_table CU;
	    pack.dataopt = SHOW_OK;
	    pack.length = 0;

	    ret = sendpackage_head(clientfd,pack,NULL);
	    for(int i=0;i<rows;i++)
	    {
		row = mysql_fetch_row(result);

		CU.pid = atoi(row[0]);
		CU.mid = atoi(row[1]);
		CU.num = atoi(row[2]);
		CU.price = atof(row[3]);
		CU.flag = atoi(row[4]);

		ret = write(clientfd,&CU,sizeof(cure_table));
		if(ret<0)
		    printf("发送数据失败\n");
	    }
	    CU.pid = -1;
	    ret= write(clientfd,&CU,sizeof(cure_table));
	    if(ret<0)
		printf("发送数据失败\n");
	}
    }

    return 1;
}



