#include <stdio.h>
#include <arpa/inet.h>

int main()
{
	printf( "%u\n", htonl(inet_addr("218.240.145.0")) );
	printf( "%u\n", htonl(inet_addr("218.240.145.1")) );
	printf( "%u\n", htonl(inet_addr("218.240.145.255")) );
	return 0;
}
