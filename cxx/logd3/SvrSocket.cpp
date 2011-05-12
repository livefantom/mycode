#include "SvrSocket.h"
#include <string.h>
#include <errno.h>
#include <assert.h>

#ifdef WIN32
#define SHUT_RDWR   SD_BOTH
#define SHUT_RD     SD_RECEIVE
#define SHUT_WR     SD_SEND
#endif

int SvrSocket::initialize(int port, MsgProc proc)
{
    this->setProc(proc);
#ifdef WIN32
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(1,1), &WSAData) != 0) 
    {
        return -1;
    }
#endif

    fd_set fds;

    struct sockaddr_in svr_addr;
    bzero( &svr_addr, sizeof(svr_addr) );
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    svr_addr.sin_port = htons(port);

    if ( -1 == (_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) )
    {
        perror("socket error");
        return -1;
    }

    if ( -1 == bind(_sock_fd, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) )
    {
        perror("bind error");
        return -1;
    }

	printf("socket initialize complete, start receiving data...\n");
    while(1)
    {
        FD_ZERO(&fds);
        FD_SET(_sock_fd, &fds);
        if ( -1 == select(_sock_fd + 1, &fds, NULL, NULL, 0) )
        {
#ifndef WIN32
            if (EINTR == errno)
                continue;
            else
#endif
            {
                perror("select error:");
                break;
            }
        }
        if ( FD_ISSET(_sock_fd, &fds) )
        {
            on_recv();
        }
    }

    return 0;
}

int SvrSocket::on_recv()
{
    char* pBuf = (char*)_rcv_buf + _rcv_len;
    int buf_len = BUFSIZ - _rcv_len;
    bzero(pBuf, buf_len);
    int len = recv(_sock_fd, pBuf, buf_len, 0);
    if (len <= 0)
    {
        perror("recv error:");
    }
    else
    {
        assert( _msg_proc != 0 );
        // parse from head.
        _rcv_len = _msg_proc( (char*)_rcv_buf, len );
        if (_rcv_len > 0)
        {
            // the data need to reserve in buffer.
            memmove(_rcv_buf, _rcv_buf + len - _rcv_len, _rcv_len);
        }
    }
    return 0;
}

int SvrSocket::release()
{
    shutdown(_sock_fd, SHUT_RDWR);
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}

