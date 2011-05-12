#include <stdlib.h>

int main(void)
{
	const char *name = "TNS_ADMIN";
	char *value = getenv(name);
	printf("tnsnames.ora path = %s/tnsnames.ora", value);
	return 0;
}
