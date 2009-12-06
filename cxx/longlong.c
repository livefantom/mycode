#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>
int main(int argc, char** argv)
{
	uint64_t ullTest = 0x1FFFFFFFFFFFFFFFL;
	long double* pTest = &ullTest;
	printf("%0llf\n", *pTest);
	printf("%0llX", *pTest);

}
