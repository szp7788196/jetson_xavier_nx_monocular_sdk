#ifndef __NET_H
#define __NET_H

#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "monocular.h"
#include "serial.h"
#include "cmd_parse.h"


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
