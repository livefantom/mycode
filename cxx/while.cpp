#include <stdio.h>


int main(void)
{
	int i = 0;
	printf("1> ");
	do {

		if(i<10)
		{
			printf("%d\t", i++);
			continue;
		}

	} while (0);

	i = 0;
	printf("\n2> ");
	while(1)
	{

		if(i<10)
		{
			printf("%d\t", i++);
			continue;
		}
		break;

	}


}
