#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE	4096

int main(int argc, char** argv)
{
	char* szIp = "192.168.1.229";
	int	nPort = 12001;
	struct sockaddr_in svrAddr;
	int fdConn = socket(AF_INET, SOCK_STREAM, 0);
	struct timeval rcvTimeO;
	int nLoop = 10;
	int i;
	rcvTimeO.tv_sec = 30;
	rcvTimeO.tv_usec = 0;

	char* szSendBuf = (char*)malloc(BUF_SIZE);
	memset(szSendBuf, 0, BUF_SIZE);
	
	if (argc >= 2)
		nPort = atoi(argv[1]);
	if (argc >= 3)
		rcvTimeO.tv_sec= atoi(argv[2]);
	if (argc >= 4)
		nLoop = atoi(argv[3]);

	if ( -1 == setsockopt(fdConn, SOL_SOCKET, SO_RCVTIMEO, (char*)&rcvTimeO, sizeof(rcvTimeO)) )
	{
		perror("setsockopt error");
		return -1;
	}

	svrAddr.sin_family = AF_INET;
	svrAddr.sin_addr.s_addr = inet_addr(szIp);
	svrAddr.sin_port = htons(nPort);
	if ( -1 == connect(fdConn, (struct sockaddr*)&svrAddr, sizeof(svrAddr)) )
	{
		perror("connect error");
		return -1;
	}

	for (i = 0; i < nLoop; i++)
	{	
		sprintf(szSendBuf, "test message! ！！！！ %d",i);
	
	    if ( -1 == send(fdConn, szSendBuf, strlen(szSendBuf), 0) )
	    {
	        perror("send error");
	        return -1;
	    }
	    while ( recv(fdConn, szSendBuf, BUF_SIZE, 0) > 0 )
	    {
	        printf("%s\n", szSendBuf);
	    }
	    perror("receive error");
	}
	shutdown(fdConn, SHUT_WR);
	close(fdConn);
	free(szSendBuf);
	szSendBuf = 0;

}
