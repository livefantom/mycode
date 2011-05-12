#include <stdio.h>
#include <inttypes.h>
#include <time.h>

typedef struct _data
{
	uint32_t state;
	uint32_t len;
	uint32_t type;
	char data[32];
} D;

int func(int n)
{
	D d;
//	d.len = n;
//	d.type = time(0)+n;
//	printf("%u\n%d\n%d\n%s\n", d.state, d.len, d.type, d.data);
	printf("%u\n%d\n%d\n%s\n", d.state, d.len, d.type, d.data);
	return 0;
}

int main()
{
	for(int i=10; i>0; --i)
	{
		func(i);
	}
	return 0;
}
