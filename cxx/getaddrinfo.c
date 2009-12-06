#include <sys/socket.h>

int main()
{
	//	getaddrinfo(const char* restrict nodename, const char* restrict servname,
	// 			const struct addrinfo* restrict hints, struct addrinfo** restrict res);
	struct addrinfo* res;
	struct addrinfo hint;
	bzero(hint, sizeof(addrinfo));
	hint.ai_family = AF_INET;
	getaddrinfo(0, 0, 0, &res);
	printf("%s", res->ai_addr)

}
