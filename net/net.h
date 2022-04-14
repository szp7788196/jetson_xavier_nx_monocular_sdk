#ifndef __NET_H
#define __NET_H

#define MAX_NET_BUF_LEN		1024

enum ConnectState
{
    UNKNOW_STATE 	= 255,
    INITIAL         = 0,
	GOT_PORT		= 1,
	GOT_IP			= 2,
	CREATED_SOCKED	= 3,
	CONNECTED		= 4,
	LOGGED_IN		= 5,
	
};

extern enum ConnectState connectState;


void *thread_net(void *arg);

#endif
