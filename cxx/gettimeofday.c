#include <unistd.h>
#include <sys/time.h>

int main()
{
	struct timeval tv;
	bzero(&tv, sizeof(tv));
	gettimeofday(&tv, 0);
	printf("%llu\n", (unsigned long long)tv.tv_sec*1000 + tv.tv_usec/1000);
	return 0;

}
