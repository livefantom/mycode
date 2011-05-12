#include "logd.h"

int main()
{
	int shm_id, i;
	struct shmem_ds* pData = 0;
	EnterMsg* pMsg = 0;
	shm_id = shmget(ENTER_KEY, 0, IPC_CREAT);
	if ( -1 == shm_id )
	{
		perror("shmget error");
		return -1;
	}
	printf("shm_id:%d\n", shm_id);
	pData = (struct shmem_ds*)shmat(shm_id, NULL, 0);
	if (-1 == (int)pData)
	{
		perror("shmat error");
		return -1;
	}
	printf("pData:%u, maxsz:%d, head:%u, tail:%u\n", pData, pData->maxsz, pData->head, pData->tail);
	for (i = 0; i < pData->maxsz; ++i)
	{
		print_mem( &pData->data + i*sizeof(EnterMsg), sizeof(EnterMsg) );
		printf("\n-----------------------------------------------------\n");
		getchar();
	}

	if ( -1 == shmdt(pData) )
		perror("detach error");
}
