#pragma once

#include "SvrSocket.h"
#include "CircularQueue.h"
#include "InnerMsg.h"

typedef CircularQueue<EnterMsg> EnterQueue;
typedef CircularQueue<LeaveMsg> LeaveQueue;

#define BLOCK_SIZE	50*1024*1024

#ifdef WIN32
#define key_t   long
#endif


class LogSvr
{
public:
    LogSvr();
    ~LogSvr();

    static int msg_proc(const char* msg, int len);

    int initialize();
    int release();

    int work();
    int init_shmem();   // init share mem with the config.

// data members.
    static EnterQueue* _enter_q;
    static LeaveQueue* _leave_q;
private:
    // share mem property.
    key_t   _shm_key;
    int     _shm_size;
    int     _shm_mask;

};
