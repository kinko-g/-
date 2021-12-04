#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <pthread.h>

#define n 10
int buffer[n] = {0};//n个缓冲区组成的数组;
sem_t blank_n,product_n;

//生产者程序;
void* producer(void *arg) {
	int i = 0;
	while(1) {
		// 信号量 - 1
		// 如果信号量 < 0 则睡眠当前进程
		sem_wait(&blank_n);
		buffer[i] = rand() % 1000 + 1;
		printf("produce---------%d\n",buffer[i]);
		sem_post(&product_n);
		i = (i + 1) % n;
		sleep(rand() % 1);
	}
}

//消费者程序;
void* consumer(void *arg) {
	int i = 0;
	while(1) {
		sem_wait(&product_n);
		printf("consumer----------%d\n",buffer[i]);
		buffer[i] = 0;
		sem_post(&blank_n);
		i = (i + 1) % n;
		sleep(rand() % 3);
	}
}

int main() {
	pthread_t t1,t2;
	sem_init(&blank_n,0,n);
	sem_init(&product_n,0,0);

	pthread_create(&t1,0,producer,0);
	pthread_create(&t2,0,consumer,0);
	pthread_join(t1,0);
	pthread_join(t2,0);

	sem_destroy(&blank_n);
	sem_destroy(&product_n);
}