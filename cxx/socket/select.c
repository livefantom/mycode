#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define MAX_LINE	8
#define MAX_BUFF	(8*MAX_LINE)

int tcp_listener(int port, int backlog);

typedef struct clt_conn
{
	int sockfd;
	char sndbuf[MAX_BUFF];
	char rcvbuf[MAX_BUFF];
	int sndlen;
	int rcvlen;
	time_t sndtime;
} SCltConn;


int main()
{
	SCltConn clts[FD_SETSIZE];	// 客户端连接，最大1024个
	int lsnfd, connfd;
	struct sockaddr_in cltaddr;
	socklen_t cltlen;
	fd_set rfds, wfds;
	int maxfd;
	struct timeval tv;
	int nready;
	int rcvlen;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	// 初始化客户端连接
	memset(clts, 0, FD_SETSIZE*sizeof(SCltConn));
	// 启动监听
	if (-1 == (lsnfd = tcp_listener(12001, SOMAXCONN)) )	// 最大队列128
	{
		return -1;
	}
	maxfd = lsnfd;
	while (1)
	{
		usleep(10000);	// 10毫秒
		int i;
		FD_ZERO(&rfds);
		// add all descriptions to readfds.
		FD_SET(lsnfd, &rfds);
		for (i = 0; i< FD_SETSIZE; ++i)	// 循环添加套接字到fd_set中
		{
			if (clts[i].sockfd > 0)
			{
				FD_SET(clts[i].sockfd, &rfds);
				FD_SET(clts[i].sockfd, &wfds);
			}
		}

		// int select(int maxfdp1,
		// fd_set* readset, fd_set* writeset, fd_set* exceptset,
		// const struct timeval* timeout);
		if (-1 == (nready = select(maxfd+1, &rfds, NULL, NULL, &tv)) )
		{
			printf("select error: %d: %s\n", errno, strerror(errno) );
			return -1;
		}
		if (nready > 0)	// “就绪”的套接字个数
		{
			printf(">>>>>poll: %d<<<<<\n", nready);
			if ( FD_ISSET(lsnfd, &rfds) )	// “监听”套接字是否就绪？
			{
				printf("someone connected!\n");
				// int accept(int s, struct sockaddr* addr, socklen_t* addrlen);
				cltlen = sizeof(cltaddr);
				if ( -1 == (connfd = accept(lsnfd, (struct sockaddr*) &cltaddr, &cltlen)) )
				{
					printf("accept error: %d: %s\n", errno, strerror(errno) );
				}
				else
				{
					// 寻找一个空位，将新的客户端套接字保存起来
					for (i = 0; i< FD_SETSIZE; ++i)
					{
						if (clts[i].sockfd <= 0)
						{
							clts[i].sockfd = connfd;
							printf("new connect accepted!\n");
							break;
						}
					}
					// too many connections!!!
					if (i >= FD_SETSIZE)
					{
						printf("too many connections!\n");
						close(connfd);
					}

					if (connfd > maxfd)	// 更新maxfd.
					{
						maxfd = connfd;
					}
					if (--nready <= 0)
					{
						continue;	// 下一次poll.
					}
				}

			}
			// else 遍历其他套接字
			for (i = 0; i< FD_SETSIZE; ++i)
			{
				if ( FD_ISSET(clts[i].sockfd, &rfds) )
				{
					int res = 0;
					
					if ( (res = reader(clts+i)) > 0 )	// 构造发送内容
					{
						clts[i].sndtime = time(0) + rand()%20;
						sprintf(clts[i].sndbuf, "result=%d\n", rand()%2);
					}
					printf("[%d]: total content = %s\n", clts[i].sockfd, clts[i].rcvbuf);

					// 拷贝接收缓冲数据到存储区
					bzero(clts[i].rcvbuf, MAX_BUFF);
					clts[i].rcvlen = 0;

					if ( res <= 0)
					{
						shutdown(clts[i].sockfd, SHUT_RDWR);
						close(clts[i].sockfd);
						bzero( clts+i, sizeof(SCltConn) );	// 清空客户端连接结构体，供下次使用
					}
				}
				// next select!
				if (--nready <= 0)
				{
					continue;
				}
			}
		}//end if 
	}//end of while(1)

}

// 阻塞式的读取器
int reader(SCltConn* clt)
{
	int rcvlen;
	while (1)
	{
		if (clt->rcvlen + MAX_LINE >= MAX_BUFF)
		{
			printf("no enough memory!\n");
			return 2;	// 等待外部清空读取缓冲
		}
		rcvlen = read(clt->sockfd, clt->rcvbuf + clt->rcvlen, MAX_LINE);
		printf("[%d]: rcvlen = %d\n", clt->sockfd, rcvlen);
		if (rcvlen > 0)
		{
			clt->rcvlen += rcvlen;
			printf("tatal received: %d\n", clt->rcvlen);
			if (rcvlen < MAX_LINE)
			{
				break;	// 本轮接收完成
			}
			// else 继续收，#### 将阻塞在这里 ####
		}
		else if (rcvlen < 0 && errno == EINTR)	// 信号中断等
		{
			continue;
		}
		else if (rcvlen < 0)
		{
			printf("receive error: %d: %s\n", errno, strerror(errno) );
			return -1;
		}
		else if (0 == rcvlen)	// 阻塞状态，为远端关闭
		{
			printf("remote closed!\n");
			return 0;
		}
	}

	return 1;
}

// 非阻塞读取器
int asyn_reader(SCltConn* clt)
{

}

int tcp_listener(int port, int backlog)
{ 
	int lsnfd;
	struct sockaddr_in svraddr;

	// int socket(int domain, int type, int protocol);
	lsnfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&svraddr, sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svraddr.sin_port = htons(port);
	// int bind(int sockfd, struct sockaddr* my_addr, socklen_t addrlen);
	if ( -1 == bind(lsnfd, (struct sockaddr*) &svraddr, sizeof(svraddr)) )
	{
		printf("bind error: %d: %s\n", errno, strerror(errno) );
		return -1;
	}
	// int listen(int s, int backlog);
	if( -1 == listen(lsnfd, backlog) )
	{
		printf("listen error: %d: %s\n", errno, strerror(errno) );
		return -1;
	}
	return lsnfd;
}


