#ifndef _ORACLE_COMMON_H
#define _ORACLE_COMMON_H

#include <ctime>
//#include <SysLogManager.h>

//#include <Macro.h>
#include "uosdef.h"
#include <occi.h>
//using namespace oracle::occi;

/*
#define TRY_ORA_EXCEPTION try
#define CATCH_ORA_EXCEPTION(DB,LOCATION) \
catch (SQLException& ex)\
{\
    printf("%s\n", ex.what());\
    DB.printException(ex, LOCATION);\
    nResult = E_SYS_DATABASE_ERROR;\
    goto ExitError;\
}

#define PROCESS_ORA_CONNECTION_INIT(RET_CODE, DB, CONNECTION) \
RET_CODE = DB.getConnection(&CONNECTION);\
if (RET_CODE != S_SUCCESS || CONNECTION == NULL)\
{\
    nResult = E_SYS_DATABASE_CONNECT_ERROR;\
    goto ExitError;\
}

#define PROCESS_ORA_CONNECTION_INIT_RET(RET_CODE, DB, CONNECTION, RETURN) \
RET_CODE = DB.getConnection(&CONNECTION);\
if (RET_CODE != S_SUCCESS || CONNECTION == NULL)\
{\
    RETURN = RET_CODE;\
    goto ExitError;\
}
*/

#define PROCESS_ORA_CONNECTION_RELEASE(LOCATION, DB, CONNECTION, STATEMENT) \
try\
{\
    if (CONNECTION != NULL)\
    {\
        if (STATEMENT != NULL)\
        {\
            if (nResult == S_SUCCESS)\
            {\
                CONNECTION->commit();\
            }\
            else\
            {\
                CONNECTION->rollback();\
            }\
            CONNECTION->terminateStatement(STATEMENT);\
            STATEMENT = NULL;\
        }\
        DB.releaseConnection(CONNECTION);\
        CONNECTION = NULL;\
    }\
}\
catch (SQLException& ex)\
{\
    nResult = E_SYS_DATABASE_ERROR;\
    DB.printException(ex, LOCATION);\
}

int OracleDateToTime(oracle::occi::Date& date, time_t *ptmTime);

int TimeToOracleDate(time_t tmTime, oracle::occi::Date* pDate);

int UINT64ToOracleNumber(uint64_t ullNumber, oracle::occi::Number& number);

int OracleNumberToUINT64(oracle::occi::Number number, uint64_t& ullNumber);

//int PrintSQLException(oracle::occi::SQLException& ex, const char *strLocation);


#endif//(_ORACLE_COMMON_H)


