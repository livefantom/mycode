#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define MAX_LINE 1024*4
char sndbuf[MAX_LINE] = "POST /MoveRole HTTP/1.1\n"
"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-silverlight, * / *\n"
"Referer: http://192.168.1.229:7000/gm-svn/test/pp-pwd.php\n"
"Accept-Language: zh-cn\n"
"Content-Type: application/x-www-form-urlencoded\n"
"Accept-Encoding: gzip, deflate\n"
"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)\n"
"Host: 192.168.1.229:7000\n"
"Content-Length: 83\n"
"Connection: Keep-Alive\n"
"Cache-Control: no-cache\r\n\r\n"
"role_name=123&role_id=131&gateway_id=160001&server_id=16000301&taget_location=12345";

struct ipaddr
{
	char ip[INET_ADDRSTRLEN];	// NET_ADDRSTRLEN = 16
	unsigned short port;
	short timeout;
};

void* clt_func(void* arg)
{
	int	connfd;
	struct sockaddr_in	svraddr;
	struct ipaddr*	svrip = arg;
	char	rcvbuf[MAX_LINE] = {0};
	int	sendtimes = 10;
	int	i = 0;
	struct timeval rcv_timeout;
	rcv_timeout.tv_sec = svrip->timeout;
	rcv_timeout.tv_usec = 0;
	// int socket(int domain, int type, int protocol); 
	connfd = socket(AF_INET, SOCK_STREAM, 0);
	// int setsockopt(int socket, int level, int option_name,
	//	 const char* option_value, socklen_t option_len);
	if ( -1 == setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&rcv_timeout, sizeof(rcv_timeout)) )
	{
		perror("set timeout error");
		return 0;
	}
	svraddr.sin_family = AF_INET;
	// int inet_pton(int af, const char* src, void* dst);
	if ( inet_pton(AF_INET, svrip->ip, &svraddr.sin_addr) != 1 )
	{
		printf("ip convert error!\n");
		return 0;
	}
	// uint16_t htons(uint16_t hostshort);
	svraddr.sin_port = htons(svrip->port);
	// int connect(int sockfd, struct sockaddr* servaddr, socklen_t addrlen);
	if ( -1 == connect(connfd, (struct sockaddr*) &svraddr, sizeof(svraddr)) )
	{
		//printf("connect error:%d\n", errno);
		printf("pid:%ld> connect %s:%d", (long) getpid(), svrip->ip, svrip->port);
		perror("connect error");
		return 0;
	}
	printf("pid:%ld> connected to %s:%d\n", (long) getpid(), svrip->ip, svrip->port);
	for (; i < sendtimes; i++)
	{
//		memset(sndbuf, 0, MAX_LINE);
//		sprintf(sndbuf, "pid:%ld> %d\n", getpid(), i);
		// ssize write(int fd, const void* buf, size_t count);
		if ( -1 == write(connfd, sndbuf, strlen(sndbuf)) )
		{
			printf("send data error:%d\n", errno);
			break;
		}
//		memset(sndbuf, 0, MAX_LINE);
		// sszie read(int fd, void* buf, size_t count);
		if ( -1 == read(connfd, rcvbuf, MAX_LINE) )
		{
			perror("receive error");
			printf("receive data error:%d\n", errno);
			break;
		}
		else
		{
			printf("pid:%ld> clt <<<< %s\n", getpid(), rcvbuf);
		}
		usleep(10000);
	}
	// int shutdown(int s, int how);
	if ( -1 == shutdown(connfd, SHUT_RDWR) )
	{
		perror("shut down error");
	}
	if ( -1 == close(connfd) )
	{
		perror("close error");
	}
}

int main(int argc, char** argv)
{
	printf("INET_ADDRSTRLEN = %d\n", INET_ADDRSTRLEN);
	struct ipaddr	svrip;
	pid_t	childpid;
	int	svr_num = 10;
	int	clt_num = 3;
	int	i, j;
	int	status;
	sprintf(svrip.ip, "192.168.1.229");
//	svrip.port = 11001;

//	sprintf(svrip.ip, "192.168.41.19");
	svrip.port = 16001;
/*
	for (i=0; i<argc; i++)
	{
		printf("%s\n", argv[i]);
	}
	return 0;
*/
	if (argc < 4)
		return -1;
	sscanf(argv[1], "%d", &svr_num);
	sscanf(argv[2], "%d", &clt_num);
	sscanf(argv[3], "%d", &(svrip.timeout));
	printf("svr_num=%d, clt_num=%d, timeout=%d\n", svr_num, clt_num, svrip.timeout);
	// create (10 * svrnum) client.
	j = 0;
	for (; j < svr_num; j++)
	{
		i = 0;
		for (; i < clt_num; i++)
		{
			// pid_t fork(void)
			if ( 0 == (childpid = fork()) )
			{// child process
				(*clt_func)(&svrip);
				exit(0);
			}
			// pid_t waitpid(pid_t pid, int* status, int options);
			childpid = waitpid(0, &status, WNOHANG);
			if (childpid > 0)
				printf("pid:%ld> child exit, status:%d\n", childpid, status);
		}
		svrip.port ++;
	}
	// wait for all child exit.
	while(1)
	{
		// pid_t wait(int* status);
		childpid = wait(&status);
		if (childpid > 0)
		{
			printf("pid:%ld> child exit, status:%d\n", childpid, status);
		}
		else
		{
			//printf("wait error:%d", errno);
			perror("wait error");
			break;
		}
	}
}
