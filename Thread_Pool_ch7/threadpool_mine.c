#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <assert.h>

#include "threadpool_mine.h"

struct threadpool* threadpool_init(int thread_num, int queue_max_num)
{
    struct threadpool *pool = NULL;
	pool = malloc(sizeof(struct threadpool));
	if (pool == NULL)
	{
	    printf("threadpool malloc error\n");
	    return NULL;
	}
	pool->thread_num = thread_num;
	pool->queue_max_num = queue_max_num;
	pool->queue_cur_num = 0;
	pool->head = NULL;
	pool->tail = NULL;
	if (pthread_mutex_init(&(pool->mutex), NULL))
	{
	    printf("mutex init NULL\n");
	    return NULL;
	}
	if (pthread_cond_init(&(pool->queue_empty), NULL))
	{
	    printf("queue_empty init error\n");
	    return NULL;
	}
	if (pthread_cond_init(&(pool->queue_not_empty), NULL))
	{
	    printf("queue_not_empty init error\n");
	    return NULL;
	}
	if (pthread_cond_init(&(pool->queue_not_full), NULL))
	{
	    printf("queue_not_full init error\n");
	    return NULL;
	}
	pool->pthreads = malloc(sizeof(pthread_t) * thread_num);
	if (pool->pthreads == NULL)
	{
	    printf("pthreads malloc error\n");
	    return NULL;
	}
	pool->queue_close = 0;
	pool->pool_close = 0;

	for (int i = 0; i < pool->thread_num; ++i)
	    pthread_create(&(pool->pthreads[i]), NULL, threadpool_function, (void *)pool);

	return pool;
}

int threadpool_add_job(struct threadpool* pool, void* (*function)(void *arg), void *arg)
{
    assert(pool != NULL);
    assert(function != NULL);
    assert(arg != NULL);

    pthread_mutex_lock(&(pool->mutex));
    while ((pool->queue_cur_num == pool->queue_max_num) && !(pool->queue_close || pool->pool_close))
        pthread_cond_wait(&(pool->queue_not_full), &(pool->mutex));   //任务队满，等
    if (pool->queue_close || pool->pool_close)
    {
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    }
    struct work *job =(struct work*) malloc(sizeof(struct work));
    if (job == NULL)
    {
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    } 
    job->function = function;    
    job->arg = arg;
    job->next = NULL;
    if (pool->head == NULL)//空队特殊
    {
        pool->head = pool->tail = job;
        pthread_cond_broadcast(&(pool->queue_not_empty));  //广播队列非空
    }
    else
    {
        pool->tail->next = job;
        pool->tail = job;    
    }
    pool->queue_cur_num++;
    pthread_mutex_unlock(&(pool->mutex));
    return 0;
}

void* threadpool_function(void* arg)
{
	struct threadpool *pool = (struct threadpool*)arg;
	struct work *job = NULL;
	while(1)
	{
		pthread_mutex_lock(&(pool->mutex));
		while ((pool->queue_cur_num == 0) && !pool->pool_close)//任务队列空，等
			pthread_cond_wait(&(pool->queue_not_empty), &(pool->mutex));
		if (pool->pool_close)
		{
			pthread_mutex_unlock(&(pool->mutex));
			pthread_exit(NULL);
		}
		pool->queue_cur_num--;
		job = pool->head;
		if (pool->queue_cur_num == 0)//最后一个任务
			pool->head = pool->tail = NULL;
		else
			pool->head = job->next;
		if (pool->queue_cur_num == 0)
			pthread_cond_broadcast(&(pool->queue_empty));//通知destroy
		if (pool->queue_cur_num <= pool->queue_max_num - 1)
			pthread_cond_broadcast(&(pool->queue_not_full));//通知任务队列不满，可以加
		pthread_mutex_unlock(&(pool->mutex));
		(*(job->function))(job->arg);
		free(job);
		job = NULL;
	}
}

int threadpool_destroy(struct threadpool *pool)
{
	assert(pool != NULL);//truth assertion
	pthread_mutex_lock(&(pool->mutex));
	if (pool->queue_close || pool->pool_close)
	{
		pthread_mutex_unlock(&(pool->mutex));
		return -1;
	}
	pool->queue_close = 1;
	while (pool->queue_cur_num != 0)
		pthread_cond_wait(&(pool->queue_empty), &(pool->mutex));
	pool->pool_close = 1;
	pthread_mutex_unlock(&(pool->mutex));
	pthread_cond_broadcast(&(pool->queue_not_empty));
	pthread_cond_broadcast(&(pool->queue_not_full)); 

    for (int i = 0; i < pool->thread_num; ++i)
        pthread_join(pool->pthreads[i], NULL);    //等待线程池的所有线程执行完毕
       
    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->queue_empty));
    pthread_cond_destroy(&(pool->queue_not_empty));   
    pthread_cond_destroy(&(pool->queue_not_full));    
    free(pool->pthreads);
    struct work *p;
    while (pool->head != NULL)
    {
    	p = pool->head;
    	pool->head = p->next;
    	free(p);
    }
    free(pool);
    return 0;
   }
