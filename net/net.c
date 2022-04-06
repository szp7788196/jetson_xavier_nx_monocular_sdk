#include "net.h"


static const char encodingTable [64] = 
{
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

enum ConnectState connectState = INITIAL;
static struct QueueMsgGpgga ub482GpggaMsg;
static struct QueueMsgNtrip ntripDataMsg;
static int ub482GpggaMsgId = -1;
static int ntripDataMsgId = -1;
static int socketFd = 0;

static int encode(char *buf, int size, const char *user, const char *pwd)
{
    unsigned char inbuf[3];
    char *out = buf;
    int i, sep = 0, fill = 0, bytes = 0;

    while(*user || *pwd)
    {
        i = 0;
        
        while(i < 3 && *user)
        {
            inbuf[i ++] = *(user ++);
        }
        
        if(i < 3 && !sep)
        {
            inbuf[i ++] = ':'; ++sep;
        
        }
        while(i < 3 && *pwd)
        {
            inbuf[i ++] = *(pwd ++);
        }

        while(i < 3)
        {
            inbuf[i ++] = 0; ++fill;
        }

        if(out - buf < size - 1)
        {
            *(out ++) = encodingTable[(inbuf [0] & 0xFC) >> 2];
        }
        
        if(out - buf < size - 1)
        {
            *(out ++) = encodingTable[((inbuf [0] & 0x03) << 4) | ((inbuf [1] & 0xF0) >> 4)];
        }
        
        if(out - buf < size - 1)
        {
            if(fill == 2)
            {
                *(out ++) = '=';
            }
            else
            {
                *(out ++) = encodingTable[((inbuf [1] & 0x0F) << 2) | ((inbuf [2] & 0xC0) >> 6)];
            }
            
        }

        if(out - buf < size - 1)
        {
            if(fill >= 1)
            {
                *(out ++) = '=';
            }
            else
            {
                *(out ++) = encodingTable[inbuf [2] & 0x3F];
            }
        }

        bytes += 4;
    }

    if(out-buf < size)
    {
        *out = 0;
    }
    
    return bytes;
}

static int packHttpLogonNtripServerFrame(char *outbuf,struct CmdArgs *args)
{
    int send_len = 0;

    if(args->mount_point != NULL && 
       args->user_name != NULL && 
       args->password != NULL)
    {
        if(strstr(args->server, "vrs.ppprtk.tk") != NULL)
        {
            send_len = snprintf(outbuf, MAX_NET_BUF_LEN - 64,
                                "GET /%s HTTP/1.0\r\n"
                                "User-Agent: NTRIP GNSSInternetRadio/1.4.10\r\n"
                                "Accept: */*\r\n"
                                "Connection: close\r\n"
                                "Authorization: Basic ",
                                args->mount_point);
        }
        else
        {
            send_len = snprintf(outbuf, MAX_NET_BUF_LEN - 64,
                                "GET /%s HTTP/1.1\r\n"
                                "Host: %s\r\n"
                                "Ntrip-Version: Ntrip/2.0\r\n"
                                "User-Agent: NTRIP NtripClientPOSIX/1.51\r\n"
                                "Connection: close\r\n"
                                "Authorization: Basic ",
                                args->mount_point, args->server);
        }

        if(send_len < 0 || send_len > MAX_NET_BUF_LEN - 64)
        {
            fprintf(stderr, "thread_net: Requested data too long\n");
            return 0;
        }
        
        send_len += encode(outbuf + send_len, MAX_NET_BUF_LEN -send_len - 4, args->user_name, args->password);
        if(send_len > MAX_NET_BUF_LEN - 4)
        {
            fprintf(stderr, "thread_net: user name and/or password too long\n");
            return 0;
        }

        strcat(outbuf,"\r\n\r\n");

        send_len += 4;
    }

    return send_len;
}

static int logonToNtripServer(struct CmdArgs *args)
{
    int ret = 0;
    unsigned char wait_cnt = 100;
    char send_buf[MAX_NET_BUF_LEN] = {0};

    ret = packHttpLogonNtripServerFrame(send_buf,args);
    if(ret > 0)
    {
        ret = send(socketFd, send_buf, ret, 0);
    }

    printf("%s",send_buf);

    if(ret > 0)
    {
        while(wait_cnt --)
        {
            usleep(1000 * 100);

            if(connectState == LOGGED_IN)
            {
                return 0;
            }
        }
    }

    return -1;
}

static int recvUb482GpggaMsgAndSendToNtripServer(struct CmdArgs *args)
{
    int ret = 0;
    char send_buf[MAX_NET_BUF_LEN] = {0};

    if(ub482GpggaMsgId == -1)
    {
        fprintf(stderr, "thread_net: no ub482GpggaMsg queue\n");
        return -1;
    }

    ret = msgrcv(ub482GpggaMsgId,&ub482GpggaMsg,sizeof(ub482GpggaMsg.mtext),1,IPC_NOWAIT);
    if(ret == -1)
    {
        return -1;
    }

    ret = snprintf(send_buf,MAX_NET_BUF_LEN - 1,"%s\r\n\r\n",ub482GpggaMsg.mtext);
    
    if(ret > 0)
    {
        ret = send(socketFd, send_buf, ret, 0);
    }

    memset(&ub482GpggaMsg,0,sizeof(struct QueueMsgGpgga));

    return ret;
}

static int recvNtripDataAndSendToNtripMsg(void)
{
    int ret = 0;
    int revc_len = 0;
    static int recv_null_cnt = 0;
    char *msg = NULL;
    static char recv_buf[MAX_NET_BUF_LEN] = {0};

    revc_len = recv(socketFd, recv_buf, MAX_NET_BUF_LEN - 1, 0);
    if(revc_len > 0)
    {
        printf("recv_len:%d\n",revc_len);
        recv_null_cnt = 0;

        if(revc_len >= 12)
        {
            if(strstr(recv_buf, "ICY 200 OK") != NULL)
            {
                connectState = LOGGED_IN;

                if((unsigned char)recv_buf[14] == 0xD3)
                {
                    msg = recv_buf + 14;
                    revc_len -= 14;
                }
                else
                {
                    return 0;
                }
            }
            else if((unsigned char)recv_buf[0] == 0xD3)
            {
                msg = recv_buf;
            }
            else
            {
                return 0;
            }

            ret = check_rtcm3((unsigned char *)recv_buf, revc_len);
            if(ret == -1)
            {
                fprintf(stderr, "thread_net: ntrip data crc error\n");
                return 0;
            }

            memset(&ntripDataMsg,0,sizeof(struct QueueMsgNtrip));

            if(revc_len > NTRIP_MSG_MAX_LEN - 2)
            {
                fprintf(stderr, "thread_net: ntrip data len error\n");
                return 0;
            }

            ntripDataMsg.mtype = 1;
            ntripDataMsg.mtext[0] = (char)(((unsigned short)revc_len >> 8) & 0x00FF);
            ntripDataMsg.mtext[1] = (char)(((unsigned short)revc_len >> 0) & 0x00FF);

            memcpy(&ntripDataMsg.mtext[2],msg,revc_len);

            ret = msgsnd(ntripDataMsgId,&ntripDataMsg,sizeof(ntripDataMsg.mtext),0);
            if(ret == -1)
            {
                fprintf(stderr, "thread_net: send ntripDataMsg failed\n");
                return 0;
            }
        }
    }
    else if(revc_len == -1 || 
            revc_len == EBADF || 
            revc_len == ENOTSOCK)
    {
        recv_null_cnt = 0;
        return -1;
    }
    else if(revc_len == 0)
    {
        recv_null_cnt ++;

        if(recv_null_cnt >= 600)
        {
            recv_null_cnt = 0;
            return -1;
        }
    }

    return 0;
}

void *thread_net_recv(void *arg)
{
    int ret = 0;

    while(1)
    {
        if(connectState == CONNECTED || connectState == LOGGED_IN)
        {
            ret = recvNtripDataAndSendToNtripMsg();
            if(ret == -1)
            {
                fprintf(stderr, "thread_recv: disconnected form ntrip server\n");

                close(socketFd);

                connectState = GOT_IP;
            }
        }

        usleep(1000 * 10);
    }
}

static void netCreateMsgQueue(void)
{
    ub482GpggaMsgId = msgget((key_t)KEY_UB482_GPGGA_MSG, IPC_CREAT | 0777);
    if(ub482GpggaMsgId == -1)
    {
        fprintf(stderr, "thread_net: create ub482GpggaMsg failed\n");
    }
    ntripDataMsgId = msgget((key_t)KEY_NTRIP_MSG, IPC_CREAT | 0777);
    if(ntripDataMsgId == -1)
    {
        fprintf(stderr, "thread_net: create ntripDataMsg failed\n");
    }

    memset(&ub482GpggaMsg,0,sizeof(struct QueueMsgGpgga));
    memset(&ntripDataMsg,0,sizeof(struct QueueMsgNtrip));
}

void *thread_net(void *arg)
{
    int ret = 0;
    struct CmdArgs *args = (struct CmdArgs *)arg;
    static pthread_t tid_net_recv = 0;
    struct sockaddr_in server_addr;
    struct hostent *he = NULL;
    char *temp_p = NULL;
    long temp_v = 0;

    netCreateMsgQueue();            //创建消息队列

    while(1)
    {   
        switch((unsigned char)connectState)
        {
            case (unsigned char)UNKNOW_STATE:

            break;

            case (unsigned char)INITIAL:
                memset(&server_addr,0,sizeof(struct sockaddr_in));

                temp_v = strtol(args->port, &temp_p, 10);
                if(temp_v != 0 && (temp_p == NULL || !*temp_p))
                {
                    server_addr.sin_port = htons(temp_v);

                    connectState = GOT_PORT;
                }
            break;

            case (unsigned char)GOT_PORT:
                he = gethostbyname(args->server);
                if(he)
                {
                    server_addr.sin_family = AF_INET;
                    server_addr.sin_addr = *((struct in_addr *)he->h_addr);

                    connectState = GOT_IP;
                }
            break;

            case (unsigned char)GOT_IP:
                socketFd = socket(AF_INET, SOCK_STREAM, 0);
                if(socketFd)
                {
                    connectState = CREATED_SOCKED;
                }
            break;

            case (unsigned char)CREATED_SOCKED:
                ret = connect(socketFd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr));
                if(ret == 0)
                {
                    printf("thread_net: connected to ntrip server\n");
                    connectState = CONNECTED;
                }

                if(tid_net_recv == 0)
                {
                    fprintf(stderr, "thread_net: create thread_net_recv\n");

                    ret = pthread_create(&tid_net_recv,NULL,thread_net_recv,NULL);
                    if(0 != ret)
                    {
                        fprintf(stderr, "thread_net: create thread_net_recv failed\n");
                        exit(1);
                    }
                }
            break;

            case (unsigned char)CONNECTED:
                ret = logonToNtripServer(args);
                if(ret == -1)
                {
                    fprintf(stderr, "%s: logon ntrip server failed\n",__func__);
                }
                else
                {
                    fprintf(stderr, "%s: logon ntrip server success\n",__func__);
                }
            break;

            case (unsigned char)LOGGED_IN:
                recvUb482GpggaMsgAndSendToNtripServer(args);
            break;

            default:
            break;
        }

        usleep(100 * 1000);
    }
}



