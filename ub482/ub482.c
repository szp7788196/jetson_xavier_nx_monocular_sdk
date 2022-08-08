#include "ub482.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "monocular.h"
#include "serial.h"
#include "cmd_parse.h"

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
    int ret7 = 0;
    int ret8 = 0;
    int ret9 = 0;
    int ret10 = 0;
    int ret11 = 0;
    char buf[32] = {0};
    long int rate = 115200;

    rate = ub482_get_data_serial_port_baudrate(baudrate);

    snprintf(buf, 32, "CONFIG COM1 %ld\r\n",rate);

    ret1  = AT_SendCmd(&serialSet,buf,                                   "response: OK",NULL,100,10,100);
    ret2  = AT_SendCmd(&serialSet,"GPGGA COM1 1\r\n",                    "response: OK",NULL,100,10,100);
    ret3  = AT_SendCmd(&serialSet,"LOG COM1 BESTPOSA ONTIME 1\r\n",      "response: OK",NULL,100,10,100);
    ret4  = AT_SendCmd(&serialSet,"LOG COM1 BESTVELA ONTIME 1\r\n",      "response: OK",NULL,100,10,100);
    ret5  = AT_SendCmd(&serialSet,"LOG COM1 HEADINGA ONTIME 1\r\n",      "response: OK",NULL,100,10,100);
    ret6  = AT_SendCmd(&serialSet,"LOG COM1 TIMEA ONTIME 1\r\n",         "response: OK",NULL,100,10,100);
    ret7  = AT_SendCmd(&serialSet,"LOG COM1 RANGEA ONTIME 1\r\n",        "response: OK",NULL,100,10,100);
    ret8  = AT_SendCmd(&serialSet,"GLOEPHEMA COM1 ONCHANGED\r\n",        "response: OK",NULL,100,10,100);
    ret9  = AT_SendCmd(&serialSet,"GPSEPHEMERISA COM1 ONCHANGED\r\n",    "response: OK",NULL,100,10,100);
    ret10 = AT_SendCmd(&serialSet,"BDSEPHEMA COM1 ONCHANGED\r\n",        "response: OK",NULL,100,10,100);
    ret11 = AT_SendCmd(&serialSet,"GALEPHEMA COM1 ONCHANGED\r\n",        "response: OK",NULL,100,10,100);

    if(ret1 || ret2 || ret3 || ret4 || ret5 || ret6 || ret7 || ret8 || ret9 || ret10 || ret11)
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
                     args->databits1,1,0);
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
                     args->databits2,1,0);
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

//    fprintf(stdout, "%s: recv ntrip rtcm queue msg\n",__func__);

    msg = (char *)ntrip_rtcm_msg->msg;
    msg_len = ntrip_rtcm_msg->len;
//    fprintf(stdout,"%s: msg_len = %d\n",__func__,msg_len);

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

//    fprintf(stdout,"%s: msg:%s",__func__,msg);

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

//    fprintf(stdout,"%s: msg:%s",__func__,msg);

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

//    fprintf(stdout,"%s: msg:%s",__func__,msg);

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

