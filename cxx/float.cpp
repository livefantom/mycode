#include <stdio.h>
#include <string.h>

int main()
{

	int op = 1;
	printf("op=0x%x\n", op);
	float f = 1.0f;
	int ff = 0;
	memcpy(&ff, &f, sizeof(float));
	printf("(int)f=%d, f=%f, ff=%d\n", f, f, ff);
	printf("f=%f, ff=%d\n", f, ff);
	for(int j=0; j<32; j++)
	{
		if (0 == j%8)
		{
			printf(" ");
		}
		printf("%d",( ff & (op << j) )>0 );
	}
	printf("\n");
	return 0;
}
