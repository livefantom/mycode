#include <stdio.h>
void* func(int a)
{
	printf("a=%d\n",a);
}

int main()
{
	(*func)(100);
	func(100);
	return 0;
}
