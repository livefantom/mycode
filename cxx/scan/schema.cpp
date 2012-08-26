#include "schema.h"

using namespace std;

/// class Column members.

Column::Column( const COL_INFO& col_info )
{
    _name = col_info.find("Field")->second;
    _type = col_info.find("Type")->second;
    _default = col_info.find("Default")->second;
}

Column::~Column(void)
{
}

bool Column::operator ==( const Column& other ) const
{
    return ( (_name == other._name) && (_type == other._type) && (_default == other._default) );
}

std::string Column::toString() const
{
    return "|" + _type + "|" + _default + "|";
}

std::string Column::diff( const Column& other ) const
{
    std::string str1 = "";
    std::string str2 = "";
    if (_type != other._type)
    {
        str1 += "|Type:" + _type;
        str2 += "|Type:" + other._type;
    }

    if (_default != other._default)
    {
        if ( _type == "text" || _type.find("varchar") != std::string::npos )
        {
            str1 += "|Default:'" + _default + "'";
        }
        else
        {
            str1 += "|Default:" + _default;
        }
 
        if ( other._type == "text" || other._type.find("varchar") != std::string::npos )
        {
            str2 += "|Default:'" + other._default + "'";
        }
        else
        {
            str2 += "|Default:" + other._default;
        }
    }
    str1 += "|";
    str2 += "|";

    return str1 + " should be " + str2;
}

/// class Table members.

Table::Table( const std::string name, const TABLE_INFO& tb_info )
{
	_name = name;
	for (unsigned int i = 0; i < tb_info.size(); ++i)
	{
		Column col(tb_info[i]);
		_col_list.insert( COL_LIST::value_type(col.name(), col) );
	}
}


Table::~Table(void)
{
}

bool Table::as( const Table& other ) const
{
   // cout << "begin Table::as() " << endl;
    bool ret = true;
    if (_name != other._name)
    {
        return false;
    }

    for ( COL_LIST::const_iterator o_it = other._col_list.begin(); o_it != other._col_list.end(); ++o_it )
    {
        COL_LIST::const_iterator it = _col_list.find( o_it->first );
        if ( it == _col_list.end() )
        {
            std::cout << _name << "." << o_it->first << " not exists.\n" ;
            ret = false;
        }
        else if( it->second == o_it->second )
        {
    //        std::cout << it->first << " is ok!\n" ;
            continue;
        }
        else
        {
            //std::cout << it->second.toString() << " should be:" << o_it->second.toString() << endl;
            std::string diff = it->second.diff(o_it->second);
            std::cout << _name << "." << o_it->first << " " << diff << endl;
            //it->second.diff(o_it->second);
            ret = false;
        }
    }
    //return true;
    return ret;
}

std::string Table::toString() const
{
    std::string outstr = "|" + _name + "|\n";
    for ( COL_LIST::const_iterator it = _col_list.begin(); it != _col_list.end(); ++it )
    {
        outstr += it->second.toString() + "\n";
    }
    return outstr;
}



/// class Schema members.

Schema::~Schema(void)
{
}

bool Schema::as( const Schema& other ) const
{
   // cout << "begin Schema::as() " << endl;

    bool ret = true;

    for ( TB_LIST::const_iterator o_it = other._tb_list.begin(); o_it != other._tb_list.end(); ++o_it )
    {
        TB_LIST::const_iterator it = _tb_list.find( o_it->first ); // search the same table name.
        if ( it == _tb_list.end() ) // find nothing.
        {
            std::cout << o_it->first << " not exists.\n" ;
            ret = false;
        }
        else if( it->second.as(o_it->second) ) // if similar.
        {
     //       std::cout << it->first << " is ok!\n" ;
            continue;
        }
        else
        {
     //       std::cout << it->first << " else ????!\n" ;
            ret = false;
            //std::cout << it->second.toString() << " should be:" << o_it->second.toString() ;
        }
    }

    return ret;
}

bool Schema::append( Table tb )
{
    std::pair<TB_LIST::iterator, bool> retval = _tb_list.insert( TB_LIST::value_type(tb.name(), tb) );
    return retval.second;
}

std::string Schema::toString() const
{
    std::string outstr = "|" + _name + "|\n";
    for ( TB_LIST::const_iterator it = _tb_list.begin(); it != _tb_list.end(); ++it )
    {
        outstr += it->second.toString() + "\n";
    }
    return outstr;
}

std::vector<std::string> Schema::table_list() const
{
    std::vector<std::string> list;
    for ( TB_LIST::const_iterator it = _tb_list.begin(); it != _tb_list.end(); ++it )
    {
        list.push_back(it->first);
    }
    return list;
}

std::string Schema::tables() const
{
    std::string out_str = "";
    for ( TB_LIST::const_iterator it = _tb_list.begin(); it != _tb_list.end(); ++it )
    {
        out_str += it->first + "\n";
    }
    return out_str;
}

int Schema::fill_tpl(const std::string& qq, const std::string& yyyymm)
{
    for ( TB_LIST::iterator it = _tb_list.begin(); it != _tb_list.end();  )
    {
        string name = it->first;
        size_t pos = name.find("_QQ");
        if ( pos != std::string::npos )
        {
            name.replace(pos, 3, qq);
        }

        pos = name.find("_YYYYMM");
        if ( pos != std::string::npos )
        {
            name.replace(pos, 7, yyyymm);
        }

        if (name != it->first)
        {
            it->second.set_name(name);
            std::pair<TB_LIST::iterator, bool> retval = _tb_list.insert( TB_LIST::value_type(it->second.name(), it->second) );
            if (retval.second)
            {
                _tb_list.erase(it++);
            }
            else
            {
                ++ it;
                cout << "insert error" << endl;
            }
        }
        else
        {
            ++ it;
        }

    }
    return 0;
}

