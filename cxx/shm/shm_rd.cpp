#include "hdr.h"

int main()
{
	int shm_id, i;
	people* pMap = 0;
	shm_id = shmget(SHM_KEY, 0, IPC_CREAT);
	if ( -1 == shm_id )
	{
		perror("shmget error");
		return -1;
	}
	printf("shm_id:%d\n", shm_id);
	pMap = (people*)shmat(shm_id, NULL, 0);
	if (-1 == (int)pMap)
	{
		perror("shmat error");
		return -1;
	}
	printf("pMap:%d\n", pMap);
	for (i =0; i<10; i++)
	{
		printf("idx:%d,\tname:%s,\tage:%d\n", i, (pMap+i)->name, (pMap+i)->age  );
	}
	if ( -1 == shmdt(pMap) )
		perror("detach error");
}
