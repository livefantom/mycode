#include <time.h>
int main()
{
	struct tm stmCallTime;
	strptime("2009-08-30 19:03:17", "%Y-%m-%d %T", &stmCallTime); //"yyyy-mm-dd hh24:mi:ss"
	printf("%d-%d-%d %d:%d:%d\n", stmCallTime.tm_year+1900, stmCallTime.tm_mon+1, stmCallTime.tm_mday, stmCallTime.tm_hour, stmCallTime.tm_min, stmCallTime.tm_sec);
	time_t nChargeTime = mktime(&stmCallTime);
	printf("%d\n", nChargeTime);
	return 0;

}
