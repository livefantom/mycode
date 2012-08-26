
#include "util.h"
#include "schema.h"

using namespace std;

int load(Schema& out_s, const MysqlConfig& db_cfg, vector<string> tables)
{
	MysqlConnection conn;
	int ret = conn.connect(db_cfg);
	if (ret != 0)
	{
		cout << "connect to " << db_cfg._host << ":" << db_cfg._port << " " << db_cfg._db_name << " error:" << conn.last_error() << endl;
		return -1;
	}

	conn.set_names("latin1");

	if( tables.empty() )
	{
		conn.list_tables(tables);
	}

	for (unsigned int i = 0; i < tables.size(); ++i)
	{
		MysqlConnection::ResultSet rs;
		string sql = "show fields from " + tables[i] + ";";
		ret = conn.query(sql, rs);
		//cout << sql << ", ret:" << ret << endl;
		if (ret > 0)
		{
			Table tb(tables[i], rs);
			bool r = out_s.append(tb);
			if (!r)
			{
				cout << "append table error!" << endl;
			}
		}
	}

	conn.disconnect();
	return 0;
}

int load(Schema& out_s, const MysqlConfig& db_cfg)
{
	vector<string> tables;
	return load(out_s, db_cfg, tables);
}

int compare(const Schema& tpl_s, const string& pd_name, const string& env)
{
	int ret = -1;
	do
	{
		MysqlConfig db_cfg;
		ret = get_db_cfg(db_cfg, pd_name, env);
		if (ret != 0)
		{
			cout << "get src db config failed!" << endl;
			break;
		}

		Schema src_s(pd_name);
		ret = load( src_s, db_cfg, tpl_s.table_list() );
		if (ret != 0)
		{
			cout << "load src db schema failed!" << endl;
			break;
		}
		//cout << src_s.toString();

		bool res = src_s.as(tpl_s);
		//cout << "--------as return:" << res << endl;
		if (res)
		{
			ret = 0;
		}
		else
		{
			ret = -1;
		}
	} while(false);

	return ret;
}

int compare(const string& dst_pd, const string& dst_env, const string& src_pd, const string& src_env)
{
	int ret = -1;
	do
	{
		MysqlConfig dst_cfg;
		ret = get_db_cfg(dst_cfg, dst_pd, dst_env);
		if (ret != 0)
		{
			cout << "get dst db config failed!" << endl;
			break;
		}

		Schema dst_s(dst_pd);
		ret = load(dst_s, dst_cfg);
		if (ret != 0)
		{
			cout << "load dst db schema failed!" << endl;
			break;
		}
		//cout << dst_s.toString();

		ret = compare(dst_s, src_pd, src_env);
	} while(false);

	return ret;
}

int main(int argc, char** argv)
{
	string env = "test";
	if (argc > 1)
	{
		env = argv[1];
	}
	cout << "=========================<<<<" << env << ">>>>===========================" << endl;

	string tpl_pd = "Template";
	string tpl_env = "real";

	MysqlConfig tpl_cfg;
	int ret = get_db_cfg(tpl_cfg, tpl_pd, tpl_env);
	if (ret != 0)
	{
		cout << "get tpl db config failed!" << endl;
		return -1;
	}

	Schema tpl_s(tpl_pd);
	ret = load(tpl_s, tpl_cfg);
	if (ret != 0)
	{
		cout << "load tpl db schema failed!" << endl;
		return -1;
	}
	//cout << tpl_s.tables();
	cout << "--------------------------------------" << endl;
	tpl_s.fill_tpl("_99", "_201208");

	try
	{
		OssBase::UnixConfig cfg(CFG_FILE);
		unsigned int num = StringToInt( cfg["PdList"]("num") );
		for (unsigned int i = 1; i <= num ; ++i)
		{
			string pd_name = cfg["PdList"]( IntToString(i) );
			cout << "=============================="<< pd_name <<"===============================" << endl;
			if ( 0 == compare(tpl_s, pd_name, env) )
			{
				cout << "Great! Noting is different!" << endl;
			}
		}
	}
	catch (OssBase::OssException e)
	{
		cout << e.what() << endl;
	}
	return 0;
}


