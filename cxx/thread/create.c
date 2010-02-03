#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *pp(void *arg)
{
	printf("child %ld running\n", pthread_self());
	printf("child %d running\n", pthread_self());
	int i = 20;
	while(i)
	{
		printf("%s\n", (char*)arg);
		sleep(2);
		i--;
	}
	return NULL;
}

int main()
{
	pthread_t pid;
	pthread_create(&pid, NULL, pp, "hello world");

	int i = 20;
	while(i)
	{
		printf("I am main thread\n");
		sleep(1);
		i--;
	}
	return 0;
}
