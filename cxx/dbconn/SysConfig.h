#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__
#include "Macro.h"
#include <map>
#include <string>

class Engine_Export SysConfig
{
	typedef std::map<std::string, std::string> KEYVALUE;
public:
    SysConfig(void);
    virtual int initialize();
    int load(const char* pcConfigFileName, int32_t *pnLineParsed);
    virtual int release();
public:
    virtual ~SysConfig(void);

    int getIntVal(const char *pcKey, int32_t *pnIntValue, int32_t nDefaultValue = 0) const;
    int getStringVal(const char *pcKey, char *pcStringValue, const char* pcDefaultValue = "") const;

    int getItemCount(size_t *pstItemCount) const;

private:
    char m_pcConfigFileName[FILE_MAX_PATH];
    KEYVALUE m_confData;
    int ltrim(char *buffer);
    int rtrim(char *buffer);
};

#endif
