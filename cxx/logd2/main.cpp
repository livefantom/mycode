#include "logd.h"
#include "CircularQueue.h"

CircularQueue<EnterMsg>* gp_enter_shm;

int main()
{
	int svr_port = 17171;
	int sock_fd;
	fd_set fds;
	BYTE rcv_buf[BUFSIZ] = {0};
	int rcv_len = 0;

	int shm_size = SHM_SIZE;

	struct sockaddr_in svr_addr;
	bzero( &svr_addr, sizeof(svr_addr) );
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	svr_addr.sin_port = htons(svr_port);

	init_shmem(ENTER_KEY, shm_size, 0660);

	if ( -1 == (sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) )
	{
		perror("socket error");
		return -1;
	}

	if ( -1 == bind(sock_fd, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) )
	{
		perror("bind error");
		return -1;
	}

	while(1)
	{
		FD_ZERO(&fds);
		FD_SET(sock_fd, &fds);
		if ( -1 == select(sock_fd + 1, &fds, NULL, NULL, 0) )
		{
			if (EINTR == errno)
				continue;
			else
			{
				perror("select error:");
				break;
			}
		}
		if ( FD_ISSET(sock_fd, &fds) )
		{
			bzero(rcv_buf, BUFSIZ);
			rcv_len = recv(sock_fd, rcv_buf, BUFSIZ, 0);
			if (rcv_len <= 0)
			{
				perror("recv error:");
				continue;
			}
			else
				parse_and_log(rcv_buf, rcv_len);
		}
	}
	
//	free_shmem();
	return 0;
}

int free_shmem(int shm_id)
{
	// detach shm from local address space!
	shmdt(gp_enter_shm);
	// destroy shm segment.
	shmctl(shm_id, IPC_RMID, NULL);
}

int parse_and_log(const BYTE* msg, int len)
{
//	print_mem(msg, len);
	append_msg( (EnterMsg*)msg );
}

int init_shmem(key_t key, int size, int mask)
{
	int shm_id = -1;
	void* shm_addr = (void*)-1;
	bool shm_exist = false;
	struct shmid_ds shm_state;

	// create shm file.
	if ( -1 == (shm_id = shmget(key, size, mask | IPC_CREAT | IPC_EXCL))
		&& EEXIST == errno )
	{		// file already exists, read it!
		shm_exist = true;
		shm_id = shmget(key, 0, mask);
	}
	if (shm_id == -1)
	{
		perror("shmget error:");
		return -1;
	}

	// attach shm to local address space!
	if( -1 == (int)(shm_addr = shmat(shm_id, NULL, 0)) )
	{
		perror("shmat error:");
		return -1;
	}

	gp_enter_shm = (CircularQueue<EnterMsg>*)shm_addr;
	if ( !shm_exist )
	{
		// the OS init the shmval to 0 while create.
//		gp_enter_shm->maxsz = ( size - ((int)&(gp_enter_shm->data) - (int)gp_enter_shm) )
//						/ sizeof(EnterMsg);
		gp_enter_shm->resize(10);
	}
	else
	{
		// realloc shm when size-setting changed!
		if ( -1 == shmctl(shm_id, IPC_STAT, &shm_state) )
		{
			perror("shmctl error:");
			return -1;
		}
		else if ( shm_state.shm_segsz != size )
		{
/*			old_size = sizeof(ShMemDS)-1 + (gp_enter_shm->tail - gp_enter_shm->head) * sizeof(EnterMsg);
			BYTE* tmp_mem = realloc(gp_enter_shm, old_size);
*/// 这部分算法有问题！！！
			// detach shm from local address space!
			shmdt(gp_enter_shm);
			// destroy shm segment.
			shmctl(shm_id, IPC_RMID, NULL);
			// recreate shm.
			return -1;
		}
	}
	printf("\n----initialize maxsz:%d, head=%d, tail=%d", gp_enter_shm->max_size(),
		gp_enter_shm->head(), gp_enter_shm->tail());

	return 0;
}

int append_msg(const EnterMsg* msg)
{
	gp_enter_shm->push(*msg);

	printf("\n----post-append--- size:%d, head=%d, tail=%d", gp_enter_shm->size(),
		gp_enter_shm->head(), gp_enter_shm->tail());

	// 写入数据库
	if (gp_enter_shm->size() >= FLUSH_ROW)
	{
		flush_msg();
	}
}

int flush_msg()
{
/*
	if ( 0 == rand()%4 )
	{
		return 0;
	}
*/

	// write to database.
	for (int i=0; i< FLUSH_ROW; i++)
		gp_enter_shm->pop();

	printf("\n--post-flush msg----------------head:%d, tail:%d--",
		gp_enter_shm->head(), gp_enter_shm->tail());
}




