#ifdef WIN32
#   include <Windows.h>
#   define uint32_t unsigned int
#   define uint64_t unsigned long long
#else
#   include <inttypes.h>
#   include <iconv.h>
#endif

#include <mysql.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "c_str.h"

#define MEM_4K_BUFFER   4096

typedef struct mysql_conf
{
	char szHostIP[16];
	uint32_t unPort;
	char szUser[32];
	char szPasswd[32];
	char szDatabase[32];
} DB_CFG;

int rd_conf(DB_CFG* pConf)
{
	FILE* fp = fopen("mysql.ini", "r");
	if (NULL == fp)
	{
		printf("open file error\n");
		return -1;
	}
	char szConf[BUFSIZ] = "";
	while ( fgets(szConf, BUFSIZ, fp) )
	{
		if (ltrim(szConf)[0] != '#')
			break;
	}
	sscanf(szConf, "%s %u %s %s %s", pConf->szHostIP, &pConf->unPort, pConf->szUser,
			pConf->szPasswd, pConf->szDatabase);
	fclose(fp);
	return 0;
}

#ifndef WIN32
int code_convert(const char* dstcode, char* dstbuf, size_t* dstlen, const char* srccode, char* srcbuf, size_t* srclen)
{
	iconv_t cd = iconv_open(dstcode, srccode);
	if ( (iconv_t)-1 == cd )
		return -1;

	if ( -1 == iconv(cd, &srcbuf, srclen, &dstbuf, dstlen) )
		return -1;

	if ( -1 == iconv_close(cd) )
		return -1;

	return 0;
}
#endif

int mysql2csv(FILE* fp, const char* header, const char* sql, MYSQL* mysql)
{
    uint64_t rownum = 0;
    uint32_t fieldnum = 0;
    MYSQL_RES* pRes = NULL;

	uint32_t buflen = MEM_4K_BUFFER;
	char*   szBuf = (char*)malloc(buflen);
    char*   pch = szBuf;
    uint32_t datalen = 0;
#ifndef WIN32
	char*   szOutBuf = NULL;
	uint32_t unOutLeft = 0;
#endif

    // Query Data.
    if ( mysql_query(mysql, sql) )
    {
        printf("mysql_query error\n");
        return -1;
    }

    pRes = mysql_store_result(mysql);
    if (NULL == pRes)
    {
        printf("get result error\n");
        return -1;
    }
    rownum = mysql_num_rows(pRes);
    fieldnum = mysql_num_fields(pRes);

    // Write data to file.
    fprintf(fp, header);
	memset(szBuf, 0, buflen);
	for (; rownum > 0; rownum--)
	{
		MYSQL_ROW row = mysql_fetch_row(pRes);
		if (NULL == row)
			break;

		unsigned long* pLens = mysql_fetch_lengths(pRes);
		uint32_t i;
		for (i = 0; i < fieldnum; i++)
		{
			datalen = strlen(szBuf);
			// if no enough memory.
			if ( (datalen + *pLens + 2) >= buflen )
			{
				szBuf = (char*)realloc(szBuf, buflen + MEM_4K_BUFFER);
				memset(szBuf + buflen, 0, MEM_4K_BUFFER);
				buflen += MEM_4K_BUFFER;
			}
			strncat(pch, row[i], pLens[i]);
			strcat(pch, ",");
		}
		strcat(pch, "\n");
	}
	datalen = strlen(szBuf);
#ifndef _WIN32
	unOutLeft = 2 * datalen;
	szOutBuf = malloc(unOutLeft);
	memset(szOutBuf, 0, unOutLeft);
	if ( -1 == code_convert("gbk", szOutBuf, &unOutLeft, "utf8", szBuf, &datalen) )
	{
		perror("code conver error");
	}
	fwrite(szOutBuf, strlen(szOutBuf), 1, fp);
    free(szOutBuf);
    szOutBuf = NULL;
#else
	fwrite(szBuf, datalen, 1, fp);
#endif
	free(szBuf);
	szBuf = NULL;
	mysql_free_result(pRes);

	return 0;
}

