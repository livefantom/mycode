#include "Auditor.h"
#include "OraCommon.h"
#include "SysConfig.h"
#include "socket.h"
//#include <sys/timeb.h>
#ifndef WIN32
#   include <sys/time.h>
#endif

#ifdef MAX_BUFFER_SIZE
#   undef MAX_BUFFER_SIZE
#endif
#define MAX_BUFFER_SIZE 4096

using namespace oracle::occi;

extern std::string AGIPDecrypt(const std::string& src, const std::string& key_path);
//extern std::string AGIPEncrypt(const std::string& src, const std::string& key_path);

static uint64_t getmillisecs()
{
    struct timeval tv;
    (void) gettimeofday( &tv, (struct timezone*) 0 );
    return (uint64_t)(tv.tv_sec) * 1000L + tv.tv_usec / 1000L;
}

Auditor::Auditor()
: _log()
{
}


int Auditor::initialize(const char* szConfPath)
{
    int nRetCode = E_ERROR;
    int nResult = E_ERROR;

    // Load Server Config.
    printf("Loading Server Config...\n");
    nRetCode = loadServerParam(szConfPath);
    PROCESS_CHECK_ERROR_RET(nRetCode == S_SUCCESS, nResult, nRetCode);
    printf("Config loaded successfully!\n");

    // Init Log Manager
    printf("Initializing Logger...\n");
    printf("\tLOG path has set to '%s'!\n", m_serverParam.szLogPath);
    nRetCode = _log.initialize(m_serverParam.szLogPath);
    PROCESS_CHECK_ERROR_RET(nRetCode == S_SUCCESS, nResult, nRetCode);
    printf("Logger initialized successfully!\n");

    // Initialize EDatebase.
    printf("Initializing EDatebase...\n");
    nRetCode = m_eDatabase.initialize(m_serverParam.database_Param);
    PROCESS_CHECK_ERROR_RET(nRetCode == S_SUCCESS, nResult, nRetCode);
    printf("EDatebase initialized successfully!\n");

    nResult = S_SUCCESS;
ExitError:
    return nResult;

}

int Auditor::release()
{
    _log.release();
    return S_SUCCESS;
}

int Auditor::loadDBParam(const SysConfig& cfg)
{
    int nRetCode = E_ERROR;
    int nResult  = E_ERROR;

    int nIntVal(0);
    char encryptPasswd[128] = {0};
	char tnsName[20] = {0};

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

	cfg.getStringVal("tnsname", tnsName, "LINEKONG");
	nRetCode = loadTns(tnsName, dbParam.szConnString, MAX_BUFFER_SIZE);
	if( nRetCode != S_SUCCESS )
	{
		goto ExitError;
	}
 
	printf("tnsName = %s\n", dbParam.szConnString);
    nResult = S_SUCCESS;
ExitError:
    return nResult;
}

int Auditor::loadServerParam(const char* szConfPath)
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

    sysConfig.release();

    nResult = S_SUCCESS;
ExitError:
    return nResult;

}


int Auditor::doIt()
{
    int retcode = E_ERROR;
    int result  = E_ERROR;

    //retcode = queryRange(gs_table);
    PROCESS_CHECK_ERROR_RET(retcode == S_SUCCESS, result, retcode);

    result = S_SUCCESS;
ExitError:
    return result;
}

