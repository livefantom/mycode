#pragma once

#ifdef WIN32
#   include <OS.h>
//#   include <WinSock2.h>
#   define bzero(p, l) memset(p, 0, l)
#else
#	include <netinet/in.h>
#endif
#include <stdio.h>

typedef int (*MsgProc)(const char* msg, int len);

class SvrSocket
{
public:
    SvrSocket():_sock_fd(0),_rcv_len(0),_msg_proc(0){}
    ~SvrSocket(){}

    int initialize(int port, MsgProc proc);
    int release();
    int on_recv();
protected:
    void setProc(MsgProc proc){ _msg_proc = proc; }

private:
    int32_t     _sock_fd;
    int8_t      _rcv_buf[BUFSIZ];
    uint32_t    _rcv_len;
    MsgProc     _msg_proc;
};
