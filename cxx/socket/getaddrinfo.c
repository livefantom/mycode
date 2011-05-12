#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char** argv)
{
	//	getaddrinfo(const char* restrict nodename, const char* restrict servname,
	// 			const struct addrinfo* restrict hints, struct addrinfo** restrict res);

	//struct addrinfo* res; // !!!this uninitialize pointer would cause crash when freeaddrinfo()!
	struct addrinfo* res = 0;
	struct addrinfo hints;
	uint32_t ip_addr;
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if ( 0 == getaddrinfo(argv[1], 0, &hints, &res))
	{
		ip_addr = ntohl( ( (struct sockaddr_in*)res->ai_addr )->sin_addr.s_addr );
		printf("%u", ip_addr);
	}
	freeaddrinfo(res);
	return 0;

}
