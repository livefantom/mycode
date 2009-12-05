#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

void sig_proc(int signo, siginfo_t* info, void* context)
{
	if (signo == SIGCHLD)
	{
		printf("child exit detected!\n");
	    pid_t   pid;
	    int status;
	    while( (pid = waitpid(-1, &status, WNOHANG)) > 0 )
	        printf("pid:%ld> sig_chld> child:%ld terminated, status:%d\n", (long)getpid(), (long) pid, status);
    }
}

int main(int argc, char** argv)
{
	pid_t childpid;
	int num = 0;
	if (argc > 1)
		num = atoi(argv[1]);
    int status;
//	daemon(1, 1);
    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = sig_proc;
	sigaction(SIGCHLD, &sigact, 0);
//	while(1)
	{
		if( 0 == (childpid = fork()) )
		{
			// this is child.
			char arg[256] = {0};
			sprintf(arg, "%d", num);
			// int execl(const char* path, const char* arg0, ...);
			if ( execl("./svr", "svr", arg, 0) != 0 )
			{
				printf("execl error:%d:%s\n", errno, strerror(errno));
			}
		}
		
		if( 0 == (childpid = fork()) )
		{
			// this is child.
			char arg[256] = {0};
			sprintf(arg, "%d", 2*num);
			// int execl(const char* path, const char* arg0, ...);
			if ( execl("./svr", "svr", arg, 0) != 0 )
			{
				printf("execl error:%d:%s\n", errno, strerror(errno));
			}
		}
	}

    do
    {
    	if ((childpid = waitpid(-1, &status, 0)) > 0)
        	printf("pid:%ld> sig_chld> child:%ld terminated, status:%d\n", (long)getpid(), (long) childpid, status);
       	else if (childpid == -1 && errno == EINTR)
       	{
       		printf("-----------EINTR!\n");
       		continue;
       	}
	} while( childpid > 0);
	
	return 0;
}
