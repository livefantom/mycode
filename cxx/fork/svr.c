#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	int num = 0;
	//print all args.
/*
	for( num = 0; num < argc; ++num )
	{
		printf("%s\n", argv[num]);
	}
*/
	num = 0;
	if (argc > 1)
		num = atoi(argv[1]);
	while(num > 0)
	{
		printf("%ld >> sleep %dth second...\n", getpid(), num);
		sleep(1);
		--num;
	}
	return 0;
}
