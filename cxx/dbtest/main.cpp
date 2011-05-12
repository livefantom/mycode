// main.cpp : Defines the entry point for the console application.
//

#include "EStatistic.h"

int main(int argc, char* argv[])
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;


    std::string conf_path = "dbtest.ini";
    if (argc > 1)
    {
        conf_path = argv[1];
    }

    EStatistic eStatistic;
    nRetCode = eStatistic.initialize(conf_path.c_str());
    if (nRetCode != S_SUCCESS)
    {
        printf("Initialize server Error!\n");
        goto ExitError;
    }
    nRetCode = eStatistic.loadTaskList();
    if (nRetCode != S_SUCCESS)
    {
        printf("Load task list Error!\n");
        goto ExitError;
    }
//    eStatistic.work();
    eStatistic.execFunction("");

    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