int main(int argc, char** argv)
{
	DB_CFG cfg;
    char szSql[BUFSIZ] = "";
    char szFileName[BUFSIZ] = "";
    char szHeader[BUFSIZ] = "";
	memset( &cfg, 0, sizeof(DB_CFG) );
	if ( rd_conf(&cfg) )
	{
		printf("rd_conf error\n");
		return -1;
	}

	MYSQL *pDbm = mysql_init(NULL);
	if (NULL == pDbm)
	{
		printf("mysql_init error\n");
		return -1;
	}
	pDbm = mysql_real_connect(pDbm, cfg.szHostIP, cfg.szUser, cfg.szPasswd,
			cfg.szDatabase, cfg.unPort, NULL, 0);
	if (NULL == pDbm)
	{
		printf("mysql_real_connect error\n");
		return -1;
	}

	// Create cvs file.
	time_t time_now = time(0);
	struct tm* ptm_now = localtime( &time_now );
	sprintf(szFileName, "《问鼎》统计%02d月%02d日.csv", ptm_now->tm_mon + 1, ptm_now->tm_mday);
	FILE* fp = fopen(szFileName, "w");
	if (NULL == fp)
	{
		printf("open file error\n");
		return -1;
	}

	sprintf(szSql, "set names 'latin1'");
	if ( mysql_query(pDbm, szSql) )
	{
		printf("mysql_query error\n");
		return -1;
	}
	printf("New client character set: %s\n", mysql_character_set_name(pDbm));

	sprintf(szHeader, "激活帐号数,总角色数\n");
	sprintf(szSql,
			"select count(distinct(account)), count(*) from hddb.user \
			where (account between 'test001001' and 'test005000') \
			or (account between 'test002001' and 'test002434') \
			or (account between 'test003703' and 'test003811')");
	if ( mysql2csv(fp, szHeader, szSql, pDbm) )
	{
		printf("data query or write error\n");
		return -1;
	}

    sprintf(szHeader, "\n\n新增角色数\n");
    sprintf(szSql,
        "select count(*) from hddb.user where create_time \
        between (curdate()-1)*1000000 and curdate()*1000000");
    if ( mysql2csv(fp, szHeader, szSql, pDbm) )
    {
        printf("data query or write error\n");
        return -1;
    }

    sprintf(szHeader, "\n\n登录角色数,登录帐号数\n");
    sprintf(szSql,
        "select count(*), count(distinct(account)) from hddb.user where \
        last_login_time between (curdate()-1)*1000000 and curdate()*1000000");
    if ( mysql2csv(fp, szHeader, szSql, pDbm) )
    {
        printf("data query or write error\n");
        return -1;
    }

    sprintf(szHeader, "\n\n等级段分布\n人数,等级段\n");
	sprintf(szSql,
			"select count(*), 5*floor(level/5) as lv_stage from hddb.user \
			where (account between 'test001001' and 'test005000') \
			or (account between 'test002001' and 'test002434') \
			or (account between 'test003703' and 'test003811') \
			group by lv_stage order by lv_stage desc");
	if ( mysql2csv(fp, szHeader, szSql, pDbm) )
	{
		printf("data query or write error\n");
		return -1;
	}

	sprintf(szHeader, "\n\n等级排名（前50）\n角色,帐号,等级\n");
	sprintf(szSql,
			"select name, account, level from hddb.user \
			where (account between 'test001001' and 'test005000') \
			or (account between 'test002001' and 'test002434') \
			or (account between 'test003703' and 'test003811') \
			order by level desc limit 50");
	if ( mysql2csv(fp, szHeader, szSql, pDbm) )
	{
		printf("data query or write error\n");
		return -1;
	}

	sprintf(szHeader, "\n\n金钱排名（前50）\n角色,帐号,金钱\n");
	sprintf(szSql,
			"select name, account, money from hddb.user \
			where (account between 'test001001' and 'test005000') \
			or (account between 'test002001' and 'test002434') \
			or (account between 'test003703' and 'test003811') \
			order by money desc limit 50;");

	if ( mysql2csv(fp, szHeader, szSql, pDbm) )
	{
		printf("data query or write error\n");
		return -1;
	}

    fclose(fp);
	fp = NULL;

	mysql_close(pDbm);

	//getchar();
	return 0;
}
