#include "ub482.h"

enum PositionVelocityType gnssState = TYPE_NONE;
static struct Serial serialSet;
static struct Serial serialGet;


struct Ub482GnssData ub482GnssData;

static int ub482_send_config_cmd(struct Serial *sn,
                                 char* cmd,
                                 char *result,
                                 unsigned int waittime,        //两个指令间的时间间隔ms
                                 unsigned char retry,          //失败重试次数
                                 unsigned int timeout)         //指令返回超时ms
{
	char *msg_p = NULL;
	unsigned int  newtime = 0;
	unsigned char retry_num = 0;
    unsigned char retryflag = 0;
    char result_buf[64];
    char flag_ok = 0;

	memset(result_buf,0,64);

    fprintf(stdout,"%s: cmd:%s\n",__func__,cmd);

    SerialWrite(sn, cmd, strlen((const char *)cmd));

	while(1)
	{
        usleep(10 * 1000);

		if(newtime >= timeout)
		{
			if(++ retry_num > retry)
				return -1;

			retryflag = 1;
		}

        flag_ok = SerialRead(sn, result_buf, 64);

		if(flag_ok > 0)
		{
			fprintf(stdout,"%s: cmd_rsp:%s\n",__func__,result_buf);

			flag_ok = 0;

			msg_p = strstr((char *)result_buf,(char *)result);

			if(msg_p != NULL)
			{
				break;
			}
		}
		else
		{
			newtime ++;
		}

		if(retryflag == 1)
		{
			retryflag = 0;
			newtime = 0;

			if(retry_num > 0 && retry_num < retry + 1)
			{
				fprintf(stderr,"%s: retry cmd:\n%s",__func__,cmd);

                SerialWrite(sn, cmd, strlen((const char *)cmd));
			}
		}
	}

	usleep(waittime * 1000);

	return 0;
}

static unsigned long int ub482_get_data_serial_port_baudrate(enum SerialBaudrate baudrate)
{
    unsigned long int rate = 115200;

    switch(baudrate)
    {
        case SPABAUD_50:
            rate = 50;
        break;

        case SPABAUD_110:
            rate = 110;
        break;

        case SPABAUD_300:
            rate = 300;
        break;

        case SPABAUD_600:
            rate = 600;
        break;

        case SPABAUD_1200:
            rate = 1200;
        break;

        case SPABAUD_2400:
            rate = 2400;
        break;

        case SPABAUD_4800:
            rate = 4800;
        break;

        case SPABAUD_9600:
            rate = 9600;
        break;

        case SPABAUD_19200:
            rate = 19200;
        break;

        case SPABAUD_38400:
            rate = 38400;
        break;

        case SPABAUD_57600:
            rate = 57600;
        break;

        case SPABAUD_115200:
            rate = 115200;
        break;

        case SPABAUD_230400:
            rate = 230400;
        break;

        case SPABAUD_460800:
            rate = 460800;
        break;

        case SPABAUD_921600:
            rate = 921600;
        break;

        default:
        break;
    }

    return rate;
}

static int ub482_config_init(enum SerialBaudrate baudrate)
{
    int ret = 0;
    int ret1 = 0;
    int ret2 = 0;
    int ret3 = 0;
    int ret4 = 0;
    int ret5 = 0;
    int ret6 = 0;
    char buf[32] = {0};
    long int rate = 115200;

    rate = ub482_get_data_serial_port_baudrate(baudrate);

    snprintf(buf, 32, "CONFIG COM1 %ld\r\n",rate);

    ret1 = ub482_send_config_cmd(&serialSet,buf,                             "response: OK",100,10,100);
    ret2 = ub482_send_config_cmd(&serialSet,"GPGGA COM1 1\r\n",              "response: OK",100,10,100);
    ret3 = ub482_send_config_cmd(&serialSet,"LOG COM1 BESTPOSA ONTIME 1\r\n","response: OK",100,10,100);
    ret4 = ub482_send_config_cmd(&serialSet,"LOG COM1 BESTVELA ONTIME 1\r\n","response: OK",100,10,100);
    ret5 = ub482_send_config_cmd(&serialSet,"LOG COM1 HEADINGA ONTIME 1\r\n","response: OK",100,10,100);
    ret6 = ub482_send_config_cmd(&serialSet,"LOG COM1 TIMEA ONTIME 1\r\n",   "response: OK",100,10,100);

    if(ret1 || ret2 || ret3 || ret4 || ret5 || ret6)
    {
        ret = -1;
    }

    return ret;
}

