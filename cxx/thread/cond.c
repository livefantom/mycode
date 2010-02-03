#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
void *pp(void *arg)
{
	int idx = *(int*)arg;
	printf("thread %lu starting...\n", pthread_self());
	printf("thread [%d] wating...\n", idx);
//	pthread_cond_wait(&cond, &mtx);
	int i = 20;
	while(i)
	{
		printf("I am thread [%d]!\n", idx);
		sleep(1);

		i--;
	}
	return NULL;
}
// result: pthread_cond_signal can only wake one???
int main()
{
	pthread_t pid[3];
	int idx = 0;
	++idx;
	pthread_create(pid+idx, NULL, pp, &idx);
	sleep(1);
	++idx;
	pthread_create(pid+idx, NULL, pp, &idx);

	int i = 0;
	do
	{
		printf("I am thread [0]!\n");
		sleep(1);
		if (( i%3) == 0)
		{
			printf("wake one thread !\n", pthread_self());
			pthread_cond_signal(&cond);
		}

		i++;
	}while(i<30);
	return 0;
}
