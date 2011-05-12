#include <unistd.h>

int main()
{

	int i = 10;
	int* p = &i;
	printf("main --- %d\n", i);
	if (0 == fork())
	{
		int m = 0;
		for (;m<10; m++)
		{
			(*p)++;
			printf("child -------------- %d\n", *p);
			sleep(1);
		}
		exit(0);
	}
	int n = 0;
	for (; n<10; n++)
	{
		(*p)--;
		printf("main --- %d\n", *p);
		sleep(1);
	}
	return 0;

}
