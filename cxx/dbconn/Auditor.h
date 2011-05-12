#ifndef _AUDITOR_H
#define _AUDITOR_H

#include<EDatabase.h>


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



class SysConfig;


class Auditor
{
public:
    Auditor();
    ~Auditor(){};

    int initialize(const char* szConfPath = "eStatServer.ini");
    int release();

    int doIt();

protected:
    int loadServerParam(const char* szConfPath);
    int loadDBParam(const SysConfig& cfg);
	int loadTns(const char *tnsName, char *outbuf, int str_len);

private:
    EDatabase       m_eDatabase;
    SERVER_PARAM    m_serverParam;
    uos::Logger     _log;

};





#endif//(_AUDITOR_H)
