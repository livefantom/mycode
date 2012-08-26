#ifndef _SCHEMA_H
#define _SCHEMA_H

#include <string>
#include <map>
#include <iostream>
#include <vector>

class Column
{
public:
    typedef std::map<std::string, std::string> COL_INFO;

    //Column( const std::string& name, const std::string& type, const std::string& dft ) : _name(name), _type(type), _default(dft) {};
	Column( const COL_INFO& col_info );
    ~Column(void);

    bool operator ==( const Column& other ) const;
    std::string diff( const Column& other ) const;
    std::string toString() const;
	std::string name() const { return _name; }
 //   std::string type() const { return _type; }
 //   std::string def_val() const { return _default; } 

private:
    std::string _name;
    std::string _type;
    std::string _default;

};


class Table
{
public:
    typedef std::vector<Column::COL_INFO> TABLE_INFO;

    Table( const std::string name, const TABLE_INFO& tb_info );
    ~Table(void);

    bool as( const Table& other ) const;
    std::string toString() const;
	std::string name() const { return _name; }
    void set_name(const std::string& name) { _name = name; }

private:
    typedef std::map<std::string, Column> COL_LIST;
    COL_LIST _col_list;
    std::string _name;

};


class Schema
{
public:
	Schema(const std::string name) : _name(name) {}
	~Schema();

    bool as( const Schema& other ) const;
    bool append( Table tb );
    std::string toString() const;
	std::string name() const { return _name; }
    std::vector<std::string> table_list() const;
    std::string tables() const;
    int fill_tpl(const std::string& qq, const std::string& yyyymm);

private:
    typedef std::map<std::string, Table> TB_LIST;
    TB_LIST _tb_list;
    std::string _name;
};




#endif//(_SCHEMA_H)
