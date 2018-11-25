#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;

int in = 0, out = 0;
int count = 0;

int insert_item(buffer_item item)
{
	while (count == BUFFER_SIZE);
	pthread_mutex_lock(&mutex);
	buffer[in] = item;
	in = (in + 1) % BUFFER_SIZE;
	count++;
	pthread_mutex_unlock(&mutex);
	return 0;
}

int remove_item(buffer_item *item)
{
	while (count == 0);
	pthread_mutex_lock(&mutex);
	*item = buffer[out];
	out = (out + 1) % BUFFER_SIZE;
	count--;
	pthread_mutex_unlock(&mutex);
	return 0;
}

void *producer(void *param)
{
	buffer_item item;
	
	while (1)
	{
		sleep(1);
		item = rand() % 10000;
		if (insert_item(item))
			fprintf(stderr, "report error condition");
		else
			printf("producer produced %d\n", item);
	}
}

void *consumer(void *param)
{
	buffer_item item;
	
	while(1)
	{
		sleep(1);
		if (remove_item(&item))
			fprintf(stderr, "report error condition");
		else
			printf("consumer consumed %d\n", item);
	}
}

int main(int argc, char *argv[])
{
	int sleepTime = atoi(argv[1]);
	int producerNum = atoi(argv[2]);
	int consumerNum = atoi(argv[3]);
	printf("%d %d %d\n", sleepTime, producerNum, consumerNum);
	pthread_t *pthreads = malloc(sizeof(pthread_t) * (producerNum + consumerNum));
	pthread_mutex_init(&mutex, NULL);
	//buffer = malloc(sizeof(buffer_item) * BUFFER_SIZE);
	for (int i = 0; i < producerNum; ++i)
		pthread_create(&pthreads[i], NULL, producer);
	for (int i = producerNum; i < consumerNum + producerNum; ++i)
		pthread_create(&pthreads[i], NULL, consumer);
	for (int i = 0; i < consumerNum + producerNum; ++i)
		pthread_join(pthreads[i], NULL);
	sleep(sleepTime);
	return 0;
}