static void getEphemerisHeader(char *msg,struct EphemHeader *header)
{
    char buf[32] = {0};

    if(strstr(msg, "GPS") != NULL)
    {
        header->time_ref = 0;
    }
    else
    {
        header->time_ref = 1;
    }

    if(strstr(msg, "FINE") != NULL)
    {
        header->time_status = 1;
    }
    else
    {
        header->time_status = 0;
    }

    get_str1((unsigned char *)msg, (unsigned char *)",", 1, (unsigned char *)",", 2, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    header->cpu_idle = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 4, (unsigned char *)",", 5, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    header->week = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 5, (unsigned char *)",", 6, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    header->time = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 7, (unsigned char *)",", 8, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    header->version = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 8, (unsigned char *)",", 9, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    header->leap_sec = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 9, (unsigned char *)";", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    header->output_delay = atoi(buf);
}

static void getGlonssEphemeris(char *msg, int msg_len,struct GLOEPHEM *glo)
{
    int pos = 0;
    char buf[32] = {0};

    getEphemerisHeader(msg,&glo->header);

    pos = mystrstr((unsigned char *)msg, (unsigned char *)";", msg_len, 1);

    msg += pos;

    get_str1((unsigned char *)msg, (unsigned char *)";", 1, (unsigned char *)",", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->sloto = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 1, (unsigned char *)",", 2, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->freqo = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 2, (unsigned char *)",", 3, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->sat_type = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 4, (unsigned char *)",", 5, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->week = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 5, (unsigned char *)",", 6, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->time = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 6, (unsigned char *)",", 7, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->t_offset = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 7, (unsigned char *)",", 8, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->nt = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 10, (unsigned char *)",", 11, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->issue = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 11, (unsigned char *)",", 12, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->health = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 12, (unsigned char *)",", 13, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->pos_x = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 13, (unsigned char *)",", 14, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->pos_y = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 14, (unsigned char *)",", 15, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->pos_z = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 15, (unsigned char *)",", 16, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->vel_x = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 16, (unsigned char *)",", 17, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->vel_y = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 17, (unsigned char *)",", 18, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->vel_z = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 18, (unsigned char *)",", 19, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->ls_acc_x = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 19, (unsigned char *)",", 20, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->ls_acc_y = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 20, (unsigned char *)",", 21, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->ls_acc_z = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 21, (unsigned char *)",", 22, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->tau_n = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 22, (unsigned char *)",", 23, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->delat_tau_n = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 23, (unsigned char *)",", 24, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->gamma = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 24, (unsigned char *)",", 25, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->tk = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 25, (unsigned char *)",", 26, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->p = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 26, (unsigned char *)",", 27, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->ft = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 27, (unsigned char *)",", 28, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->age = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 28, (unsigned char *)"*", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    glo->flags = atoi(buf);
}

static void getGpsEphemeris(char *msg, int msg_len,struct GPSEPHEM *gps)
{
    int pos = 0;
    char buf[32] = {0};

    getEphemerisHeader(msg,&gps->header);

    pos = mystrstr((unsigned char *)msg, (unsigned char *)";", msg_len, 1);

    msg += pos;

    get_str1((unsigned char *)msg, (unsigned char *)";", 1, (unsigned char *)",", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->prn = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 1, (unsigned char *)",", 2, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->tow = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 2, (unsigned char *)",", 3, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->health = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 3, (unsigned char *)",", 4, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->iode_1 = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 4, (unsigned char *)",", 5, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->iode_2 = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 5, (unsigned char *)",", 6, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->week = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 6, (unsigned char *)",", 7, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->z_week = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 7, (unsigned char *)",", 8, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->toe = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 8, (unsigned char *)",", 9, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->a = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 9, (unsigned char *)",", 10, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->delat_n = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 10, (unsigned char *)",", 11, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->m_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 11, (unsigned char *)",", 12, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->ecc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 12, (unsigned char *)",", 13, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->omega = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 13, (unsigned char *)",", 14, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->cuc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 14, (unsigned char *)",", 15, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->cus = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 15, (unsigned char *)",", 16, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->crc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 16, (unsigned char *)",", 17, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->crs = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 17, (unsigned char *)",", 18, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->cic = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 18, (unsigned char *)",", 19, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->cis = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 19, (unsigned char *)",", 20, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->i_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 20, (unsigned char *)",", 21, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->idot = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 21, (unsigned char *)",", 22, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->omega_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 22, (unsigned char *)",", 23, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->omega_dot = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 23, (unsigned char *)",", 24, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->iodc = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 24, (unsigned char *)",", 25, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->toc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 25, (unsigned char *)",", 26, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->tgd = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 26, (unsigned char *)",", 27, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->af_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 27, (unsigned char *)",", 28, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->af_1 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 28, (unsigned char *)",", 29, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->af_2 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 29, (unsigned char *)",", 30, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->as = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 30, (unsigned char *)",", 31, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }

    if(strstr(buf, "TRUE") != NULL)
    {
        gps->as = 1;
    }
    else
    {
        gps->as = 0;
    }

    gps->n = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 31, (unsigned char *)"*", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gps->ura = atof(buf);
}

static void getBdsEphemeris(char *msg, int msg_len,struct BDSEPHEM *bds)
{
    int pos = 0;
    char buf[32] = {0};

    getEphemerisHeader(msg,&bds->header);

    pos = mystrstr((unsigned char *)msg, (unsigned char *)";", msg_len, 1);

    msg += pos;

    get_str1((unsigned char *)msg, (unsigned char *)";", 1, (unsigned char *)",", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->prn = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 1, (unsigned char *)",", 2, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->tow = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 2, (unsigned char *)",", 3, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->health = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 3, (unsigned char *)",", 4, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->iode_1 = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 4, (unsigned char *)",", 5, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->iode_2 = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 5, (unsigned char *)",", 6, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->week = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 6, (unsigned char *)",", 7, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->z_week = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 7, (unsigned char *)",", 8, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->toe = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 8, (unsigned char *)",", 9, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->a = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 9, (unsigned char *)",", 10, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->delat_n = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 10, (unsigned char *)",", 11, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->m_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 11, (unsigned char *)",", 12, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->ecc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 12, (unsigned char *)",", 13, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->omega = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 13, (unsigned char *)",", 14, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->cuc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 14, (unsigned char *)",", 15, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->cus = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 15, (unsigned char *)",", 16, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->crc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 16, (unsigned char *)",", 17, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->crs = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 17, (unsigned char *)",", 18, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->cic = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 18, (unsigned char *)",", 19, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->cis = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 19, (unsigned char *)",", 20, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->i_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 20, (unsigned char *)",", 21, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->idot = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 21, (unsigned char *)",", 22, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->omega_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 22, (unsigned char *)",", 23, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->omega_dot = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 23, (unsigned char *)",", 24, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->iodc = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 24, (unsigned char *)",", 25, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->toc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 25, (unsigned char *)",", 26, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->tgd_1 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 26, (unsigned char *)",", 27, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->tgd_2 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 27, (unsigned char *)",", 28, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->af_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 28, (unsigned char *)",", 29, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->af_1 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 29, (unsigned char *)",", 30, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->af_2 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 30, (unsigned char *)",", 31, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->as = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 31, (unsigned char *)",", 32, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }

    if(strstr(buf, "TRUE") != NULL)
    {
        bds->as = 1;
    }
    else
    {
        bds->as = 0;
    }

    bds->n = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 32, (unsigned char *)"*", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    bds->ura = atof(buf);
}

static void getGalEphemeris(char *msg, int msg_len,struct GALEPHEM *gal)
{
    int pos = 0;
    char buf[32] = {0};

    getEphemerisHeader(msg,&gal->header);

    pos = mystrstr((unsigned char *)msg, (unsigned char *)";", msg_len, 1);

    msg += pos;

    get_str1((unsigned char *)msg, (unsigned char *)";", 1, (unsigned char *)",", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->satid = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 1, (unsigned char *)",", 2, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->fnav_received = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 2, (unsigned char *)",", 3, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->inav_received = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 3, (unsigned char *)",", 4, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->e1b_health = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 4, (unsigned char *)",", 5, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->e5a_health = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 5, (unsigned char *)",", 6, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->e5b_health = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 6, (unsigned char *)",", 7, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->e1b_dvs = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 7, (unsigned char *)",", 8, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->e5a_dvs = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 8, (unsigned char *)",", 9, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->e5b_dvs = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 9, (unsigned char *)",", 10, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->sisa = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 11, (unsigned char *)",", 12, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->iodnav = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 12, (unsigned char *)",", 13, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->t0e = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 13, (unsigned char *)",", 14, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->a = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 14, (unsigned char *)",", 15, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->delat_n = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 15, (unsigned char *)",", 16, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->m_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 16, (unsigned char *)",", 17, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->ecc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 17, (unsigned char *)",", 18, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->omega = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 18, (unsigned char *)",", 19, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->cuc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 19, (unsigned char *)",", 20, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->cus = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 20, (unsigned char *)",", 21, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->crc = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 21, (unsigned char *)",", 22, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->crs = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 22, (unsigned char *)",", 23, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->cic = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 23, (unsigned char *)",", 24, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->cis = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 24, (unsigned char *)",", 25, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->i_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 25, (unsigned char *)",", 26, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->idot = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 26, (unsigned char *)",", 27, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->omega_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 27, (unsigned char *)",", 28, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->omega_dot = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 28, (unsigned char *)",", 29, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->fnavt0c = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 29, (unsigned char *)",", 30, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->fnavaf_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 30, (unsigned char *)",", 31, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->fnavaf_1 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 31, (unsigned char *)",", 32, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->fnavaf_2 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 32, (unsigned char *)",", 33, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->inavt0c = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 33, (unsigned char *)",", 34, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->inavaf_0 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 34, (unsigned char *)",", 35, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->inavaf_1 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 35, (unsigned char *)",", 36, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->inavaf_2 = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 36, (unsigned char *)",", 37, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->e1e5a_bgd = atof(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 37, (unsigned char *)"*", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return;
    }
    gal->e1e5b_bgd = atof(buf);
}

static int getRangehData(char *msg, int msg_len,struct Rangeh *rangeh)
{
    int pos = 0;
    int weeks = 0;
    double seconds = 0;
    char buf[32] = {0};
    unsigned char i = 0;

    get_str1((unsigned char *)msg, (unsigned char *)",", 5, (unsigned char *)",", 6, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return -1;
    }
    weeks = atoi(buf);

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)",", 6, (unsigned char *)",", 7, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return -1;
    }
    seconds = atof(buf);

    rangeh->time_stamp = (double)(weeks * 7 * 24 * 3600) + seconds;

    pos = mystrstr((unsigned char *)msg, (unsigned char *)";", msg_len, 1);

    msg += pos;

    memset(buf,0,32);
    get_str1((unsigned char *)msg, (unsigned char *)";", 1, (unsigned char *)",", 1, (unsigned char *)buf);
    if(strlen(buf) == 0)
    {
        return -1;
    }
    rangeh->satellite_num = atoi(buf);

    rangeh->data = NULL;

    rangeh->data = calloc(rangeh->satellite_num,sizeof(struct RangehData *));
    if(rangeh->data == NULL)
    {
        return -1;
    }

    for(i = 0; i < rangeh->satellite_num; i ++)
    {
        rangeh->data[i] = (struct RangehData *)malloc(sizeof(struct RangehData));

        if(rangeh->data[i] == NULL)
        {
            goto err_exit;
        }

        memset(buf,0,32);
        get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 1, (unsigned char *)",", i * 10 + 2, (unsigned char *)buf);
        if(strlen(buf) == 0)
        {
            return -1;
        }
        rangeh->data[i]->prn = atoi(buf);

        memset(buf,0,32);
        get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 2, (unsigned char *)",", i * 10 + 3, (unsigned char *)buf);
        if(strlen(buf) == 0)
        {
            return -1;
        }
        rangeh->data[i]->glofreq = atoi(buf);

        memset(buf,0,32);
        get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 3, (unsigned char *)",", i * 10 + 4, (unsigned char *)buf);
        if(strlen(buf) == 0)
        {
            return -1;
        }
        rangeh->data[i]->psr = atof(buf);

        memset(buf,0,32);
        get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 4, (unsigned char *)",", i * 10 + 5, (unsigned char *)buf);
        if(strlen(buf) == 0)
        {
            return -1;
        }
        rangeh->data[i]->psr_std = atof(buf);

        memset(buf,0,32);
        get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 5, (unsigned char *)",", i * 10 + 6, (unsigned char *)buf);
        if(strlen(buf) == 0)
        {
            return -1;
        }
        rangeh->data[i]->adr = atof(buf);

        memset(buf,0,32);
        get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 6, (unsigned char *)",", i * 10 + 7, (unsigned char *)buf);
        if(strlen(buf) == 0)
        {
            return -1;
        }
        rangeh->data[i]->adr_std = atof(buf);

        memset(buf,0,32);
        get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 7, (unsigned char *)",", i * 10 + 8, (unsigned char *)buf);
        if(strlen(buf) == 0)
        {
            return -1;
        }
        rangeh->data[i]->dopp = atof(buf);

        memset(buf,0,32);
        get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 8, (unsigned char *)",", i * 10 + 9, (unsigned char *)buf);
        if(strlen(buf) == 0)
        {
            return -1;
        }
        rangeh->data[i]->c_no = atof(buf);

        memset(buf,0,32);
        get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 9, (unsigned char *)",", i * 10 + 10, (unsigned char *)buf);
        if(strlen(buf) == 0)
        {
            return -1;
        }
        rangeh->data[i]->locktime = atof(buf);

        if(i < (rangeh->satellite_num - 1))
        {
            memset(buf,0,32);
            get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 10, (unsigned char *)",", i * 10 + 11, (unsigned char *)buf);
            if(strlen(buf) == 0)
            {
                return -1;
            }
            rangeh->data[i]->ch_tr_status = atoi(buf);
        }
        else
        {
            memset(buf,0,32);
            get_str1((unsigned char *)msg, (unsigned char *)",", i * 10 + 10, (unsigned char *)"*", 1, (unsigned char *)buf);
            if(strlen(buf) == 0)
            {
                return -1;
            }
            rangeh->data[i]->ch_tr_status = atoi(buf);
        }
    }
    return 0;

