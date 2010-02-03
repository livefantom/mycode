#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <iconv.h>
#include <errno.h>

#define BUF_SIZE 8*1024

int code_convert(const char* dstcode, char* dstbuf, size_t* dstlen, const char* srccode, char* srcbuf, size_t* srclen)
{
	iconv_t cd = iconv_open(dstcode, srccode);
	if ( (iconv_t)-1 == cd )
		return -1;

	if ( -1 == iconv(cd, &srcbuf, srclen, &dstbuf, dstlen) )
		return -1;
	
	if ( -1 == iconv_close(cd) )
		return -1;
	
	return 0;	
}

int main(int argc, char** argv)
{
	int		nRetCode = -1;
	char* 	szInBuf = NULL;
	char 	szOutBuf[BUF_SIZE] = "";
	int 	nOutLeft = BUF_SIZE;
	struct stat filestat;
	uint32_t filesize;
	FILE* 	fp = NULL;
	printf("sizeof(size_t)=%d\n", sizeof(size_t) );
	// allocate buffer.
	// int stat(const char* filename, struct stat *buf);
	stat("bbb.csv", &filestat);
	filesize = filestat.st_size;
	printf("file size=%d\n", filesize);
	szInBuf = (char*)malloc(filesize + 1);
	// load data
	if ( NULL == (fp = fopen("bbb.csv", "r")) )
	{
		printf("open file error\n");
		nRetCode = -1;
		goto ExitError;
	}
	if ( fread(szInBuf, filesize, 1, fp) != 1 )
	{
		printf("read file error\n");
		nRetCode = -1;
		goto ExitError;
	}
	fclose(fp);
	fp = NULL;

	// convert code.
	printf("%s\n", szInBuf);
	if (-1 == code_convert("gbk", szOutBuf, &nOutLeft, "utf8", szInBuf, &filesize) )
	{
		perror("code convert error:");
		nRetCode = -1;
		goto ExitError;
	}
	printf("%s\n", szOutBuf);
	
	nRetCode = 0;
ExitError:
	free(szInBuf);
	szInBuf = NULL;
	return nRetCode;
}

