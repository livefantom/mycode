#include <string.h>
#include <stdio.h>

int main()
{
//	���ۣ�strcpy����ʱ�Ḵ��'\0',strncpy������ȷ�Χ��û��'\0',�򲻻����'\0'��ĩβ��
	char szBuf[255]  = "aaaaa";
	printf( "%d\n", strlen(szBuf) );
	strncpy(szBuf, "aaa", 3);
	printf( "%d\n", strlen(szBuf) );
	strcpy(szBuf, "aaa");
	printf( "%d\n", strlen(szBuf) );
	strcpy(szBuf, 0);// this will crash!
	printf( "%d\n", strlen(szBuf) );
	
}
