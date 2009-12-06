#include <stdio.h>
#include <fcntl.h>

#define _TEST

int main()
{
	int fd1, fd2;
	if ( -1 == (fd1 = open("ppp.txt", O_WRONLY | O_APPEND)) )
	{
		perror("open1 error:");
		return -1;
	}
	printf("fd1=%d\n", fd1);
#ifdef _TEST
	if ( -1 == (fd2 = open("ppp.txt", O_WRONLY | O_APPEND)) )
	{
		perror("open2 error:");
		close(fd1);
		return -1;
	}
	printf("fd2=%d\n", fd2);
#endif
	if (-1 == write(fd1, "11111", 6))
	{
		perror("write1 error:");
	}
#ifdef _TEST
	if (-1 == write(fd2, "22222", 6))
	{
		perror("write2 error:");
	}
#endif
	close(fd1);
#ifdef _TEST
	close(fd2);
#endif
	return 0;
	
}
