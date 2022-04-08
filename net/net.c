#include "net.h"


static const char encodingTable [64] = 
{
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

enum ConnectState connectState = INITIAL;
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
            fprintf(stderr, "%s: Requested data too long\n",__func__);
            return 0;
        }
        
        send_len += encode(outbuf + send_len, MAX_NET_BUF_LEN -send_len - 4, args->user_name, args->password);
        if(send_len > MAX_NET_BUF_LEN - 4)
        {
            fprintf(stderr, "%s: user name and/or password too long\n",__func__);
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
    char *gpgga_msg = NULL;

    ret = xQueueReceive((key_t)KEY_UB482_GPGGA_MSG,(void **)&gpgga_msg,0);
    if(ret == -1)
    {
        return -1;
    }

    ret = snprintf(send_buf,MAX_NET_BUF_LEN - 1,"%s\r\n\r\n",gpgga_msg);

    free(gpgga_msg);
    gpgga_msg = NULL;

    if(ret > 0)
    {
        ret = send(socketFd, send_buf, ret, 0);
    }

    return ret;
}

static int recvNtripDataAndSendToNtripMsg(void)
{
    int ret = 0;
    int revc_len = 0;
    static int recv_null_cnt = 0;
    char *msg = NULL;
    static char recv_buf[MAX_NET_BUF_LEN] = {0};
    struct NormalMsg *ntrip_rtcm_msg = NULL;

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
                fprintf(stderr, "%s: ntrip data crc error\n",__func__);
                return 0;
            }

            if(revc_len > NTRIP_RTCM_MSG_MAX_LEN)
            {
                fprintf(stderr, "%s: ntrip data len error\n",__func__);
                return 0;
            }

            ntrip_rtcm_msg = (struct NormalMsg *)malloc(sizeof(struct NormalMsg));
            if(ntrip_rtcm_msg != NULL)
            {
                ntrip_rtcm_msg->msg = (unsigned char *)malloc(revc_len);
                if(ntrip_rtcm_msg->msg != NULL)
                {
                    ntrip_rtcm_msg->len = revc_len;
                    memcpy(ntrip_rtcm_msg->msg,msg,revc_len);

                    ret = xQueueSend((key_t)KEY_NTRIP_RTCM_MSG,ntrip_rtcm_msg);
                    if(ret == -1)
                    {
                        fprintf(stderr, "%s: send ntrip rtcm queue msg failed\n",__func__);
                    }
                }
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
                fprintf(stderr, "%s: disconnected form ntrip server\n",__func__);

                close(socketFd);

                connectState = GOT_IP;
            }
        }

        usleep(1000 * 10);
    }
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
                    printf("%s: connected to ntrip server\n",__func__);
                    connectState = CONNECTED;
                }

                if(tid_net_recv == 0)
                {
                    fprintf(stderr, "%s: create thread_net_recv\n",__func__);

                    ret = pthread_create(&tid_net_recv,NULL,thread_net_recv,NULL);
                    if(0 != ret)
                    {
                        fprintf(stderr, "%s: create thread_net_recv failed\n",__func__);
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



