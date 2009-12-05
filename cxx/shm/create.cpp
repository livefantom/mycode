#include "hdr.h"
#include <map>
using namespace std;
typedef map<int, Man> ManMap;
typedef ManMap::iterator ManIter;
typedef pair<int, Man> ManPair;


void* getShm(key_t key, int size, int flg);

int main()
{
	int i;
	char buf[2] = {0};
	printf("%d\n", sizeof(key_t));

#ifdef _USE_STRUCT
	people* pMap;
	if (-1 == (int)(pMap = (people*)getShm(SHM_KEY, 4096, 0660)) )
	{
		return -1;
	}
	for (i=0; i<10; i++)
	{
		printf("i=%d, name=%s, age=%d\n", i, (pMap+i)->name, (pMap+i)->age );
		buf[0] = 'a' + i;
		memset( (pMap+i)->name, 0, 4);
		strncpy( (pMap+i)->name, &buf, 1);
		(pMap+i)->age = 20+i;
	}
#elif (defined _USE_CLASS)
	Man* pMap;
	if (-1 == (int)(pMap = (Man*)getShm(SHM_KEY, 4096, 0660)) )
	{
		return -1;
	}
	for (i=0; i<10; ++i)
	{
		printf("i=%d, name=%s, age=%d\n", i, (pMap+i)->getName(), (pMap+i)->getAge() );
		buf[0] = 'a' + i;
		(pMap+i)->setName(buf);
		(pMap+i)->setAge(20+i);
	}
#else
	ManMap* pMap;
	if (-1 == (int)(pMap = (ManMap*)getShm(SHM_KEY, 1024*1024, 0660)) )
	{
		return -1;
	}
	// add item
	ManMap map;
	Man m;
	for (i=0; i<10; ++i)
	{
		buf[0] = 'a' + i;
		m.setName(buf);
		m.setAge(20+i);
		map.insert( ManPair(i, m) );
	}
	memcpy(pMap, &map, sizeof(map));
	// show item
	for (i=0; i<10; ++i)
	{
		m = pMap->find(i)->second;
		printf("i=%d, name=%s, age=%d\n", i, m.getName(), m.getAge() );
	}
	
#endif
	// int shmdt(const void* shmaddr);
	if (-1 == shmdt(pMap))
	{
		perror("detach error");
		return -1;
	}
	return 1;
}

void* getShm(key_t key, int size, int flg)
{
	// int shmget(key_t key, int size, int shmflag);
	int shm_id = shmget(key, size, flg | IPC_CREAT | IPC_EXCL);
	if (-1 == shm_id)
	{
		printf("errno:%d\n", errno);
		if ( EEXIST == errno )
		{
			printf("File exists\n");
			shm_id = shmget(key, 0, flg);
			if (-1 == shm_id)
			{
				perror("shmget error");
			}
		}
		else
		{
			perror("shmget error");
		}
	}

	void* shm_addr = (void*)-1;
	if (shm_id != -1)
	{
		// void* shmat(int shmid, const void* shmaddr, int shmflag);
		shm_addr = shmat(shm_id, NULL, 0);
		if (-1 == (int)shm_addr)
		{
			perror("shmat error");
		}
	}
	return shm_addr;
}


