int main()
{
	int** pp = 0;
	int n = 10;
	int* p = &n;
	*pp = p;
	printf("pp=%d, \n", pp);
	printf(" *pp=%d, \n",  *pp);
	printf("**pp=%d\n",  **pp);
	return 0;
}

