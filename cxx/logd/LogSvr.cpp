#include "LogSvr.h"
#ifndef WIN32
#   include <sys/ipc.h>
#   include <sys/shm.h>
#endif
#include <errno.h>
#include <assert.h>

EnterQueue* LogSvr::_enter_q = 0;
LeaveQueue* LogSvr::_leave_q = 0;

LogSvr::LogSvr(void)
{
    _shm_key = 0x444444;
    _shm_size = 2 * BLOCK_SIZE;
    _shm_mask = 0666;
}

LogSvr::~LogSvr(void)
{
}

int LogSvr::initialize()
{
    //TODO: ftok(argv[0], 0)
    init_shmem();
    return 0;
}

int LogSvr::release()
{
    return 0;
}

int LogSvr::work()
{
    SvrSocket sock;
    sock.initialize(17171, msg_proc);
    // start thread.
    sock.release();
    return 0;
}

int LogSvr::init_shmem()
{
    int shm_id = -1;
    void* shm_addr = (void*)-1;
    bool shm_exist = false;

#ifndef WIN32
    struct shmid_ds shm_state;

    // create shm file.
    if ( -1 == (shm_id = shmget(_shm_key, _shm_size, _shm_mask | IPC_CREAT | IPC_EXCL))
        && EEXIST == errno )
    {		// file already exists, read it!
        shm_exist = true;
        shm_id = shmget(_shm_key, 0,_shm_mask);
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

    _enter_q = (EnterQueue*)shm_addr;
    _leave_q = (LeaveQueue*)((char*)shm_addr + BLOCK_SIZE);
    if ( !shm_exist )
    {
        // the OS init the shmval to 0 while create.
        _enter_q->resize( (BLOCK_SIZE - sizeof(EnterQueue))/sizeof(EnterMsg) );
        _leave_q->resize( (BLOCK_SIZE - sizeof(LeaveQueue))/sizeof(LeaveMsg) );
    }
    else
    {
        // realloc shm when size-setting changed!
        if ( -1 == shmctl(shm_id, IPC_STAT, &shm_state) )
        {
            perror("shmctl error:");
            return -1;
        }
        else if ( shm_state.shm_segsz != _shm_size )
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
#endif
    printf("----initialize EnterQ---- maxsz:%d, head:%d, tail:%d.\n", _enter_q->max_size(),
        _enter_q->head(), _enter_q->tail());
    printf("----initialize LeaveQ---- maxsz:%d, head:%d, tail:%d.\n", _leave_q->max_size(),
        _leave_q->head(), _leave_q->tail());

    return 0;
}

int LogSvr::msg_proc(const char* msg, int len)
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
            assert(_enter_q != 0);
            _enter_q->push( *(EnterMsg*)(pData + sizeof(MsgHeader)) );
            break;
        case MSG_LEAVE_WORLD:
            assert(_leave_q != 0);
            _leave_q->push( *(LeaveMsg*)(pData + sizeof(MsgHeader)) );
            break;
        }
        left_len -= msg_len;
        pData += msg_len;
    }
    return left_len;
}
