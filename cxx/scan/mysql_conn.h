#ifndef _MYSQL_CONN
#define _MYSQL_CONN

#include <map>
#include <vector>
#include <string>

class MysqlConfig
{
public:
    //MysqlConfig(const std::string host, const std::string user, const std::string passwd, const std::string db_name, int port)
    //    : _host(host), _user(user), _passwd(passwd), _db_name(db_name), _port(port) {}
    MysqlConfig(void);
    ~MysqlConfig(void);

    std::string toString() const ;
    std::string host() const { return _host; }
    std::string user() const { return _user; }
    std::string passwd() const { return _passwd; }
    std::string db_name() const { return _db_name; }
    int port() const { return _port; }

    std::string _host;
    std::string _user;
    std::string _passwd;
    std::string _db_name;
    int _port;
};


class MysqlConnection
{
public:
    typedef std::map<std::string, std::string> Row;
    typedef std::vector<Row> ResultSet;

    //MysqlConnection(const std::string& host, const std::string& user, const std::string& passwd, const std::string& database, int port = 3306);
    MysqlConnection(void);
    ~MysqlConnection(void);

    //int connect();
    int connect(const MysqlConfig& cfg);
    int set_names(std::string charset);
    void disconnect();
    int list_tables(std::vector<std::string>& tables);
	int query(std::string sql, ResultSet& rs);
    int update(std::string sql);

    int last_errno() { return _errno; }
    std::string last_error() { return _error; };

private:
    //std::string _host;
    //int _port;
    //std::string _user;
    //std::string _passwd;
    //std::string _database;

    MYSQL _client;
    int _errno;
    std::string _error;
    
};




#endif//(_MYSQL_CONN)

