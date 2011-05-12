#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BYTE unsigned char

int main()
{
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	BYTE rd_buf[BUFSIZ] = {0};
	int rd_len = 0;
	char svr_ip[32] = "192.168.41.89";
	int svr_port = 17171;
	struct sockaddr_in svr_addr;
	bzero(&svr_addr, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	if ( -1 == inet_pton(AF_INET, svr_ip, &svr_addr.sin_addr) )
	{
		perror("inet_pton error:");
		return -1;
	}
	svr_addr.sin_port = htons(svr_port);
	while(1)
	{
		bzero(rd_buf, BUFSIZ);
		rd_len = read(STDIN_FILENO, rd_buf, BUFSIZ);
		if (rd_len >= 0)
		{
			printf("%s\n", rd_buf);
			sendto(sock_fd, rd_buf, rd_len, 0, (struct sockaddr*)&svr_addr, sizeof(svr_addr));
		}
	}
	
	
}

