#include "hdr.h"

#include <map>
using namespace std;
typedef map<int, Man> ManMap;
typedef ManMap::iterator ManIter;
typedef pair<int, Man> ManPair;


void* create_shm(key_t key, int size, int flg);

int main()
{
	int i;
	char buf[2] = {0};
	printf("%d\n", sizeof(key_t));

	ManMap* pMap;
	if (-1 == (int)(pMap = (ManMap*)create_shm(SHM_KEY, 1024*1024, 0660)) )
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
	
	// int shmdt(const void* shmaddr);
	if (-1 == shmdt(pMap))
	{
		perror("detach error");
		return -1;
	}
	return 1;
}

void* create_shm(key_t key, int size, int mask)
{
	int shm_id = -1;
	void* shm_addr = (void*)-1;

	// create shm file.
	if ( -1 == (shm_id = shmget(key, size, mask | IPC_CREAT | IPC_EXCL))
		&& EEXIST == errno )		// file already exists, read it!
		shm_id = shmget(key, 0, mask);

	// attach shm to local address space!
	if (shm_id != -1)
		shm_addr = shmat(shm_id, NULL, 0);

	return shm_addr;
}



