#include <string.h>
#include <stdio.h>

int xorEncode(const char* szSrc, const char* szKey, char* szHexDst)
{
    int nSrcLen = strlen(szSrc);
    int nKeyLen = strlen(szKey);
    int j = 0;
    unsigned char ch;
    for (int i = 0; i < nSrcLen; i++)
    {
        if (j == nKeyLen)
        {
            j = 0;
        }
        ch = szSrc[i] ^ szKey[j];
        sprintf(szHexDst + 2*i, "%02x", ch);
        j++;
    }
	
    return 1;
}

int main()
{
    char szSrc[255] = "abcde";
    char szKey[16] = "linekong.com";
    char szDst[255] = "";
    xorEncode(szSrc, szKey, szDst);
    printf("%s\n", szDst);

    strcpy(szSrc, "ฮารว");
    memset(szDst, 0, 255);
    xorEncode(szSrc, szKey, szDst);
    printf("%s\n", szDst);
    

}
