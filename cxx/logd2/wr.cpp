#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#define BYTE unsigned char
#include "logd.h"

int main()
{
	int rd_len = 0;
	char svr_ip[32] = "192.168.41.89";
	int svr_port = 17171;
	EnterMsg emsg;
	int log_id = 1000000L;
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
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
		emsg.log_id  = log_id++;
		emsg.user_id = 1000011;
		emsg.role_id = 100001;
		emsg.role_level = 100;
		emsg.gateway_id = 1002;
		emsg.server_id = 100201;
		emsg.login_time = time(0);
		emsg.user_ip = 123123123;
		emsg.user_port = 55555;

		printf("%s\n", &emsg);
		sendto(sock_fd, &emsg, sizeof(emsg), 0, (struct sockaddr*)&svr_addr, sizeof(svr_addr));

		usleep(20000);
	}
	
}

