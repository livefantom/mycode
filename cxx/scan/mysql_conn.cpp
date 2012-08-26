#include <sstream>
#include <mysql.h>
#include "mysql_conn.h"

MysqlConfig::MysqlConfig(void)
{
}

MysqlConfig::~MysqlConfig(void)
{
}

std::string MysqlConfig::toString() const
{
    std::ostringstream oss;
    //oss << "db_name:" << _db_name << ", host:" << _host << ", port:" << _port << ", passwd:" << _passwd << ", user:" << _user;
    oss << "db_name:" << _db_name << ", host:" << _host << ", port:" << _port << ", user:" << _user;
    return oss.str();
}

/*
MysqlConnection::MysqlConnection(const std::string& host, const std::string& user, const std::string& passwd, const std::string& database, int port)
    : _host(host)
    , _port(port)
    , _user(user)
    , _passwd(passwd)
    , _database(database)
{
    if ( 0 == mysql_init(&_client) )
    {
        _errno = mysql_errno(&_client);
        _error = mysql_error(&_client);
    }
}
*/

MysqlConnection::MysqlConnection(void)
{
    if ( 0 == mysql_init(&_client) )
    {
        _errno = mysql_errno(&_client);
        _error = mysql_error(&_client);
    }
}

MysqlConnection::~MysqlConnection(void)
{
}

/*
int MysqlConnection::connect()
{
    if ( 0 == mysql_real_connect(&_client, _host.c_str(), _user.c_str(), _passwd.c_str(),
        _database.c_str(), _port, NULL, CLIENT_FOUND_ROWS) )
    {
        _errno = mysql_errno(&_client);
        _error = mysql_error(&_client);
        return -1;
    }
    return 0;
}
*/

int MysqlConnection::connect(const MysqlConfig& cfg)
{
    if ( 0 == mysql_real_connect(&_client, cfg.host().c_str(), cfg.user().c_str(), cfg.passwd().c_str(),
        cfg.db_name().c_str(), cfg.port(), NULL, CLIENT_FOUND_ROWS) )
    {
        _errno = mysql_errno(&_client);
        _error = mysql_error(&_client);
        return -1;
    }
    return 0;
}

int MysqlConnection::set_names(const std::string& charset)
{
    //if (!mysql_set_character_set(&_client, "latin1")) //4.1.13
    //{
    //    printf("New client character set: %s\n",
    //           mysql_character_set_name(&_client));
    //}
    return this->update("set names " + charset);
}

int MysqlConnection::query(const std::string& sql, ResultSet& rs)
{
    if ( mysql_query(&_client, sql.c_str()) != 0) 
    {
        _errno = mysql_errno(&_client);
        _error = mysql_error(&_client);
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(&_client);
    if (0 == res)
    {
        if( mysql_errno(&_client) != 0 )
        {
            _errno = mysql_errno(&_client);
            _error = mysql_error(&_client);
            return -1;
        }
        else
        {
            mysql_free_result(res);
            return 0;
        }
    }

    int num_rows = mysql_num_rows(res);
    int num_fields = mysql_num_fields(res);
    if (  num_rows == 0 || num_fields == 0 )
    {
        mysql_free_result(res);
        return 0;
    }
 
    rs.clear();
    rs.resize(num_rows);
 
    MYSQL_FIELD* fields = mysql_fetch_fields(res);
 
    if (0 == fields)
    {
        _errno = mysql_errno(&_client);
        _error = mysql_error(&_client);
        return -1;
    }
 
    MYSQL_ROW row;
    int rowindex = 0;
    while ( (row = mysql_fetch_row(res)) )
    {
        Row& maprow = rs[rowindex++];
        for(int i=0; i < num_fields; i++)
        {
            maprow[fields[i].name] = row[i] ? row[i] : "";
        }
    }
    mysql_free_result(res);
 
    return num_rows;
}

int MysqlConnection::update(const std::string& sql)
{
    int updateflag=0;
    if ( sql.find("delete") != std::string::npos ) updateflag=1;
    else if ( sql.find("update") != std::string::npos ) updateflag=1;
    else if ( sql.find("replace") != std::string::npos ) updateflag=1;
    else if ( sql.find("insert") != std::string::npos ) updateflag=2;
    else if ( sql.find("DELETE") != std::string::npos ) updateflag=1;
    else if ( sql.find("UPDATE") != std::string::npos ) updateflag=1;
    else if ( sql.find("REPLACE") != std::string::npos ) updateflag=1;
    else if ( sql.find("INSERT") != std::string::npos ) updateflag=2;
 
    if (mysql_query(&_client, sql.c_str()) != 0)
    {
        _errno = mysql_errno(&_client);
        _error = mysql_error(&_client);
        return -1;
    }
    if ( updateflag == 1 )
    {
        return mysql_affected_rows(&_client);
    }
    else if ( updateflag == 2 )
    {
        return  mysql_insert_id(&_client);
    }

    return 0;
}

int MysqlConnection::list_tables(std::vector<std::string>& tables)
{
    MYSQL_RES* res = mysql_list_tables(&_client, 0);
    if (0 == res)
    {
        if( mysql_errno(&_client) != 0 )
        {
            _errno = mysql_errno(&_client);
            _error = mysql_error(&_client);
            return -1;
        }
        else
        {
            mysql_free_result(res);
            return 0;
        }
    }

    int num_rows = mysql_num_rows(res);
    int num_fields = mysql_num_fields(res);
    if (  num_rows == 0 || num_fields == 0 )
    {
        mysql_free_result(res);
        return 0;
    }
 
    tables.clear();
    tables.resize(num_rows);
 
    MYSQL_FIELD* fields = mysql_fetch_fields(res);
 
    if (0 == fields)
    {
        _errno = mysql_errno(&_client);
        _error = mysql_error(&_client);
        return -1;
    }
 
    MYSQL_ROW row;
    int rowindex = 0;
    while ( (row = mysql_fetch_row(res)) )
    {
        tables[rowindex++] = row[0] ? row[0] : "";
    }
    mysql_free_result(res);
 
    return num_rows;
}

void MysqlConnection::disconnect()
{
    mysql_close(&_client);
}

