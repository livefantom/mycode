#include "SysConfig.h"

using namespace std;

SysConfig::SysConfig(void)
{
}

SysConfig::~SysConfig(void)
{
}

int SysConfig::initialize()
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;

    nResult = S_SUCCESS;
    return nResult;
}


int SysConfig::release()
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;

    this->m_confData.clear();

    nResult = S_SUCCESS;
    return nResult;
}

int SysConfig::ltrim(char *buffer)
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;

    size_t i = 0;

    ASSERT(buffer != NULL);
    char * pcBuf = buffer;

    for (i = 0;i < strlen(pcBuf);i ++)
    {
        if (pcBuf[i] != '\t' && pcBuf[i] != ' ')
        {
            strcpy(buffer, pcBuf + i);
            buffer[strlen(pcBuf)] = 0;
            break;
        }
    }

    nResult = S_SUCCESS;
    return nResult;
}

int SysConfig::rtrim(char *buffer)
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;

    size_t i = 0;

    ASSERT(buffer != NULL);

    char* pcBuf = buffer;
    for (i = strlen(pcBuf) - 1;i >= 0;i --)
    {
        if (pcBuf[i] != '\t' && pcBuf[i] != ' ')
        {
            buffer[i + 1] = 0;
            break;
        }
    }

    nResult = S_SUCCESS;
    return nResult;
}

int SysConfig::load(const char *pcConfigFileName, int *pnLineParsed)
{
    int    nResult  = E_ERROR;
    int    nRetCode = E_ERROR;

    char   pcBuffer[1024];
    char   pcBuf[512];
    FILE  *pFile = 0;
    size_t i = 0;

    ASSERT(pcConfigFileName != NULL);
    ASSERT(pnLineParsed     != NULL);
    *pnLineParsed = 0;

    pFile = fopen(pcConfigFileName, "r");
    if (pFile == NULL)
    {
        goto ExitError;
    }

    while (!feof(pFile))
    {
        memset(pcBuffer, 0, 1024);
        fgets(pcBuffer, sizeof(pcBuffer), pFile);

        for (i = 0; i < strlen(pcBuffer); i++)
        {
            if (
                (pcBuffer[i] == '\r') ||
                (pcBuffer[i] == '\n') ||
                (pcBuffer[i] == '#')
            )
            {
                pcBuffer[i] = 0;
                break;
            }
        }
        ltrim(pcBuffer);
        rtrim(pcBuffer);

        if (
            (*pcBuffer == '\0') ||
            (*pcBuffer == '#')  ||
            (*pcBuffer == '\r') ||
            (*pcBuffer == '\n')
        )
        {
            continue;
        }
        char *ca = 0;
        while (ca = strchr(pcBuffer, '\t'))
        {
            pcBuffer[ca - pcBuffer] = ' ';
        }

        string strKeyName = "", strKeyValue = "";

        if (ca = strchr(pcBuffer, '='))
        {
            *ca = 0;

            strcpy(pcBuf, pcBuffer);
            ltrim(pcBuf);
            rtrim(pcBuf);
            strKeyName = pcBuf;
            ca++;
            while (*ca == ' ')
            {
                ca++;
            }

            //È¥µôÌØÊâ·ûºÅ¡£
            strcpy(pcBuf, ca);
            ltrim(pcBuf);
            rtrim(pcBuf);
            strKeyValue = pcBuf;
        }
        else
        {
            strKeyName = pcBuffer;
        }
        this->m_confData[strKeyName] = strKeyValue;
        ++(*pnLineParsed);
    }
    fclose(pFile);

    nResult = S_SUCCESS;
ExitError:

    return nResult;
}


int SysConfig::getIntVal(const char *pcKey, int *pnIntValue, int32_t nDefaultValue) const
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;

    ASSERT(pcKey != NULL);
    ASSERT(pnIntValue != NULL);

    KEYVALUE::const_iterator it = this->m_confData.find(pcKey);
    if (it != this->m_confData.end())
    {
        *pnIntValue = atoi((it->second).c_str());
        goto ExitOK;
    }
    *pnIntValue = nDefaultValue;
ExitOK:
    nResult = S_SUCCESS;
//ExitError:
    return nResult;

}

int SysConfig::getStringVal(const char *pcKey, char *pcStringValue, const char* pcDefaultValue /* = */ ) const
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;

    ASSERT(pcKey != NULL);
    ASSERT(pcStringValue != NULL);
    ASSERT(pcDefaultValue != NULL);

    KEYVALUE::const_iterator it = this->m_confData.find(pcKey);
    if (it != this->m_confData.end())
    {
        //*pnIntValue = atoi((it->second).c_str());
        strcpy(pcStringValue, (it->second).c_str());
        pcStringValue[(it->second).length()] = '\0';
        goto ExitOK;
    }
    strcpy(pcStringValue, pcDefaultValue);
ExitOK:
    nResult = S_SUCCESS;
    //ExitError:
    return nResult;

}

int SysConfig::getItemCount(size_t *pstItemCount) const
{
    int nResult = E_ERROR;
    int nRetCode = E_ERROR;
  
    ASSERT(pstItemCount != NULL);
    *pstItemCount = this->m_confData.size();

    nResult = S_SUCCESS;
    return nResult;

}