static int ub482_serial_init(struct CmdArgs *args)
{
    int ret = 0;

    OPEN_SERIAL1:
    ret = SerialInit(&serialSet, 
                     args->serial1,
                     args->baudrate1,
                     args->stopbits1,
                     args->protocol1,
                     args->parity1,
                     args->databits1,1);
    if(ret)
    {
        fprintf(stderr, "%s: open %s failed\n",__func__,args->serial1);

        sleep(5);

        goto OPEN_SERIAL1;
    }

    OPEN_SERIAL2:
    ret = SerialInit(&serialGet,
                     args->serial2,
                     args->baudrate2,
                     args->stopbits2,
                     args->protocol2,
                     args->parity2,
                     args->databits2,1);
    if(ret)
    {
        fprintf(stderr, "%s: open %s failed\n",__func__,args->serial2);

        sleep(5);

        goto OPEN_SERIAL2;
    }

    return ret;
}

static int recvNtripDataMsgAndWriteToUb482(void)
{
    int ret = 0;
    char *msg = NULL;
    unsigned short msg_len = 0;
    struct NormalMsg *ntrip_rtcm_msg = NULL;

    ret = xQueueReceive((key_t)KEY_NTRIP_RTCM_MSG,(void **)&ntrip_rtcm_msg,0);
    if(ret == -1)
    {
        return -1;
    }

    fprintf(stdout, "%s: recv ntrip rtcm queue msg\n",__func__);

    msg = (char *)ntrip_rtcm_msg->msg;
    msg_len = ntrip_rtcm_msg->len;
    fprintf(stdout,"%s: msg_len = %d\n",__func__,msg_len);

    ret = SerialWrite(&serialSet, msg, msg_len);

    free(ntrip_rtcm_msg->msg);
    ntrip_rtcm_msg->msg = NULL;
    free(ntrip_rtcm_msg);
    ntrip_rtcm_msg = NULL;

    return ret;
}

enum PositionVelocityType getPositionVelocityType(char *msg)
{
    enum PositionVelocityType type = TYPE_NONE;

     if(strstr(msg, "NONE") != NULL)
     {
        type = TYPE_NONE;
     }
     else if(strstr(msg, "FIXEDPOS") != NULL)
     {
        type = TYPE_FIXEDPOS;
     }
     else if(strstr(msg, "FIXEDHEIGHT") != NULL)
     {
        type = TYPE_FIXEDHEIGHT;
     }
     else if(strstr(msg, "DOPPLER_VELOCITY") != NULL)
     {
        type = TYPE_DOPPLER_VELOCITY;
     }
     else if(strstr(msg, "SINGLE") != NULL)
     {
        type = TYPE_SINGLE;
     }
     else if(strstr(msg, "PSRDIFF") != NULL)
     {
        type = TYPE_PSRDIFF;
     }
     else if(strstr(msg, "WAAS") != NULL)
     {
        type = TYPE_WAAS;
     }
     else if(strstr(msg, "L1_FLOAT") != NULL)
     {
        type = TYPE_L1_FLOAT;
     }
     else if(strstr(msg, "IONOFREE_FLOAT") != NULL)
     {
        type = TYPE_IONOFREE_FLOAT;
     }
     else if(strstr(msg, "NARROW_FLOAT") != NULL)
     {
        type = TYPE_NARROW_FLOAT;
     }
     else if(strstr(msg, "L1_INT") != NULL)
     {
        type = TYPE_L1_INT;
     }
     else if(strstr(msg, "WIDE_INT") != NULL)
     {
        type = TYPE_WIDE_INT;
     }
     else if(strstr(msg, "NARROW_INT") != NULL)
     {
        type = TYPE_NARROW_INT;
     }
     else if(strstr(msg, "INS") != NULL)
     {
        type = TYPE_INS;
     }
     else if(strstr(msg, "INS_PSRSP") != NULL)
     {
        type = TYPE_INS_PSRSP;
     }
     else if(strstr(msg, "INS_PSRDIFF") != NULL)
     {
        type = TYPE_INS_PSRDIFF;
     }
     else if(strstr(msg, "INS_RTKFLOA") != NULL)
     {
        type = TYPE_INS_RTKFLOA;
     }
     else if(strstr(msg, "INS_RTKFIXED") != NULL)
     {
        type = TYPE_INS_RTKFIXED;
     }
     else
     {
        type = TYPE_NONE;
     }

