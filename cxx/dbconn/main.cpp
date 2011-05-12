// main.cpp : Defines the entry point for the console application.
//

#include "Auditor.h"
#include <signal.h>

bool g_bStop = false;

void sig_proc(int signo, siginfo_t* info, void* context)
{
	printf("signo=%d\n", signo);
   // if (signo == SIGTERM)
    {
        printf("^C detected!\n");
        g_bStop = true;
    }
}


int main(int argc, char* argv[])
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;

    daemon(1,1);

    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = sig_proc;
    sigaction(SIGTERM, &sigact, 0);
    sigaction(SIGINT, &sigact, 0);
    sigaction(SIGSTOP, &sigact, 0);

    std::string conf_path = "dbtest.ini";
    if (argc > 1)
    {
        conf_path = argv[1];
    }

    Auditor auditor;
    nRetCode = auditor.initialize(conf_path.c_str());
    if (nRetCode != S_SUCCESS)
    {
        printf("Initialize server Error!\n");
        goto ExitError;
    }

    auditor.doIt();

    auditor.release();

    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

