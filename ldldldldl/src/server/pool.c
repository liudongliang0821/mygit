#include"pool.h" 


/************消费者,共享资源 : 函数指针链表  *************/
static void *thread_routine(void *argv)
{
    pool_t *pool = (pool_t*)argv;
    WORKER *current = NULL;

    for(;;)
    {
	pthread_mutex_lock(&pool->mutex);

	while(pool->head == NULL &&pool->shutdown==0)
	{
	    pthread_cond_wait(&pool->cond,&pool->mutex);
	}

	if(pool->shutdown == 1)
	{
	    pthread_mutex_unlock(&pool->mutex);
	    pthread_exit(NULL);
	}

	assert(pool->head!=NULL);
	current = pool->head;
	pool->head = pool->head->next;
	pthread_mutex_unlock(&pool->mutex);

	(current->pfunc)(current->argv);  //通过函数指针调用函数
	free(current->argv);
	free(current);
    }
}


/****** 初始化--->pool_t  结构体里的参数,创建number个线程  ******/
int thread_pool_init(pool_t *pool,int number)
{
    if(pool==NULL)
	return -1;
    if(number<=0)
	return -1;

    pool->pth = malloc(sizeof(pthread_t)*number);
    if(pool->pth==NULL)
	return -1;

    pool->number = number;
    pool->shutdown = 0;
    pool->head= NULL;
    pthread_mutex_init(&pool->mutex,NULL);
    pthread_cond_init(&pool->cond,NULL);

    for(int i=0;i<pool->number;i++)
    {
	if(pthread_create((pool->pth)+i,NULL,thread_routine,pool)!=0)
	{
	    free(pool->pth);
	    free(pool);
	    pool=NULL;
	    return -1;
	}
    }

    return 0;
}

/**************  生产者  *****************/
int thread_pool_addworker(pool_t *pool,void*(*pfunc)(void*),void *argv,size_t size)
{
    WORKER *tail = NULL;
    WORKER *temp = malloc(sizeof(WORKER));
    if(temp==NULL)
	return -1;

    temp->argv = malloc(size);
    if(temp->argv==NULL)
    {
	free(temp);
	return -1;
    }

    temp->pfunc = pfunc;
    memcpy(temp->argv,argv,size);
    temp->next = NULL;

    pthread_mutex_lock(&pool->mutex);
    if(pool->head == NULL)
	pool->head = temp;
    else
    {
	tail = pool->head;
	while(tail->next!=NULL)
	    tail = tail->next;
	tail->next = temp;
    }

    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_signal(&pool->cond);
    return 0;
}


/*****************  销毁所有线程   ***************/
int thread_pool_destroy(pool_t *pool)
{
    WORKER *temp = NULL;
    if(pool == NULL)
	return -1;
    if(pool->shutdown==1)
	return -1;

    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->cond);
    for(int i=0;i<pool->number;i++)
    {
	pthread_join(*(pool->pth+i),NULL);
    }
    free(pool->pth);

    while(pool->head!=NULL)
    {
	temp = pool->head->next;
	free(pool->head->argv);
	free(pool->head);
	pool->head = temp;
    }

    free(pool);
    pool = NULL;

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    return 0;

}
