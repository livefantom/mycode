#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifndef BYTE
#	define BYTE unsigned char
#endif

#define ENTER_KEY 0X444444
#define LEAVE_KEY 0X555555
#define FLUSH_ROW	3
#define SHM_SIZE	50*1024*1024

typedef struct enter_msg
{
	uint64_t log_id;
	uint32_t user_id;
	uint32_t role_id;
	uint16_t role_level;
	uint32_t gateway_id;
	uint32_t server_id;
	time_t 	 login_time;
	uint32_t user_ip;
	uint32_t user_port;
} EnterMsg;

typedef struct leave_msg
{
	uint64_t log_id;
	time_t 	 level_time;
	uint16_t role_level;
	uint32_t money1;
	uint32_t money2;
	uint32_t experience;
} LeaveMsg;

struct shmem_ds
{
	uint32_t maxsz;
	uint32_t head;
	uint32_t tail;
	BYTE	 data;
} ShMemDS;

int append_msg(const EnterMsg* msg);

int flush_msg();
int OnRecv();
int parse_and_log(const BYTE* msg, int len);

int init_shmem(key_t key, int size, int mask);
int free_shmem();

int print_mem(const BYTE* mem, int len)
{
	for (int i = 0; i < len; i++)
    {
        if (i%16 == 0)
            printf("\n%06Xh:©¦", i);
        else if (i%8 == 0)
            printf(" ");
        printf("%02X ", mem[i]);
        if ( (i+1)%16 == 0 )
        {
        	printf(" ©¦");
        	for(int j = i-15; j<=i; ++j)
        	{
            	printf("%c", mem[j]);
            }
        }
    }
	fflush(stdout);
}


