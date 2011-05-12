#include "EDatabase.h"
#include <time.h>

using namespace oracle::occi;

int OracleDateToTime(Date& date, time_t *ptmTime)
{
    assert(ptmTime != NULL);

    int          nYear    = 0;
    unsigned int unMonth  = 0;
    unsigned int unDay    = 0;
    unsigned int unHour   = 0;
    unsigned int unMinute = 0;
    unsigned int unSecond = 0;
    struct tm    stmTime;
    memset(&stmTime, 0, sizeof(tm));

    if (date.isNull())
    {
        *ptmTime = 0;
    }
    else
    {
        date.getDate(nYear, unMonth, unDay, unHour, unMinute, unSecond);
        stmTime.tm_year = nYear   - 1900;
        stmTime.tm_mon  = unMonth - 1;
        stmTime.tm_mday = unDay;
        stmTime.tm_hour = unHour;
        stmTime.tm_min  = unMinute;
        stmTime.tm_sec  = unSecond;
        *ptmTime = mktime(&stmTime);
    }
    return S_SUCCESS;
}

int TimeToOracleDate(time_t tmTime, Date* pDate)
{
    assert(pDate != NULL);

    tm* pstmTime  = NULL;
    if (tmTime == 0)
    {
        pDate->setNull();
    }
    else
    {
        pstmTime = localtime(&tmTime);
        pDate->setDate(
            pstmTime->tm_year + 1900,
            pstmTime->tm_mon  + 1,
            pstmTime->tm_mday,
            pstmTime->tm_hour,
            pstmTime->tm_min,
            pstmTime->tm_sec
            );
    }
    return S_SUCCESS;
}

int UINT64ToOracleNumber(uint64_t ullNumber, Number& number)
{
    uint32_t unNumberHiger = (uint32_t)(ullNumber >> 32);
    uint32_t unNumberLower = (uint32_t)(ullNumber & 0xFFFFFFFF);
    Number numMax = 2147483647; // 2^ 31 - 1
    numMax += 1;
    numMax *= 2;

    number  = unNumberHiger;
    number *= numMax;
    number += unNumberLower;

    return S_SUCCESS;

}

int OracleNumberToUINT64(Number number, uint64_t& ullNumber)
{
    Number numMax = 2147483647; // 2^ 31 - 1
    numMax += 1;
    numMax *= 2;

    uint32_t unNumberLower = number % numMax;
    Number numHiger = (number / numMax).floor();
    uint64_t ullNumberHiger = (uint32_t)(numHiger % numMax);

    ullNumber = (ullNumberHiger << 32) + unNumberLower;
    return S_SUCCESS;
}

EDatabase::EDatabase(void)
: m_pEnvironment(0)
, m_pConnection(0)
, m_pStatelessConnectionPool(0)
{
    memset(&m_databaseParam, 0, sizeof(DATABASE_PARAM));
}

EDatabase::~EDatabase(void)
{
    finalize();
}

int EDatabase::initialize(const DATABASE_PARAM &databaseParam)
{
    int retcode = E_ERROR;

    memcpy(&m_databaseParam, &databaseParam, sizeof(DATABASE_PARAM));
    //sprintf(m_databaseParam.strConnectionString,
    //    "(DESCRIPTION ="
    //    "   (ADDRESS = (PROTOCOL = TCP)(HOST = %s)(PORT = %u))"
    //    "   (CONNECT_DATA ="
    //    "       (SERVER = DEDICATED)"
    //    "       (SERVICE_NAME = %s)"
    //    "   )"
    //    ")",
    //    m_databaseParam.strHostName,
    //    m_databaseParam.usPort,
    //    m_databaseParam.strServiceName
    //);
    m_databaseParam.szConnString[sizeof(m_databaseParam.szConnString) - 1] = '\0';

    retcode = m_log.initialize("logs", 1024, "ora_", uos::Logger::LOG_INFO);
    if (retcode != S_SUCCESS)
    {
        printf("logger initialize failed.\n");
    }
    return initEnvironment();
}

int EDatabase::initEnvironment()
{
    int nResult  = E_ERROR;

    m_mtx.lock();
    if (EDatabase::m_pEnvironment == NULL)
    {
        try
        {
            EDatabase::m_pEnvironment = Environment::createEnvironment(Environment::Mode(Environment::THREADED_MUTEXED | Environment::OBJECT));
        }
        catch (SQLException& ex)
        {
            printException(ex, "EDatabase::initEnvironment");
            goto ExitError;
        }
    }
    if (EDatabase::m_pEnvironment == NULL)
    {
        goto ExitError;
    }

    if (m_databaseParam.nUseDatabasePool == 1 && m_pStatelessConnectionPool == NULL)
    {
        try
        {
            m_pStatelessConnectionPool = EDatabase::m_pEnvironment->createStatelessConnectionPool(
                m_databaseParam.strUserName,
                m_databaseParam.strPassword,
                m_databaseParam.szConnString,
                m_databaseParam.nMaxConnection,
                m_databaseParam.nMinConnection,
                1,
                StatelessConnectionPool::HOMOGENEOUS
                );
            m_pStatelessConnectionPool->setTimeOut(m_databaseParam.nMaxTimeOut);
            m_pStatelessConnectionPool->setStmtCacheSize(m_databaseParam.nStatementCacheSize);
        }
        catch (SQLException& ex)
        {
            printException(ex, "EDatabase::initEnvironment");
            goto ExitError;
        }
    }
    else if (m_databaseParam.nUseDatabasePool == 0) // Directly connection
    {
        try
        {
            m_pConnection = EDatabase::m_pEnvironment->createConnection(
                m_databaseParam.strUserName,
                m_databaseParam.strPassword,
                m_databaseParam.szConnString
                );
            if (m_pConnection == NULL)
            {
                goto ExitError;
            }
        }
        catch (SQLException& ex)
        {
            printException(ex, "EDatabase::initEnvironment");
            goto ExitError;
        }
    }
    printf("Oracle connection established, database initialized.\n");

    nResult = S_SUCCESS;
ExitError:

    if (nResult != S_SUCCESS)
    {
        releaseEnvironment();
    }
    m_mtx.unlock();
    return nResult;
}


