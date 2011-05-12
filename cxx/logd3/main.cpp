#include "CircularQueue.h"
#include "InnerMsg.h"
//#include "types.h"
#ifndef WIN32
#   include <sys/ipc.h>
#   include <sys/shm.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stropts.h>
#include "SvrSocket.h"

#define BLOCK_SIZE	50*1024*1024

int msg_proc(const char* msg, int len);
#ifndef WIN32
int init_shmem(key_t key, int size, int mask);
#endif

typedef CircularQueue<EnterMsg> EnterQueue;
typedef CircularQueue<LeaveMsg> LeaveQueue;

EnterQueue* gp_enter_q;
LeaveQueue* gp_leave_q;

int main(int argc, char** argv)
{
#ifndef WIN32
	init_shmem(ftok(argv[0], 0), 2*BLOCK_SIZE, 0660);
#endif
    SvrSocket sock;
    sock.initialize(17171, msg_proc);
    // start thread.
    sock.release();
    return 0;
}

#ifndef WIN32
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

	gp_enter_q = (EnterQueue*)shm_addr;
	gp_leave_q = (LeaveQueue*)((char*)shm_addr + BLOCK_SIZE);
	if ( !shm_exist )
	{
		// the OS init the shmval to 0 while create.
		gp_enter_q->resize( (BLOCK_SIZE - sizeof(EnterQueue))/sizeof(EnterMsg) );
		gp_leave_q->resize( (BLOCK_SIZE - sizeof(LeaveQueue))/sizeof(LeaveMsg) );
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
			// TODO: flush old data to db or copy them to new shm.
			// detach shm from local address space!
			shmdt(shm_addr);
			// destroy shm segment.
			shmctl(shm_id, IPC_RMID, NULL);
			// recreate shm.
			return -1;
		}
	}
	printf("----initialize EnterQ---- maxsz:%d, head:%d, tail:%d.\n", gp_enter_q->max_size(),
		gp_enter_q->head(), gp_enter_q->tail());
	printf("----initialize LeaveQ---- maxsz:%d, head:%d, tail:%d.\n", gp_leave_q->max_size(),
		gp_leave_q->head(), gp_leave_q->tail());

	return 0;
}
#endif

int msg_proc(const char* msg, int len)
{
    const char* pData = msg;
    int left_len = len;
    while (left_len > sizeof(MsgHeader))
    {
        MsgHeader* pHdr = (MsgHeader*)pData;
        int msg_len = sizeof(MsgHeader) + pHdr->size;
        // msg is not a complete MSG.
        if (left_len < msg_len)
        {
            break;
        }
        // TODO: write to file while no spare mem.
        switch ( pHdr->type )
        {
        case MSG_ENTER_WORLD:
            assert(gp_enter_q != 0);
            gp_enter_q->push( *(EnterMsg*)(pData + sizeof(MsgHeader)) );
            break;
        case MSG_LEAVE_WORLD:
            assert(gp_leave_q != 0);
            gp_leave_q->push( *(LeaveMsg*)(pData + sizeof(MsgHeader)) );
            break;
        }
        left_len -= msg_len;
        pData += msg_len;
    }
    return left_len;
}
