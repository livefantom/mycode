#include <stdio.h>

char aaa[] = "abcdefghijklmnopqrstuvwxyz";

int func(char** p)
{
	*p = aaa + 10;
	printf("%s\n", *p);
}

int main()
{
	char* p = NULL;
	printf("%s\n", aaa);
	func(&p);
	printf("%s\n", p);
}
