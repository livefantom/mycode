#ifndef _INNER_MESSAGE_H
#define _INNER_MESSAGE_H

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

typedef struct msg_hdr
{
    uint16_t    type;
    uint16_t    size;   // size of the following data
} MsgHeader;

#define MSG_ENTER_WORLD 0x0001
#define MSG_LEAVE_WORLD 0x0002


#endif//(_INNER_MESSAGE_H)