     return type;
}

static void getBestPositionData(char *msg, int msg_len)
{
    int pos = 0;
    int weeks = 0;
    double seconds = 0;
    char buf[32] = {0};

    get_str1((unsigned char *)msg, (unsigned char *)",", 5, (unsigned char *)",", 6, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    weeks = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 6, (unsigned char *)",", 7, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    seconds = atof(buf);

    ub482GnssData.time_stamp = (double)(weeks * 7 * 24 * 3600) + seconds;

    pos = mystrstr((unsigned char *)msg, (unsigned char *)";", msg_len, 1);
    
    msg += (pos + 1);

    fprintf(stdout,"%s: msg:%s",__func__,msg);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 1, (unsigned char *)",", 2, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.pos_type = getPositionVelocityType(buf);
    gnssState = ub482GnssData.pos_type;

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 2, (unsigned char *)",", 3, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.lat = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 3, (unsigned char *)",", 4, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.lon = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 4, (unsigned char *)",", 5, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.height = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 7, (unsigned char *)",", 8, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.lat_std = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 8, (unsigned char *)",", 9, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.lon_std = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 9, (unsigned char *)",", 10, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.height_std = atof(buf);
}

static void getBestVelocityData(char *msg, int msg_len)
{
    int pos = 0;
    char buf[32] = {0};
    double velocity = 0.0f;
    double angle = 0.0f;
    double radian = 0.0f;

    pos = mystrstr((unsigned char *)msg, (unsigned char *)";", msg_len, 1);
    
    msg += (pos + 1);

    fprintf(stdout,"%s: msg:%s",__func__,msg);

    get_str1((unsigned char *)msg, (unsigned char *)",", 1, (unsigned char *)",", 2, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.vel_type = getPositionVelocityType(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 4, (unsigned char *)",", 5, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    velocity = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 5, (unsigned char *)",", 6, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    angle = 90.0f - atof(buf);

    radian = angle * PI / 360.0f;

    ub482GnssData.vx = velocity * cos(radian);
    ub482GnssData.vy = velocity * sin(radian);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 6, (unsigned char *)",", 7, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.vz = atof(buf);

    ub482GnssData.vx_std = 9999.0f;
    ub482GnssData.vy_std = 9999.0f;
    ub482GnssData.vz_std = 9999.0f;
}

static void getBestAttitudeData(char *msg, int msg_len)
{
    int pos = 0;
    char buf[32] = {0};

    pos = mystrstr((unsigned char *)msg, (unsigned char *)";", msg_len, 1);
    
    msg += (pos + 1);

    fprintf(stdout,"%s: msg:%s",__func__,msg);

    get_str1((unsigned char *)msg, (unsigned char *)",", 1, (unsigned char *)",", 2, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.att_type = getPositionVelocityType(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 3, (unsigned char *)",", 4, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.yaw = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 4, (unsigned char *)",", 5, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.pitch = atof(buf);

    ub482GnssData.roll = 9999.0f;

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 6, (unsigned char *)",", 7, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.yaw_std = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 7, (unsigned char *)",", 8, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    ub482GnssData.pitch_std = atof(buf);

    ub482GnssData.roll_std = 9999.0f;    
}

void sendTimeStampMsgToThreadSync(void)
{
    int ret = 0;
    double *time_stamp = NULL;

    time_stamp = (double *)malloc(sizeof(double));
    if(time_stamp != NULL)
    {
        *time_stamp = ub482GnssData.time_stamp;
    }

    ret = xQueueSend((key_t)KEY_UB482_TIME_STAMP_MSG,time_stamp);
    if(ret == -1)
    {
        fprintf(stderr, "%s: send ub482 time stamp queue msg failed\n",__func__);
    }
}

static int recvAndParseUb482GnssData(void)
{
    int ret = 0;
    int recv_len = 0;
    struct timeval tv;
    static time_t time_sec = 0;
    unsigned char check_num_buf[8] = {0};
    unsigned int check_num_recv = 0;
    unsigned int check_num_calc = 0;
    char check_buf[10] = {0};
    static char recv_buf[MAX_UB482_BUF_LEN] = {0};
    char *gpgga_msg = NULL;

    memset(recv_buf,0,MAX_UB482_BUF_LEN);
    recv_len = SerialRead(&serialGet, recv_buf, MAX_UB482_BUF_LEN - 1);
    if(recv_len > 0)
    {
        if((recv_buf[0] != '#' && recv_buf[0] != '$') || 
            recv_buf[recv_len - 2] != '\r' || 
            recv_buf[recv_len - 1] != '\n')
        {
            fprintf(stderr, "%s: recv invalid data\n",__func__);
            return -1;
        }

        get_str1((unsigned char *)recv_buf, (unsigned char *)"*", 1, (unsigned char *)"\r\n", 1, (unsigned char *)check_buf);

        if(strlen(check_buf) != 2 && strlen(check_buf) != 8)
        {
            fprintf(stderr, "%s: check num len error\n",__func__);
            return -1;
        }

        if(strlen(check_buf) == 2)
        {
            StrToHex((unsigned char *)&check_num_recv, check_buf, 1);
        }
        else
        {
            StrToHex(check_num_buf, check_buf, 4);
            check_num_recv = ((((unsigned int)check_num_buf[0]) << 24) & 0xFF000000) + 
                             ((((unsigned int)check_num_buf[1]) << 16) & 0x00FF0000) + 
                             ((((unsigned int)check_num_buf[2]) <<  8) & 0x0000FF00) +
                             ((((unsigned int)check_num_buf[3]) <<  0) & 0x000000FF);
        }

        if(strstr(recv_buf, "$GPGGA") != NULL)
        {
            check_num_calc = CalCheckOr((unsigned char *)&recv_buf[1], recv_len - 6);
            if(check_num_recv != check_num_calc)
            {
                fprintf(stderr, "%s: $GPGGA data check num error,recv is %d but calc is %d\n",
                        __func__,check_num_recv,check_num_calc);
                return -1;
            }

            if(recv_len <= 26)
            {
                fprintf(stderr, "%s: GPGGA data is null,GPGGA data len is %d,should more than 26\n",__func__,recv_len);
                return -1;
            }

            gettimeofday(&tv,NULL);
            if(tv.tv_sec - time_sec >= 1)
            {
                time_sec = tv.tv_sec;

                gpgga_msg = (char *)malloc(recv_len - 2 + 1);
                if(gpgga_msg != NULL)
                {
                    memset(gpgga_msg,0,recv_len - 2 + 1);
                    memcpy(gpgga_msg,recv_buf,recv_len - 2);

                    ret = xQueueSend((key_t)KEY_UB482_GPGGA_MSG,gpgga_msg);
                    if(ret == -1)
                    {
                        fprintf(stderr, "%s: send gpgga queue msg failed\n",__func__);
                    }
                }
            }
        }
        else
        {
            check_num_calc = CRC32((unsigned char *)&recv_buf[1], recv_len - 12);
            if(check_num_recv != check_num_calc)
            {
                fprintf(stderr, "%s: other data check num error\n",__func__);
                return -1;
            }

            if(strstr(recv_buf, "#BESTPOSA") != NULL)
            {
                getBestPositionData(recv_buf,recv_len);
            }
            else if(strstr(recv_buf, "#BESTVELA") != NULL)
            {
                getBestVelocityData(recv_buf,recv_len);
            }
            else if(strstr(recv_buf, "#HEADINGA") != NULL)
            {
                getBestAttitudeData(recv_buf,recv_len);
            }
            else if(strstr(recv_buf, "#TIMEA") != NULL)
            {
                sendTimeStampMsgToThreadSync();
/*
                printf("*******************************************\n");
                printf("* time_stamp: %lf\n",ub482GnssData.time_stamp);
                printf("* pos_type  : %d\n",(unsigned char)ub482GnssData.pos_type);
                printf("* vel_type  : %d\n",(unsigned char)ub482GnssData.vel_type);
                printf("* att_type  : %d\n",(unsigned char)ub482GnssData.att_type);
                printf("* lat       : %lf\n",ub482GnssData.lat);
                printf("* lon       : %lf\n",ub482GnssData.lon);
                printf("* height    : %lf\n",ub482GnssData.height);
                printf("* lat_std   : %lf\n",ub482GnssData.lat_std);
                printf("* lon_std   : %lf\n",ub482GnssData.lon_std);
                printf("* height_std: %lf\n",ub482GnssData.height_std);
                printf("* vx        : %lf\n",ub482GnssData.vx);
                printf("* vy        : %lf\n",ub482GnssData.vy);
                printf("* vz        : %lf\n",ub482GnssData.vz);
                printf("* vx_std    : %lf\n",ub482GnssData.vx_std);
                printf("* vy_std    : %lf\n",ub482GnssData.vy_std);
                printf("* vz_std    : %lf\n",ub482GnssData.vz_std);
                printf("* pitch     : %f\n",ub482GnssData.pitch);
                printf("* roll      : %f\n",ub482GnssData.roll);
                printf("* yaw       : %f\n",ub482GnssData.yaw);
                printf("* pitch_std : %f\n",ub482GnssData.pitch_std);
                printf("* roll_std  : %f\n",ub482GnssData.roll_std);
                printf("* yaw_std   : %f\n",ub482GnssData.yaw_std);
                printf("*******************************************\n");
*/
            }
            else
            {
                fprintf(stderr, "%s: invalid data frame\n",__func__);
                return -1;
            }
        }
    }
    else
    {
        return -1;
    }

    return ret;
}

void *thread_ub482(void *arg)
{
    int ret = 0;
    struct CmdArgs *args = (struct CmdArgs *)arg;

    ret = ub482_serial_init(args);                  //初始化并打开串口
    if(ret != 0)
    {
        goto THREAD_EXIT;
    }

    ret = ub482_config_init(args->baudrate2);       //配置ub482
    if(ret)
    {
        fprintf(stderr, "%s: ub482 config init failed\n",__func__);
    }

    while(1)
    {
        recvAndParseUb482GnssData();                        //接收并解析ub482数据
        recvNtripDataMsgAndWriteToUb482();                  //接收ntrip服务器的矫正数据，并发送给UB482
        
        usleep(1000 * 10);
    }

THREAD_EXIT:
    pthread_exit("thread_ub482: open serial port failed\n");
}















