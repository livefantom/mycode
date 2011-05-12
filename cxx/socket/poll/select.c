#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define MAX_LINE 1024

int tcp_listener(int port, int backlog);

typedef struct clt_conn
{
	int sockfd;
	char sndbuf[MAX_LINE];
	char rcvbuf[MAX_LINE];
	int sndlen;
	int rcvlen;
	time_t sndtime;
} SCltConn;


int main()
{
	SCltConn clts[FD_SETSIZE];
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

	memset(clts, 0, FD_SETSIZE*sizeof(SCltConn));
	if (-1 == (lsnfd = tcp_listener(12001, SOMAXCONN)) )
	{
		return -1;
	}
	maxfd = lsnfd;
	while (1)
	{
		usleep(10000);
		int i;
		FD_ZERO(&rfds);
		// add all descriptions to readfds.
		FD_SET(lsnfd, &rfds);
		for (i = 0; i< FD_SETSIZE; i++)
		{
			if (clts[i].sockfd > 0)
			{
				FD_SET(clts->sockfd, &rfds);
				FD_SET(clts->sockfd, &wfds);
			}
		}
		
		// int select(int maxfdp1,
		// fd_set* readset, fd_set* writeset, fd_set* exceptset,
		// const struct timeval* timeout);
		if (-1 == (nready = select(maxfd+1, &rfds, NULL, NULL, &tv)) )
		{
			perror("select error!");
			return -1;
		}
		if (nready)
		{
			if ( FD_ISSET(lsnfd, &rfds) )
			{
				printf("someone connected!");
				// int accept(int s, struct sockaddr* addr, socklen_t* addrlen);
				cltlen = sizeof(cltaddr);
				if ( -1 == (connfd = accept(lsnfd, (struct sockaddr*) &cltaddr, &cltlen)) )
				{
					perror("accept error!");
				}
				else
				{
					for (i = 0; i< FD_SETSIZE; i++)
					{
						if (clts[i].sockfd <= 0)
						{
							clts[i].sockfd = connfd;
							break;
						}
					}
					// too many connections!!!
					if (i >= FD_SETSIZE)
					{
						printf("too many connections!!!");
						close(connfd);
					}

					if (connfd > maxfd)
					{
						maxfd = connfd;
					}
					if (--nready <= 0)
					{
						continue;
					}
				}

			}
			for (i = 0; i< FD_SETSIZE; i++)
			{
				if ( FD_ISSET(clts[i].sockfd, &rfds) )
				{
					if (-1 == reader(clts+i) )
					{
						shutdown(clts[i].sockfd, SHUT_RDWR);
						close(clts[i].sockfd);
						clts[i].sockfd = -1;
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

int reader(SCltConn* clt)
{
	int rcvlen;
	while (1)
	{
		rcvlen = read(clt->sockfd, clt->rcvbuf + clt->rcvlen, MAX_LINE);
		if (rcvlen > 0)
		{
			clt->rcvlen += rcvlen;
		}
		else if (rcvlen < 0 && errno == EINTR)
		{
			continue;
		}
		else if (rcvlen < 0)
		{
			perror("receive error!");
			return -1;
		}
		// received complete!!
		break;
	}

	//process msg!
	printf("%s\n", clt.rcvbuf);
	srand(clt.sockfd);
	clt.sndtime = time(0) + rand()%20;
	sprintf(clt.sndbuf, "result=%d\n", rand()%2);

	return 0;
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
		perror("bind error!");
		return -1;
	}
	// int listen(int s, int backlog);
	if( -1 == listen(lsnfd, backlog) )
	{
		perror("listen error!");
		return -1;
	}
	return lsnfd;
}


