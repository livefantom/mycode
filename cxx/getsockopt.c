#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
	//int getsockopt(int socket, int level, int option_name,
		// void *restrict option_value, socklen_t *restrict option_len);
	int sock_fd = socket(SOCK_STREAM, AF_INET, 0);
	int buf_len =0;
	socklen_t len = sizeof(buf_len);
	getsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, (void*)&buf_len, &len);
	printf("socket receive buffer len is %d\n", buf_len);

	getsockopt(sock_fd, IPPROTO_TCP, SO_RCVBUF, (void*)&buf_len, &len);
	printf("tcp receive buffer len is %d\n", buf_len);
}
