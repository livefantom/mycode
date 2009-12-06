#include <stdio.h>
#include <string.h>

int main()
{
// 结论：strtok会将原字符串的分割符改变为'\0'，因此不能反复处理同一个串。
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