err_exit:
    for(i = 0; i < rangeh->satellite_num; i ++)
    {
        if(rangeh->data[i] != NULL)
        {
            free(rangeh->data[i]);
            rangeh->data[i] = NULL;
        }
    }

    free(rangeh->data);
    rangeh->data = NULL;

    return -1;
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

    ret = xQueueSend((key_t)KEY_UB482_TIME_STAMP_MSG,time_stamp,MAX_QUEUE_MSG_NUM);
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
            fprintf(stderr, "%s: recv : %s\n",__func__,recv_buf);
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

                    ret = xQueueSend((key_t)KEY_UB482_GPGGA_MSG,gpgga_msg,MAX_QUEUE_MSG_NUM);
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
            else
            {
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
                    struct Ub482GnssData *ub482_gnss_data = NULL;

                    ub482_gnss_data = (struct Ub482GnssData *)malloc(sizeof(struct Ub482GnssData));
                    if(ub482_gnss_data != NULL)
                    {
                        memcpy(ub482_gnss_data,&ub482GnssData,sizeof(struct Ub482GnssData));

                        ret = xQueueSend((key_t)KEY_GNSS_UB482_HANDLER_MSG,ub482_gnss_data,MAX_QUEUE_MSG_NUM);
                        if(ret == -1)
                        {
                            fprintf(stderr, "%s: send ub482_gnss_data queue msg failed\n",__func__);
                        }
                    }

                    sendTimeStampMsgToThreadSync();
                }
                else if(strstr(recv_buf, "#GLOEPHEMA") != NULL ||
                        strstr(recv_buf, "#GPSEPHEMERISA") != NULL ||
                        strstr(recv_buf, "#BDSEPHEMA") != NULL ||
                        strstr(recv_buf, "#GALEPHEMA") != NULL)
                {
                    struct Ephemeris *ephemeris = NULL;

                    ephemeris = (struct Ephemeris *)malloc(sizeof(struct Ephemeris));
                    if(ephemeris == NULL)
                    {
                        fprintf(stderr, "%s: malloc ephemeris failed\n",__func__);
                        return -1;
                    }

                    memset(ephemeris,0,sizeof(struct Ephemeris));

                    if(strstr(recv_buf, "#GLOEPHEMA") != NULL)
                    {
                        struct GLOEPHEM *glo = NULL;

                        glo = (struct GLOEPHEM *)malloc(sizeof(struct GLOEPHEM));
                        if(glo == NULL)
                        {
                            fprintf(stderr, "%s: malloc glo failed\n",__func__);
                            return -1;
                        }

                        memset(glo,0,sizeof(struct GLOEPHEM));

                        ephemeris->flag = 0x01;
                        ephemeris->ephem = (void *)glo;

                        getGlonssEphemeris(recv_buf,recv_len,glo);
                    }
                    else if(strstr(recv_buf, "#GPSEPHEMERISA") != NULL)
                    {
                        struct GPSEPHEM *gps = NULL;

                        gps = (struct GPSEPHEM *)malloc(sizeof(struct GPSEPHEM));
                        if(gps == NULL)
                        {
                            fprintf(stderr, "%s: malloc gps failed\n",__func__);
                            return -1;
                        }

                        memset(gps,0,sizeof(struct GPSEPHEM));

                        ephemeris->flag = 0x02;
                        ephemeris->ephem = (void *)gps;

                        getGpsEphemeris(recv_buf,recv_len,gps);
                    }
                    else if(strstr(recv_buf, "#BDSEPHEMA") != NULL)
                    {
                        struct BDSEPHEM *bds = NULL;

                        bds = (struct BDSEPHEM *)malloc(sizeof(struct BDSEPHEM));
                        if(bds == NULL)
                        {
                            fprintf(stderr, "%s: malloc bds failed\n",__func__);
                            return -1;
                        }

                        memset(bds,0,sizeof(struct BDSEPHEM));

                        ephemeris->flag = 0x04;
                        ephemeris->ephem = (void *)bds;

                        getBdsEphemeris(recv_buf,recv_len,bds);
                    }
                    else if(strstr(recv_buf, "#GALEPHEMA") != NULL)
                    {
                        struct GALEPHEM *gal = NULL;

                        gal = (struct GALEPHEM *)malloc(sizeof(struct GALEPHEM));
                        if(gal == NULL)
                        {
                            fprintf(stderr, "%s: malloc gal failed\n",__func__);
                            return -1;
                        }

                        memset(gal,0,sizeof(struct GALEPHEM));

                        ephemeris->flag = 0x08;
                        ephemeris->ephem = (void *)gal;

                        getGalEphemeris(recv_buf,recv_len,gal);
                    }

                    ret = xQueueSend((key_t)KEY_EPHEMERIS_MSG,ephemeris,MAX_QUEUE_MSG_NUM);
                    if(ret == -1)
                    {
                        fprintf(stderr, "%s: send ephemeris queue msg failed\n",__func__);
                    }
                }
                else if(strstr(recv_buf, "#RANGEA") != NULL)
                {
                    struct Rangeh *rangeh = NULL;

                    rangeh = (struct Rangeh *)malloc(sizeof(struct Rangeh));
                    if(rangeh != NULL)
                    {
                        ret = getRangehData(recv_buf, recv_len,rangeh);
                        if(ret == 0)
                        {
                            ret = xQueueSend((key_t)KEY_RANGEH_MSG,rangeh,MAX_QUEUE_MSG_NUM);
                            if(ret == -1)
                            {
                                fprintf(stderr, "%s: send rangeh queue msg failed\n",__func__);
                            }
                        }
                    }
                }
                else
                {
                    fprintf(stderr, "%s: invalid data frame\n",__func__);
                    return -1;
                }
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

        usleep(1000 * 100);
    }

THREAD_EXIT:
    pthread_exit("thread_ub482: open serial port failed\n");
}















