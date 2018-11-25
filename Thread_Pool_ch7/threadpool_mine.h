#include <pthread.h>

struct work
{
	void* (*function)(void *arg);
	void *arg;
	struct work *next;
};

struct threadpool
{
	int thread_num;
	int queue_max_num;
	struct work *head;
	struct work *tail;
	pthread_t *pthreads;
	pthread_mutex_t mutex;
	pthread_cond_t queue_empty;
	pthread_cond_t queue_not_empty;
	pthread_cond_t queue_not_full;
	int queue_cur_num; //当前work数量
	int queue_close; //queue closed or not
	int pool_close; //pool closed or not
};

struct threadpool* threadpool_init(int thread_num, int queue_max_num);

int threadpool_add_work(struct threadpool* pool, void* (*function)(void *arg), void *arg);

int threadpool_destroy(struct threadpool *pool);

void* threadpool_function(void *arg);
