#include <pthread.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_LINE	1024*4
#define _USE_FORK
//#define _USE_PTHREAD
char http_resp[MAX_LINE] = "HTTP/1.1 200 OK\n"
    "Date: Tue, 08 Jul 2008 13:04:53 GMT\n"
    "Server: Apache/2.2.4 (Unix)\n"
    "Content-Length: 1\n"
    "Keep-Alive: timeout=5, max=100\n"
    "Connection: Keep-Alive\n"
    "Content-Type: text/html; charset=GBK\n\n";

// sighandler_t
void sig_chld(int signo)
{
	pid_t	pid;
	int	status;
	while( (pid = waitpid(-1, &status, WNOHANG)) > 0 )
		printf("pid:%ld> sig_chld> child:%ld terminated, status:%d\n", (long)getpid(), (long) pid, status);
}

void* str_echo(void* arg)
{
	ssize_t		rcvlen;
	char		rcvbuf[MAX_LINE];
	int* 		sockfd = (int*)arg;
#ifdef _USE_FORK
	pid_t		pid = getpid();
#elif defined(_USE_PTHREAD)
	pthread_t	pid = pthread_self();
#endif
	srand(pid);
	printf("\t\\_ pid:%ld> child starting...\n", (long) pid);
	do
	{
		rcvlen = read(*sockfd, rcvbuf, MAX_LINE);
		if (rcvlen > 0)
		{
            printf("\t\\_ pid:%ld> received %d bytes\n", (long) pid, rcvlen);
			int sec = rand()%20;
			printf("\t\\_ pid:%ld> sleep %d seconds...\n", (long) pid, sec);
			sleep( sec );	// wait for client receive timeout!!
			//memcpy(sndbuf, rcvbuf, rcvlen);
			snprintf( sndbuf, MAX_LINE, "%s%d", http_resp, rand()%2 );
            write(*sockfd, sndbuf, strlen(sndbuf)); // TODO: error proc.
			//continue;
		}
		else if (rcvlen < 0 && errno == EINTR)
        {
			continue;
        }
		else if (rcvlen < 0)
        {
			printf("\t\\_ pid:%ld> read error!\n", (long) pid);
        }
        // else if rcvlen == 0, remote closed.
	} while(false);
	close(*sockfd);
}

void* svr_func(void* arg)
{
	int* port = (int*)arg;
	struct sockaddr_in	svraddr, cltaddr;
	socklen_t		cltlen;
	int 			listenfd, connfd;
#ifdef _USE_FORK
	pid_t			childpid;
	pid_t			mypid = getpid();
#elif defined(_USE_PTHREAD)
	pthread_t		childpid;
#endif
	int			status;
	// int socket(int family, int type, int protocol);
	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	bzero(&svraddr, sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svraddr.sin_port = htons(*port);
	// int bind(int sockfd, const struct sockaddr* myaddr, socklen_t addrlen);
	if ( -1 == bind( listenfd, (struct sockaddr*) &svraddr, sizeof(svraddr) ) )
	{
		printf("pid:%ld> ", (long)mypid);
		perror("bind error");
		return;
	}
	// int listen(int sockfd, int backlog);
	if ( -1 == listen(listenfd, SOMAXCONN) )
	{
		printf("pid:%ld> ", (long)mypid);
		perror("listen error");
		return;
	}
	printf("pid:%ld> listen at port:%d, backlog:%d\n", (long)mypid, *port, SOMAXCONN);
#ifdef _USE_FORK
	// must call waitpid() to release zombie!
	// sighandler_t signal(int signum, sighandler_t handler);
	signal(SIGCHLD, sig_chld);
#endif
	while(1)
	{
		cltlen = sizeof(cltaddr);
		// int accept(int sockfd, sturct sockaddr* cliaddr, socklen_t* addrlen);
		if ( -1 == (connfd = accept(listenfd, (struct sockaddr*) &cltaddr, &cltlen)) )
		{
			if (EINTR == errno)	// this may occur when sighandler return!
				continue;
			else
				printf("pid:%ld> accept error:%d\n", getpid(), errno);
		}
		// ### if no new connections, this accept() will block!!!
#ifdef _USE_FORK
		// pid_t fork(void)
		if ( 0 == (childpid = fork()) )
		{// child process
			// int close(int sockfd);
			close(listenfd);
			str_echo( (void*) &connfd );
			exit(0);
		}
		close(connfd);// passed it to child, so close it!!
/*		// ### release all exited child before accept new connections.
		// pid_t waitpid(pid_t pid, int* status, int options);
		while( (childpid = waitpid(0, &status, WNOHANG)) > 0 )
			printf("pid:%ld> child exit, status:%d\n", (long) childpid, status);
*/ // ### now use signal() instead! otherwise there is always one zombie.

#elif defined(_USE_PTHREAD)
		pthread_create(&childpid, NULL, str_echo, (void*) &connfd);
#endif
	}
}


int main(int argc, char** argv)
{
	pid_t	childpid;
	int	childnum = 10;
	int	port = 11001;
	int 	i = 0;
	int	status;
	for (; i < childnum; i++)
	{
#ifdef _USE_FORK
		if ( 0 == (childpid = fork()) )
		{
			svr_func(&port);
			exit(0);
		}
#endif
		port++;
	}
	while( (childpid = waitpid(-1, &status, WNOHANG)) > 0 )
		printf("pid:%ld> child terminated, status:%d\n", (long) childpid, status);
	return 0;
}

