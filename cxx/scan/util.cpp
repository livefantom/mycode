#include "util.h"

using namespace std;

int get_db_cfg(MysqlConfig& db_cfg, const string& pd_name, const string& env)
{
	// construct config file path and config node name.
	string cfg_file = DB_CFG_FILE;
	string cfg_node = "db_" + pd_name;
	if (env == "test" || env == "waibao")
	{
		cfg_file = CFG_FILE;
		cfg_node = "db_" + env;
	}
	
	// special for Template.
	if (pd_name == "Template")
	{
		cfg_file = CFG_FILE;
	}

	// read config from file.
	try
	{
		OssBase::UnixConfig cfg(cfg_file);
		db_cfg._host = cfg[cfg_node]("host");
		db_cfg._user = cfg[cfg_node]("user");
		db_cfg._passwd = cfg[cfg_node]("passwd");
		db_cfg._port = StringToInt( cfg[cfg_node]("port") );
		if (env == "test" || env == "waibao")
		{
			db_cfg._db_name = "db" + pd_name + "Market";
		}
		else
		{
			db_cfg._db_name = cfg[cfg_node]("database");
		}
	}
    catch (OssBase::OssException e)
    {
        cout << e.what() << endl;
        return -1;
    }
	return 0;
}
