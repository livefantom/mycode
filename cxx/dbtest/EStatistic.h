#ifndef _E_STATISTIC_H
#define _E_STATISTIC_H

#include <EDatabase.h>


typedef struct _SERVER_PARAM_
{
    char     szServerIP[20];
    uint16_t usServerPort;
    int16_t  sActiveTimeIntervalSecond;
    int32_t  nSleepMillisecond;
    int32_t  nDBHeartIntervalSecond;
    int32_t  nAllowDelaySeconds;    // 允许的时间误差，单位秒
    int32_t  nThreadStackSize;      //线程堆栈大小
    char     szLogPath[64];        //服务器日志路径
    DATABASE_PARAM database_Param;
}SERVER_PARAM;

typedef struct stat_task
{
    uint32_t    unInterval;  // 执行间隔，以秒为单位。
    uint32_t    unOffset;   // 与基准时间的偏移。
    uint32_t    unAllowDelay;   // 延后的容忍时间。
    std::string  strFuncName;
} STAT_TASK;

typedef std::vector<STAT_TASK>       TASK_VECTOR;
typedef TASK_VECTOR::iterator   TASK_ITER;

class SysConfig;

class EStatistic
{
public:
    EStatistic(void);
    ~EStatistic(void);

    int initialize(const char* szConfPath = "eStatServer.ini");

    int loadTaskList(const char* szTaskPath = "eStatTask.conf");
    int work();
    int execStatFunc(const std::string& strFuncName);

    int execFunction(const std::string& strFuncName);

protected:
    int loadServerParam(const char* szConfPath);
    int loadDBParam(const SysConfig& cfg);
    int release();

private:
    EDatabase       m_eDatabase;
    SERVER_PARAM    m_serverParam;

    TASK_VECTOR     m_vectTask;
};





#endif//(_E_STATISTIC_H)
