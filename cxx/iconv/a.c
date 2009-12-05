#include <iconv.h>
#include <errno.h>
#include <stdio.h>
#define OUTLEN 255
#define BUF_SIZE 1024
main()
{
	char *in_utf8 = "姝ｅ?ㄥ??瑁?";
	char *in_gb2312 = "正在安装";
	char out[OUTLEN];
	int rc = 0;
/*
	iconv_t cd = iconv_open("utf-8", "gb2312");
	if (-1 == cd)
	{
		printf("iconv_open error:%d\n", errno);
		return -1;
	}
	char* pIn = in_gb2312;
	char* pOut = out;
	int nInLeft = strlen(pIn);
	printf("nInLeft=%d\n", nInLeft);
	int nOutLeft = OUTLEN;
	int nLen = iconv(cd, &in_gb2312, &nInLeft, &pOut, &nOutLeft);
	if (-1 == nLen)
	{
		perror("iconv error:");
		printf("iconv error:%d\n", errno);
	}
	printf("%d chars converted!\n", nLen);
	iconv_close(cd);
*/
	char szInBuf[BUF_SIZE+1] = "";
	FILE* fp = fopen("bbb.csv", "r");
	if (NULL == fp)
	{
		printf("open file error\n");
		return -1;
	}
	if ( fread(szInBuf, BUF_SIZE, 1, fp) < 0 )
	{
		printf("read file error\n");
		return -1;
	}
	szInBuf[BUF_SIZE] = 0;
	int nInLeft = strlen(szInBuf);
	printf("%d\n", nInLeft = strlen(szInBuf) );
	printf("%s\n", szInBuf);
	fclose(fp);
	fp = NULL;

	//rc = u2g(szInBuf, nInLeft, out, OUTLEN);
	rc = code_convert("utf-8","gb2312", szInBuf, nInLeft, out, OUTLEN);
	printf("unicode-->gb2312 out=%s\n",out);

	//unicode码转为gb2312码
	rc = u2g(in_utf8,strlen(in_utf8),out,OUTLEN);
	printf("unicode-->gb2312 out=%s\n",out);
	//gb2312码转为unicode码
	rc = g2u(in_gb2312,strlen(in_gb2312),out,OUTLEN);
	printf("gb2312-->unicode out=%s\n",out);

}
//代码转换:从一种编码转为另一种编码
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	//if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
	if (iconv(cd, &inbuf, &inlen, &outbuf, &outlen)==-1) return -1;
	iconv_close(cd);
	return 0;
}
//UNICODE码转为GB2312码
int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
	return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
}
//GB2312码转为UNICODE码
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}

