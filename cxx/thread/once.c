#include <stdio.h>
#include <pthread.h>

pthread_once_t once = PTHREAD_ONCE_INIT;

void once_run()
{
	printf("once_run in thread %d\n", pthread_self());
}

void* child1(void* arg)
{
	int tid = pthread_self();
	printf("thread %d enter\n", tid);
	pthread_once(&once, once_run);
	printf("thread %d returns\n", tid);
}

void* child2()
{
	int tid = pthread_self();
	printf("thread %d enter\n", tid);
	pthread_once(&once, once_run);
	printf("thread %d returns\n", tid);
}

int main()
{
	int tid1, tid2;
	printf("hello\n");
	pthread_create(&tid1, NULL, child1, NULL);
	pthread_create(&tid2, NULL, child2, NULL);
//	sleep(10);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("main thread exit\n");
	return 0;
}
