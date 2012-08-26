#ifndef __UTIL_H
#define __UTIL_H

#include <web_all.h>
#include "mysql_conn.h"

#define DB_CFG_FILE ("/usr/local/gameshopwebadmin/cfg/market/market.conf")
#define CFG_FILE ("./scan.conf")


#define StringToInt OssBase::TypeTransform::StringToInt
#define IntToString OssBase::TypeTransform::IntToString

int get_db_cfg(MysqlConfig& db_cfg, const string& pd_name, const string& env);



#endif//(__UTIL_H)

