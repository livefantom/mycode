#include "EStatistic.h"
#include "OraCommon.h"
#include <SysConfig.h>
#include <sys/timeb.h>

extern std::string AGIPDecrypt(const std::string& src, const std::string& key_path);
//extern std::string AGIPEncrypt(const std::string& src, const std::string& key_path);

EStatistic::EStatistic(void)
{
}

EStatistic::~EStatistic(void)
{
}

int EStatistic::initialize(const char* szConfPath)
{
    int nRetCode = E_ERROR;
    int nResult = E_ERROR;

    // Load Server Config.
    printf("Loading Server Config...\n");
    nRetCode = loadServerParam(szConfPath);
    PROCESS_CHECK_ERROR_RET(nRetCode == S_SUCCESS, nResult, nRetCode);
    printf("Config loaded successfully!\n");

    // Init Log Manager
    printf("Initializing LOG manager...\n");
    printf("\tLOG path has set to '%s'!\n", m_serverParam.szLogPath);
    nRetCode = SysLogManager::initialize(m_serverParam.szLogPath);
    PROCESS_CHECK_ERROR_RET(nRetCode == S_SUCCESS, nResult, nRetCode);
    printf("LOG manager initialized successfully!\n");

    // Initialize EDatebase.
    printf("Initializing EDatebase...\n");
    nRetCode = m_eDatabase.initialize(m_serverParam.database_Param);
    PROCESS_CHECK_ERROR_RET(nRetCode == S_SUCCESS, nResult, nRetCode);
    printf("EDatebase initialized successfully!\n");

    nResult = S_SUCCESS;
ExitError:
    return nResult;

}

