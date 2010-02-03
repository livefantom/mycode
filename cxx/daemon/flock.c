#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int lf = open("LOCKF", O_RDWR | O_CREAT, 0640);
	printf("%d open\n", lf);
	if ( -1 == lf )
	{
		perror("open");
		exit(1);
	}
	if ( flock(lf, LOCK_EX | LOCK_NB) )
	{
		perror("flock");
		exit(0);
	}
	printf("%d running!\n", getpid());

	while(1)
	{
		sleep(1);
	}
}
