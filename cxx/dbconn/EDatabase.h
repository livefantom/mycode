#ifndef __E_DATABASE_H__
#define __E_DATABASE_H__
#include <Macro.h>
#include <occi.h>

#include "logger.h"

typedef struct _DATABASE_PARAM_
{
    //char strHostName[32];
    //uint16_t usPort;
    //char strServiceName[32];
    //char strDatabase[32];
    //char strConnectionString[256];
    char szConnString[MAX_BUFFER_SIZE];

    char strUserName[32];
    char strPassword[32];

    int  nUseDatabasePool;

    int  nMinConnection;
    int  nMaxConnection;

    int  nMaxTimeOut;
    int  nStatementCacheSize;


}DATABASE_PARAM;

class EDatabase
{
public:
    EDatabase(void);
    ~EDatabase(void);
    int initialize(const DATABASE_PARAM& databaseParam);
    void finalize();

    int getConnection(oracle::occi::Connection **ppConnection);
    int releaseConnection(oracle::occi::Connection *pConnection);
    int printException(const oracle::occi::SQLException& ex, const char *pcLocation);

    const oracle::occi::Environment* environment() const;

protected:
    int initEnvironment();
    int releaseEnvironment();
    int isAlive(oracle::occi::Connection *pConnection);

private:
    DATABASE_PARAM m_databaseParam;

    oracle::occi::Environment      *m_pEnvironment;
    oracle::occi::Connection              *m_pConnection;
    oracle::occi::StatelessConnectionPool *m_pStatelessConnectionPool;

    uos::Mutex m_mtx;
    uos::Logger m_log;

};

inline const oracle::occi::Environment* EDatabase::environment() const
{
    return m_pEnvironment;
}

#endif