void EDatabase::finalize(void)
{
    releaseEnvironment();
}

int EDatabase::getConnection(Connection **ppConnection)
{
    int nRetCode = E_ERROR;
    int nResult  = E_ERROR;

    ASSERT(ppConnection != NULL);
    *ppConnection = NULL;

    if (m_databaseParam.nUseDatabasePool == 0) // Connect directly...
    {
        if (m_pConnection == NULL)
        {
            try
            {
                m_pConnection = EDatabase::m_pEnvironment->createConnection(
                    m_databaseParam.strUserName,
                    m_databaseParam.strPassword,
                    m_databaseParam.szConnString
                );
            }
            catch (SQLException &ex)
            {
                printException(ex, "EDatabase::getConnection");
                nResult = E_SYS_DATABASE_ERROR;
                goto ExitError;
            }

        }
        if (m_pConnection == NULL)
        {
            nResult = E_SYS_DATABASE_ERROR;
            goto ExitError;
        }
        *ppConnection = m_pConnection;
    }
    // To Use Pool..............
    else
    {

        if (m_pStatelessConnectionPool == NULL)
        {
            nRetCode = initialize(m_databaseParam);
            if (nRetCode != S_SUCCESS || m_pStatelessConnectionPool == NULL)
            {
                nResult = E_SYS_DATABASE_ERROR;
                goto ExitError;
            }
        }
        try
        {
            *ppConnection = m_pStatelessConnectionPool->getConnection();
            if (*ppConnection == NULL)
            {
                nResult = E_SYS_DATABASE_ERROR;
                goto ExitError;
            }
        }
        catch (SQLException& ex)
        {
            printException(ex, "EDatabase::getConnection");
            nResult = E_SYS_DATABASE_ERROR;
            goto ExitError;
        }

    }

//ExitOK:
    nResult = S_SUCCESS;
ExitError:
    if (*ppConnection != NULL)
    {
        nRetCode = this->isAlive(*ppConnection);
        if ( nRetCode != S_SUCCESS)
        {
            this->releaseConnection(*ppConnection);
            *ppConnection = NULL;

            this->finalize();
            nResult = nRetCode;
        }
    }
    return nResult;
}

int EDatabase::releaseConnection(Connection *pConnection)
{
    int nResult  = E_ERROR;

    if (pConnection == NULL)
        return S_SUCCESS;

    if (m_databaseParam.nUseDatabasePool == 0) // Connect directly...
    {
        if (m_pConnection != NULL && m_pConnection != pConnection)
        {
            goto ExitError;
        }
    }
    else
    {
        try
        {
            m_pStatelessConnectionPool->releaseConnection(pConnection);
        }
        catch (SQLException& ex)
        {
            printException(ex, "EDatabase::releaseConnection");
            goto ExitError;
        }
    }

    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

int EDatabase::isAlive(Connection *pConnection)
{
    int nResult  = E_ERROR;
//    const char *strLocation = "EDatabase::isAlive";
    Statement  *pStatement  = NULL;

    if (pConnection == NULL)
    {
        nResult = E_SYS_DATABASE_ERROR;
        goto ExitError;
    }

    try
    {
        pStatement = pConnection->createStatement();
        pStatement->executeQuery("select 1 from dual");
    }
    catch(...)
    {
        nResult = E_SYS_DATABASE_ERROR;
        goto ExitError;
    }
    nResult = S_SUCCESS;
ExitError:
    if (pConnection != NULL)
    {
        if (pStatement != NULL)
        {
            try
            {
                pConnection->terminateStatement(pStatement);
                pStatement = NULL;
            }
            catch(...)
            {
                nResult = E_SYS_DATABASE_ERROR;
            }
        }
    }
    return nResult;
}

int EDatabase::releaseEnvironment()
{
    int nResult  = E_ERROR;
//    const char *strLocation = "EDatabase::releaseEnvironment";

    if (m_pEnvironment != NULL)
    {
        // finalize Connection or ConnnectionPool.
        try
        {
            if (m_pConnection != NULL)
            {
                m_pEnvironment->terminateConnection(m_pConnection);
                m_pConnection = NULL;
            }
            else if (m_pStatelessConnectionPool != NULL)
            {
                m_pEnvironment->terminateStatelessConnectionPool(m_pStatelessConnectionPool);
                m_pStatelessConnectionPool = NULL;
            }
            printf("Oracle Connection or ConnnectionPool finalized.\n");
        }
        catch (SQLException& ex)
        {
            printException(ex, "EDatabase::releaseEnvironment");
            goto ExitError;
        }

        // finalize Environment.
        try
        {
            Environment::terminateEnvironment(EDatabase::m_pEnvironment);
            m_pEnvironment = NULL;
        }
        catch (SQLException& ex)
        {
            printException(ex, "EDatabase::releaseEnvironment");
            nResult = E_SYS_DATABASE_ERROR;
            goto ExitError;
        }
    }
    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

int EDatabase::printException(const SQLException& ex, const char *pcLocation)
{
    char* pcExp = (char*)ex.what(); // throw()

    printf("%s - %s", pcLocation, pcExp);
    return m_log.output(uos::Logger::LOG_ERROR, "[ORA-%05d@%s] %s.\n", ex.getErrorCode(), pcLocation, pcExp);
}

