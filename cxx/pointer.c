int main()
{
	char a[100] = "a";
	char* p = a;
	char** pp = &p;
	printf("%d, %d, %d, %d, %d, %d", (int)a, (int)&a, (int)p, (int)&p, (int)pp, (int)*pp);
	return 0;
}
