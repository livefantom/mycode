#include <stdio.h>
#include <string.h>

int main()
{
// ���ۣ�strtok�Ὣԭ�ַ����ķָ���ı�Ϊ'\0'����˲��ܷ�������ͬһ������
	char str1[] = ";user_id=aaa;role_name=bbb;ip=192.168.1.1";
	printf("org=%s\n", str1);

	char* cp = strtok(str1, ";");
	printf("org=%s, %s\n", str1, cp);
	cp = strtok(str1, "=");
	printf("org=%s, %s\n", str1, cp);

	cp = strtok(0, "=");
	printf("org=%s, %s\n", str1, cp);
	return 0;
}
