#include <string.h>
#include <stdio.h>

int main()
{
//	结论：strcpy复制时会复制'\0',strncpy如果长度范围内没有'\0',则不会添加'\0'至末尾。
	char szBuf[255]  = "aaaaa";
	printf( "%d\n", strlen(szBuf) );
	strncpy(szBuf, "aaa", 3);
	printf( "%d\n", strlen(szBuf) );
	strcpy(szBuf, "aaa");
	printf( "%d\n", strlen(szBuf) );
	strcpy(szBuf, 0);// this will crash!
	printf( "%d\n", strlen(szBuf) );
	
}