int Auditor::loadTns(const char *tnsName, char *outbuf, int str_len)
{
	if( tnsName == NULL || outbuf == NULL || str_len < 1 )
	{
		printf("load tns param error.\n");
		return E_ERROR;
	}
	FILE *fp;
	char buf[10000] = {0};
	char *substr;//this pointer is operated in the char array buf[10000]
	if( !(fp = fopen("tnsnames.ora","r")) )
	{
		printf("Can't open file tnsnames.ora\n");
		return E_SYS_FILE_NOT_FOUND;
	}
	fread(buf,1,10000,fp);
	substr = strstr(buf,tnsName);
	if(substr == NULL)
	{
        printf("No string %s included in file tnsnames.ora\n",tnsName);
        return E_SYS_CONFIG_ERROR;
	}
	while( strchr("= \n\r\t",*(substr + strlen(tnsName))) == NULL)
	{
		substr++;
		substr = strstr(substr,tnsName);
		if(substr == NULL)
		{
			printf("No string %s included in filie tnsnames.ora\n",tnsName);
			return E_SYS_CONFIG_ERROR;
		}
	}
	//printf("go to makestring.\n");

	char *str1 = substr;
	char *str2;
    while(*str1 != 0)//find the first '(' or ')'
    {
        if(*str1 == ')')
        {
            printf("Error string in tnsnames.ora.\n");
            return E_SYS_CONFIG_ERROR;
        }
        if(*str1 == '(')
        {
            break;
        }
		str1++;
    }
	if( *str1 == 0 )
	{
		printf("Error string in tnsnames.ora.\n");
		return E_SYS_CONFIG_ERROR;
	}

	substr = str1;//drop the string head before the first '(', and find tail of the string
	str2 = strstr(str1,"DESCRIPTION");
	str2 ++;
	str2 = strstr(str2,"DESCRIPTION");
	if( str2 == NULL )
	{
		str2 = str1;
		while( *str2 != 0 )
		  str2 ++;
	}
	while(*str2 != ')')
	{
		str2 --;
	}
	*(str2 + 1) = 0;

	if( str2 < str1 )
	{
		printf("Error string in tnsnames.ora.\n");
		return E_SYS_CONFIG_ERROR;
	}

	str2 = str1;
	while( *str1 != 0 )// trim all blank and \r\n
	{
		if( *str1 == ' '||*str1 == 10||*str1 == 13 )
		{
			str1++;
		}
		else 
		{
			*str2 = *str1;
			str1++;
			str2++;
		}
	}
	*str2 = 0;

	str1 = substr;
	str2 = substr;

	//find the front part
	if( strncmp(str1,"(DESCRIPTION=(ADDRESS_LIST=",27) != 0 )
	{
		printf("Error string in tnsnames.ora\n");
		return E_SYS_CONFIG_ERROR;
	}
	//match the address string
	str2 = str2 + 27;
	while( *str2 != ')')
	{
		if( strncmp(str2,"(ADDRESS=(PROTOCOL=",19) != 0 )
		{
			printf("Error string in tnsnames.ora:address error.\n");
			return E_SYS_CONFIG_ERROR;
		}
		while( *str2 != ')' )
			str2++;
		if(*str2 != ')'&&strncmp(str2 + 1,"(HOST=",6) != 0)
		{
			printf("Error string in tnsnames.ora:address error.\n");
			return E_SYS_CONFIG_ERROR;
		}
		str2++;
		while( *str2 != ')')
			str2++;
		if(*str2 != ')'&&strncmp(str2 + 1,"(PORT=",6) != 0)
		{
			printf("Error string in tnsnames.ora:address error.\n");
			printf("\n");
			return E_SYS_CONFIG_ERROR;
		}
		str2++;
		while( *str2 != ')')
			str2++;
		str2++;//now str2 should be in the finish of the address string
		if( *(str2) != ')')
		{
			printf("Error string in tnsnames.ora:address error.\n");
			return E_SYS_CONFIG_ERROR;
		}
		str2++;
	}
	if(*str2 != ')'&&strncmp(str2,"(CONNECT_DATA=",14) != 0)
	{
		printf("Error string in tnsnames.ora:CONNECT_DATA error\n");
		return E_SYS_CONFIG_ERROR;
	}
	//match the server string
	str2 = str2 + 15;
	while( *str2 != ')')
	{
		if( strncmp(str2,"(SERVER=",8) != 0&&strncmp(str2,"(SERVICE_NAME=",14) != 0 )
		{
			printf("Error string in tnsnames.ora:CONNECT_DATA error.\n");
			return E_SYS_CONFIG_ERROR;
		}
		while( *str2 != ')' )
			str2++;//now str2 should be in the finish of the service string
		str2++;
	}
	//the tail of string should be "))\0"
	if(*(str2 + 1) != ')'||*(str2 + 2) != 0)
	{
		printf("Error string in tnsnames.ora:tns name tail error\n");
		return E_SYS_CONFIG_ERROR;
	}
	if( strlen(substr) > (unsigned long)str_len )
	{
		printf("Error string in tnsnames:too long.\n");
		return E_SYS_CONFIG_ERROR;
	}
	memcpy(outbuf,substr,strlen(substr));
	return S_SUCCESS;
}

