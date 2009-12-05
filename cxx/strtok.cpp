#include "thread.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>

class WorkThread : public Thread
{
protected:
	virtual void run()
	{
		char buffer[256] = "you are bad girls.";
		char* cp = strtok(buffer, " ");
		while(cp != 0)
		{
			printf("%d > %s\n", pthread_self(), cp);
			sleep(1);
			cp = strtok(0, " ");
		}
	}
};

int main()
{
	char buffer[256] = "i am a good boy!";
	char* p = 0;
	WorkThread* pth = new WorkThread();
	char* cp = strtok_r(buffer, " ", &p);
	pth->start();
	//char* cp = strtok(buffer, " ");
	while(cp != 0)
	{
		printf("%d > %s\n", pthread_self(), cp);
		sleep(2);
		cp = strtok_r(0, " ", &p);
	}
	pth->wait();
	delete pth;
	return 0;
}