int EStatistic::release()
{
    int nResult  = E_ERROR;
    int nRetCode = E_ERROR;

    nRetCode = m_eDatabase.release();
    PROCESS_CHECK_ERROR_RET(nRetCode == S_SUCCESS, nResult, nRetCode);

    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

int EStatistic::loadDBParam(const SysConfig& cfg)
{
    int nRetCode = E_ERROR;
    int nResult  = E_ERROR;

    int nIntVal(0);
    char encryptPasswd[128] = {0};

    DATABASE_PARAM& dbParam = m_serverParam.database_Param;

    cfg.getIntVal("db_use_pool", &nIntVal, 0);
    if (nIntVal > 1 || nIntVal < 0)
    {
        printf("Invalid `db_use_pool' option value:%d\n", nIntVal);
        goto ExitError;
    }
    dbParam.nUseDatabasePool = nIntVal;

    cfg.getIntVal("db_timeout", &nIntVal, 60);
    if (nIntVal <= 0)
    {
        printf("Invalid `db_timeout' option value:%d\n", nIntVal);
        goto ExitError;
    }
    dbParam.nMaxTimeOut = nIntVal;

    cfg.getStringVal("db_host", dbParam.strHostName, "0.0.0.0");

    cfg.getIntVal("db_port", &nIntVal, 1521);
    dbParam.usPort = (uint16_t)nIntVal;

    cfg.getIntVal("db_min_connection", &nIntVal, 1);
    if ( 1 == dbParam.nUseDatabasePool && nIntVal <= 0 )
    {
        printf("Invalid `db_min_connection' option value:%d\n", nIntVal);
        goto ExitError;
    }
    dbParam.nMinConnection = nIntVal;

    cfg.getIntVal("db_max_connection", &nIntVal, 1);
    if ( 1 == dbParam.nUseDatabasePool && nIntVal < dbParam.nMinConnection )
    {
        printf("Invalid `db_max_connection' option value:%d\n", nIntVal);
        goto ExitError;
    }
    dbParam.nMaxConnection = nIntVal;

    cfg.getIntVal("db_stm_cache_size", &nIntVal, 1);
    if (nIntVal <= 0)
    {
        printf("Invalid `db_stm_cache_size' option value:%d\n", nIntVal);
        goto ExitError;
    }
    dbParam.nStatementCacheSize = nIntVal;

    cfg.getStringVal("db_username", dbParam.strUserName);
    dbParam.strUserName[sizeof(dbParam.strUserName) - 1] = '\0';

    cfg.getStringVal("db_password", encryptPasswd);
    strncpy(dbParam.strPassword, AGIPDecrypt(encryptPasswd, "AGIP.key").c_str(), 32);
    dbParam.strPassword[sizeof(dbParam.strPassword) - 1] = '\0';

    cfg.getStringVal("db_service_name", dbParam.strServiceName);
    dbParam.strServiceName[sizeof(dbParam.strServiceName) - 1] = '\0';

    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

int EStatistic::loadServerParam(const char* szConfPath)
{
    int nRetCode = E_ERROR;
    int nResult  = E_ERROR;

    int nLineParsed = 0;
    int nIntVal = 0;

    SysConfig sysConfig;
    nRetCode = sysConfig.initialize();
    ASSERT(nRetCode == S_SUCCESS);

    nRetCode = sysConfig.load(szConfPath, &nLineParsed);
    if (nRetCode != S_SUCCESS)
    {
        printf("Server config file load error!\n");
        goto ExitError;
    }

    if ( loadDBParam(sysConfig) != S_SUCCESS )
    {
        printf("Database config file load error!\n");
        goto ExitError;
    }
    
    m_serverParam.nDBHeartIntervalSecond    = 30;
    m_serverParam.nSleepMillisecond         = 1000;
    m_serverParam.sActiveTimeIntervalSecond = 20;

    //get Thread Stack Size
    sysConfig.getIntVal("thread_stack_size", &nIntVal, 0);
    m_serverParam.nThreadStackSize = nIntVal;

    //get Log Path
    sysConfig.getStringVal("Log_Path", m_serverParam.szLogPath, "");
    m_serverParam.szLogPath[sizeof(m_serverParam.szLogPath) - 1]   = '\0';

    sysConfig.getStringVal("server_ip", m_serverParam.szServerIP, "127.0.0.1");
    m_serverParam.szServerIP[sizeof(m_serverParam.szServerIP) - 1] = '\0';

    sysConfig.getIntVal("server_port", &nIntVal, 8888);
    m_serverParam.usServerPort = (uint16_t)nIntVal;

    sysConfig.getIntVal("allow_delay_seconds", &nIntVal, 10);
    m_serverParam.nAllowDelaySeconds = nIntVal;

    nResult = S_SUCCESS;
ExitError:
    return nResult;

}

int EStatistic::loadTaskList(const char* szTaskPath)
{
    int nResult  = E_ERROR;
    int nRetCode = E_ERROR;

    STAT_TASK   sTask;
#ifdef _TEST
    sTask.unInterval = 60;
    sTask.unOffset = 5;
    sTask.unAllowDelay = m_serverParam.nAllowDelaySeconds;
    sTask.strFuncName = "PKG_STAT.test(null)";
    m_vectTask.push_back(sTask);
#endif
    sTask.unInterval = 300;
    sTask.unOffset = 0;
    sTask.unAllowDelay = m_serverParam.nAllowDelaySeconds;
    sTask.strFuncName = "PKG_STAT.onlineUserCount";
    m_vectTask.push_back(sTask);

    sTask.unInterval = 300;
    sTask.unOffset = 0;
    sTask.unAllowDelay = m_serverParam.nAllowDelaySeconds;
    sTask.strFuncName = "PKG_STAT.regionalonlineusercount";
    m_vectTask.push_back(sTask);

    return S_SUCCESS;
}

int EStatistic::work()
{
    int nResult  = E_ERROR;
    int nRetCode = E_ERROR;

    timeb   stb_current;
    timeb   stb_begin;
    TASK_ITER iter;
    TASK_ITER iter_end = m_vectTask.end();
    while (true)
    {
        ftime(&stb_begin);
#ifdef _TEST
        char szTime[128] = {0};
        strcpy(szTime, ctime(&stb_begin.time));
        szTime[strlen(szTime) - 1] = 0;
        printf("%s .%03d\n", szTime, stb_begin.millitm);
#endif
        // 遍历执行全部任务，小间隔的优先
        iter = m_vectTask.begin();
        for (;iter != iter_end; iter++)
        {
            ftime(&stb_current);
            // 执行统计函数
            if (iter->unOffset == 0 && stb_current.time % iter->unInterval == 0)
            {
                nRetCode = execStatFunc(iter->strFuncName);
                LOGGER(SysLog::LOG_INFO, "%s|%d|\n", iter->strFuncName.c_str(), nRetCode);
            }
            else if ( stb_current.time % iter->unInterval == iter->unOffset )
            {
                nRetCode = execStatFunc(iter->strFuncName);
                LOGGER(SysLog::LOG_INFO, "%s|%d|\n", iter->strFuncName.c_str(), nRetCode);
            }
            else
            {
                // 已消耗时间（秒）
                if (stb_current.time == stb_begin.time)
                {
                    continue;
                }

                // 如果本轮执行时间超过1秒
                time_t tm_past = stb_begin.time;
                for (; tm_past < stb_current.time; ++tm_past)
                {
                    // 如果延时超过容差
                    if ( (stb_current.time - tm_past) > iter->unAllowDelay )
                    {
                        nRetCode = -1;
                        LOGGER(SysLog::LOG_INFO, "%s|%d|ExecTime passed!\n", iter->strFuncName.c_str(), nRetCode);
                        break;
                    }
                    // 如果时间点符合，且延时在允许范围内
                    if (iter->unOffset == 0 && tm_past % iter->unInterval == 0)
                    {
                        execStatFunc(iter->strFuncName);
                        LOGGER(SysLog::LOG_INFO, "%s|%d|\n", iter->strFuncName.c_str(), nRetCode);
                        break;
                    }
                    else if ( tm_past % iter->unInterval == iter->unOffset )
                    {
                        nRetCode = execStatFunc(iter->strFuncName);
                        LOGGER(SysLog::LOG_INFO, "%s|%d|\n", iter->strFuncName.c_str(), nRetCode);
                        break;
                    }
                }
            }
        }

        ftime(&stb_current);
        // 本轮任务在1s内完成
        if (stb_current.time == stb_begin.time)
        {
            // 休息到下一秒
            usleep( 1000 * (1000 - stb_current.millitm) );
        }
    }

    m_vectTask.clear();
    return S_SUCCESS;
}

int EStatistic::execStatFunc(const std::string& strFuncName)
{
    int nResult  = E_ERROR;
    int nRetCode = E_ERROR;

    const char* strLocation = "EStatistic::execStatFunc";

    Statement*  pStatement  = NULL;
    Connection* pConnection = NULL;
    std::string      strSQL = "begin :1 := ";

    PROCESS_ORA_CONNECTION_INIT_RET(nRetCode, m_eDatabase, pConnection, nResult);

    strSQL += strFuncName;
    strSQL += "; end;";
    TRY_ORA_EXCEPTION
    {
        pStatement = pConnection->createStatement();
        pStatement->setSQL(strSQL);
        pStatement->registerOutParam(1, OCCIINT);
        pStatement->execute();
        nRetCode = pStatement->getInt(1);
    }
    CATCH_ORA_EXCEPTION(strLocation);

    nResult = nRetCode;
ExitError:
    PROCESS_ORA_CONNECTION_RELEASE(strLocation, m_eDatabase, pConnection, pStatement);
    return nResult;
}


int EStatistic::execFunction(const std::string& strFuncName)
{
    int nResult  = E_ERROR;
    int nRetCode = E_ERROR;

    const char* strLocation = "EStatistic::execFunction";

    //printf("%s\n", AGIPDecrypt(AGIPEncrypt("abcdefghigklmn", "AGIP.key"), "AGIP.key").c_str());

    Statement*  stmt  = NULL;
    Connection* conn = NULL;
	std::string      strSQL = "begin :1 := ";

    PROCESS_ORA_CONNECTION_INIT_RET(nRetCode, m_eDatabase, conn, nResult);

    //strSQL += strFuncName;
    strSQL += "PKG_AUDIT.getUserForAudit(123, 3, :2); end;";
    TRY_ORA_EXCEPTION
    {
        stmt = conn->createStatement();
        stmt->setSQL(strSQL);
        stmt->registerOutParam(1, OCCIINT);
        stmt->registerOutParam(2, OCCICURSOR);
        stmt->setPrefetchRowCount(10);
        stmt->execute();
        nRetCode = stmt->getInt(1);
        ResultSet* rs = stmt->getCursor(2);
        while (rs->next())
        {
            printf("%d\n", rs->getUInt(1));
        }
    }
    CATCH_ORA_EXCEPTION(strLocation);

    nResult = nRetCode;
ExitError:
    PROCESS_ORA_CONNECTION_RELEASE(strLocation, m_eDatabase, conn, stmt);
    return nResult;
}

