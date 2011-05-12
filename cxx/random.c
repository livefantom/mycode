#include <stdlib.h>
#include <stdio.h>

int main()
{
	srand(1);
	int i;
	for (i=1; i<10; i++)
	{
		printf("%d\t", rand());
	}
	return 0;
}
