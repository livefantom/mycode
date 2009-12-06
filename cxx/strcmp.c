
int main()
{
	const char* cp = "hello";
	if (0==strcmp(cp, 0))// this will crash!
	{
		printf("incredible!\n");
	}
	return 0;
}
