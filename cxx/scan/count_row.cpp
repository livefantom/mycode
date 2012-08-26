// count all table's rowcount .
#include "util.h"

using namespace std;

int count_row(const MysqlConfig& db_cfg)
{
	MysqlConnection conn;
	int ret = conn.connect(db_cfg);
	if (ret != 0)
	{
		cout << "connect to " << db_cfg._host << ":" << db_cfg._port << " failed!" << endl;
		return -1;
	}

	vector<string> tables;
	conn.list_tables(tables);
	MysqlConnection::ResultSet rs;
	string sql = "";
	int cnt = 0;
	for (unsigned int i = 0; i < tables.size(); ++i)
	{
		//cout << tables[i] << endl;
		sql = "select count(*) as row_cnt from " + tables[i] + ";";
		ret = conn.query(sql, rs);
		//cout << sql << ", ret:" << ret << endl;
		if ( ret > 0)
		{
			for (unsigned int j = 0; j < rs.size(); ++j)
			{
				int row_cnt = StringToInt(rs[j]["row_cnt"]);
				//cout << db_cfg._db_name << "." << tables[i] << ": " << row_cnt << endl;
				if ( row_cnt > 0)
				{
					++ cnt;
					cout << db_cfg._db_name << "." << tables[i] << ": " << row_cnt << endl;
				}
			}
		}
	}
	return cnt;
}

int main(int argc, char** argv)
{
	string env = "test";
	if (argc > 1)
	{
		env = argv[1];
	}
	cout << "env:" << env << endl;
    try
    {
        OssBase::UnixConfig cfg(CFG_FILE);
        unsigned int num = StringToInt(cfg["PdList"]("num"));
		for (unsigned int i = 0; i < num; ++i)
		{
			string pd_name = cfg["PdList"](IntToString(i+1));
			MysqlConfig db_cfg;
			int ret = get_db_cfg(db_cfg, pd_name, env);
			if (ret != 0)
			{
				cout << "get db config failed!" << endl;
				break;
			}
			
			cout << "------------- begin " << pd_name << ", host:"  << db_cfg._host << ", port:" << db_cfg._port
				<< ", db_name:" << db_cfg._db_name << endl;
			int cnt = count_row(db_cfg);
			if(cnt > 0)
			{
				cout << "------------- end " << pd_name << ", [" << cnt << "] tables have data in total." << endl;
			}
			else if(0 == cnt)
			{
				cout << "------------- end " << pd_name << ", NO_DATA_FOUND." << endl;
			}
			else
			{
				cout << "------------- end " << pd_name << ", read database data failed." << endl;
			}
		}
    }
    catch (OssBase::OssException e)
    {
        cout << e.what() << endl;
    }

    return 0;
}

