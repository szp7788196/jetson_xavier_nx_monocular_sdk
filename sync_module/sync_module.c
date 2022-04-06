#include "sync_module.h"

// IMU协议格式：EB 90 00 [8字节本地时间戳] [8字节gnss时间戳] 40 79 [32字节IMU数据] [4字节counter]
// len = 3 + 8 + 8 + 2 + 32 + 4 = 57
// 时间戳协议格式：EB 90 0X [8字节本地时间戳] [8字节gnss时间戳] [4字节counter]
// len = 3 + 8 + 8 + 4 = 23
static enum SyncState syncState = SET_STOP;
static struct Serial serialSync;
static struct SyncImuData syncImuData;
static struct SyncCamTimeStamp syncCamTimeStamp;
static struct QueueMsgNormal FrameRateMsg;
static struct QueueMsgNormal ub482TimeStampMsg;
static struct QueueMsgNormal syncCamCounterMsg;
static int FrameRateMsgId = -1;
static int ub482TimeStampMsgId = -1;
static int syncCamCounterMsgId = -1;
static bool autoFrameRate = 0;
static double frameRate = 30.0f;


static int sync_serial_init(struct CmdArgs *args)
{
    int ret = 0;

    OPEN_SERIAL:
    ret = SerialInit(&serialSync, 
                     args->serial3,
                     args->baudrate3,
                     args->stopbits3,
                     args->protocol3,
                     args->parity3,
                     args->databits3,0);
    if(ret)
    {
        fprintf(stderr, "thread_sync: open %s failed\n",args->serial3);

        sleep(5);

        goto OPEN_SERIAL;
    }

    return ret;
}

static int syncSetCamTrigFreq(unsigned short freq_hz)
{
    int ret = 0;
    int i = 0;
    unsigned int  num = 0;
    unsigned char msg[7] = {0xEB, 0x90, 0x02, 0x00, 0x03, 0xD0, 0x8F};
    unsigned char tmp[4] = {0};

    num = FPGA_CLOCK_HZ / 2 / freq_hz - 1;

    memcpy(tmp, &num, sizeof(unsigned int));

    for(i = 0; i < 4; i ++) 
    {
        msg[3 + i] = tmp[3 - i];
    }

    ret = SerialWrite(&serialSync, (char *)msg, 7);

    return ret;
}

static int syncSetCamTrigStart(void)
{
    int ret = 0;
    unsigned char msg[3] = {0xEB, 0x90, 0x01};

    ret = SerialWrite(&serialSync, (char *)msg, 3);

    return ret;
}

static int syncSetCamTrigStop(void)
{
    int ret = 0;
    unsigned char msg[3] = {0xEB, 0x90, 0x00};

    ret = SerialWrite(&serialSync, (char *)msg, 3);

    return ret;
}

static int syncSetTimeStamp(double t)
{
    int ret = 0;
    int i = 0;
    unsigned char msg[11] = {0xEB, 0x90, 0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    unsigned char tmsg[8] = {0};
    unsigned long time = (unsigned long )(t * 1000.0f) * FPGA_CLOCK_HZ;

    memcpy(tmsg, &time, 8);

    for(i = 0; i < 8; i ++)
    {
      msg[3 + i] = tmsg[7 - i];
    }

    ret = SerialWrite(&serialSync, (char *)msg, 11);

    return ret;
}

static int syncParseImuData(unsigned char *inbuf)
{
    int ret = 0;
    unsigned short check_sum_cal = 0;
    unsigned short check_sum_recv = 0;
    static unsigned int last_counter = 0;
    unsigned int time_stamp = 0;

    if(*(inbuf + POS_IMU_HEAD + 0) != 0x40 || *(inbuf + POS_IMU_HEAD + 1) != 0x79)
    {
        fprintf(stderr, "imu data head err,is not 0x40 0x79\n");
        return -1;
    }

    check_sum_cal = CalCheckSum(inbuf + POS_IMU_PAYLOAD, IMU_PAYLOAD_LEN - 2);
    check_sum_recv = ((((unsigned short)(*(inbuf + POS_IMU_CHECK + 0))) << 8) & 0xFF00) + 
                     ((((unsigned short)(*(inbuf + POS_IMU_CHECK + 1))) << 0) & 0x00FF);
    if(check_sum_cal != check_sum_recv)
    {
        fprintf(stderr, "imu data check sum err,cal:%04X,recv:%04X\n",check_sum_cal,check_sum_recv);
        return -1;
    }

    syncImuData.counter = ((((unsigned int)(*(inbuf + POS_IMU_CNT2 + 0))) << 24) & 0xFF000000) + 
                          ((((unsigned int)(*(inbuf + POS_IMU_CNT2 + 1))) << 16) & 0x00FF0000) + 
                          ((((unsigned int)(*(inbuf + POS_IMU_CNT2 + 2))) <<  8) & 0x0000FF00) + 
                          ((((unsigned int)(*(inbuf + POS_IMU_CNT2 + 3))) <<  0) & 0x000000FF);
    if(last_counter > 0)
    {
        if(syncImuData.counter - last_counter != 1)
        {
            fprintf(stderr, "imu data counter err,is not continous\n");
        }
    }

    last_counter = syncImuData.counter;

    time_stamp = ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 3))) << 24) & 0xFF000000) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 2))) << 16) & 0x00FF0000) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 1))) <<  8) & 0x0000FF00) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 0))) <<  0) & 0x000000FF);

    syncImuData.time_stamp_local = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    time_stamp = ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 3))) << 24) & 0xFF000000) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 2))) << 16) & 0x00FF0000) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 1))) <<  8) & 0x0000FF00) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 0))) <<  0) & 0x000000FF);

    syncImuData.time_stamp_gnss = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    syncImuData.gx = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 2))) << 24) & 0xFF000000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 3))) << 16) & 0x00FF0000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 0))) <<  8) & 0x0000FF00) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 1))) <<  0) & 0x000000FF);

    syncImuData.gy = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 6))) << 24) & 0xFF000000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 7))) << 16) & 0x00FF0000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 4))) <<  8) & 0x0000FF00) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 5))) <<  0) & 0x000000FF);

    syncImuData.gz = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 10))) << 24) & 0xFF000000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 11))) << 16) & 0x00FF0000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD +  8))) <<  8) & 0x0000FF00) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD +  9))) <<  0) & 0x000000FF);

    syncImuData.ax = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 14))) << 24) & 0xFF000000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 15))) << 16) & 0x00FF0000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 12))) <<  8) & 0x0000FF00) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 13))) <<  0) & 0x000000FF);

    syncImuData.ay = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 18))) << 24) & 0xFF000000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 19))) << 16) & 0x00FF0000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 16))) <<  8) & 0x0000FF00) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 17))) <<  0) & 0x000000FF);

    syncImuData.az = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 22))) << 24) & 0xFF000000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 23))) << 16) & 0x00FF0000) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 20))) <<  8) & 0x0000FF00) + 
                     ((((int)(*(inbuf + POS_IMU_PAYLOAD + 21))) <<  0) & 0x000000FF);

    syncImuData.temperature = ((((unsigned short)(*(inbuf + POS_IMU_PAYLOAD + 24))) << 8) & 0xFF00) + 
                              ((((unsigned short)(*(inbuf + POS_IMU_PAYLOAD + 25))) << 0) & 0x00FF);

    return ret;
}

static int syncSendCamCounterToMainThread(unsigned int counter)
{
    int ret = 0;

    syncCamCounterMsg.mtype = 1;
    syncCamCounterMsg.mtext[0] = (char)((counter >> 24) & 0x000000FF);
    syncCamCounterMsg.mtext[1] = (char)((counter >> 16) & 0x000000FF);
    syncCamCounterMsg.mtext[2] = (char)((counter >> 16) & 0x000000FF);
    syncCamCounterMsg.mtext[3] = (char)((counter >> 16) & 0x000000FF);

    ret = msgsnd(syncCamCounterMsgId,&syncCamCounterMsg,sizeof(syncCamCounterMsg.mtext),0);
    if(ret == -1)
    {
        fprintf(stderr, "thread_sync: send syncCamCounterMsg failed\n");
    }

    return ret;
}

static int syncParseCamTimeStamp(unsigned char *inbuf)
{
    int ret = 0;
    static unsigned int last_counter = 0;
    unsigned int time_stamp = 0;

    syncCamTimeStamp.counter = ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 0))) << 24) & 0xFF000000) + 
                               ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 1))) << 16) & 0x00FF0000) + 
                               ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 2))) <<  8) & 0x0000FF00) + 
                               ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 3))) <<  0) & 0x000000FF);
    if(last_counter > 0)
    {
        if(syncCamTimeStamp.counter - last_counter != 1)
        {
            fprintf(stderr, "camare time stamp counter err,is not continous\n");
        }
    }

    last_counter = syncCamTimeStamp.counter;

    time_stamp = ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 3))) << 24) & 0xFF000000) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 2))) << 16) & 0x00FF0000) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 1))) <<  8) & 0x0000FF00) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 0))) <<  0) & 0x000000FF);

    syncCamTimeStamp.time_stamp_local = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    time_stamp = ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 3))) << 24) & 0xFF000000) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 2))) << 16) & 0x00FF0000) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 1))) <<  8) & 0x0000FF00) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 0))) <<  0) & 0x000000FF);

    syncCamTimeStamp.time_stamp_gnss = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    ret = syncSendCamCounterToMainThread(syncCamTimeStamp.counter);

    printf("------------------------------------------------------------------- camera counter = %d\n",syncCamTimeStamp.counter);

    return ret;
}

static void syncRecvAndParseMessage(void)
{
    int ret = 0;
    struct timeval tv;
    unsigned short head_pos = 0xFFFF;
    unsigned char frame_len = 0;
    static time_t time_sec = 0;
    static int recv_len = 0;
    static unsigned short recv_pos = 0;
    static unsigned char frame_head[2] = {0xEB,0x90};
    static unsigned char recv_buf[MAX_SYNC_BUF_LEN] = {0};

    recv_len = SerialRead(&serialSync, (char *)&recv_buf[recv_pos], MAX_SYNC_BUF_LEN - recv_pos);
    if(recv_len > 0)
    {
        gettimeofday(&tv,NULL);
        time_sec = tv.tv_sec;

        recv_pos = recv_pos + recv_len;
		recv_len = recv_pos;

        PARSE_LOOP:
        frame_len = 0;
        head_pos = mystrstr(recv_buf, frame_head, recv_len, 2);
        if(head_pos != 0xFFFF)
        {
            if(recv_buf[head_pos + 2] == 0x00)
            {
                frame_len = 57;
            }
            else if(recv_buf[head_pos + 2] == 0x01)
            {
                frame_len = 23;
            }
            
            if(frame_len)
            {
                if(frame_len <= recv_len)
                {
                    // do parse...
                    if(recv_buf[head_pos + 2] == 0x00)
                    {
                        ret = syncParseImuData(&recv_buf[head_pos]);
                        if(ret != 0)
                        {
                            fprintf(stderr, "parse imu data failed\n");
                        }
                    }
                    else if(recv_buf[head_pos + 2] == 0x01)
                    {
                        ret = syncParseCamTimeStamp(&recv_buf[head_pos]);
                        if(ret != 0)
                        {
                            fprintf(stderr, "parse camera time stamp failed\n");
                        }
                    }

                    if(recv_len > (frame_len + head_pos))
					{
						recv_len = recv_len - frame_len;
						recv_pos = recv_len;

						memcpy(recv_buf,&recv_buf[frame_len],recv_len);

						goto PARSE_LOOP;
					}
					else
					{
						recv_pos = 0;
						recv_len = 0;
						frame_len = 0;
					}
                }
            }
        }
    }

    if(recv_pos != 0 || recv_len != 0)
    {
        gettimeofday(&tv,NULL);
        if(tv.tv_sec - time_sec >= 1)
        {
            recv_pos = 0;
            recv_len = 0;
            frame_len = 0;
        }
    }
}

static int recvFrameRateMsg(void)
{
    int ret = 0;
    unsigned short msg_len = 0;
    bool auto_frame_rate = 0;
    double frame_rate;

    if(FrameRateMsgId == -1)
    {
        fprintf(stderr, "thread_sync: no FrameRateMsgId queue\n");
        return -1;
    }

    ret = msgrcv(FrameRateMsgId,&FrameRateMsg,sizeof(FrameRateMsg.mtext),1,IPC_NOWAIT);
    if(ret == -1)
    {
        return -1;
    }

    fprintf(stderr, "thread_sync: revc FrameRateMsg queue\n");

    msg_len = ((((unsigned short)FrameRateMsg.mtext[0]) << 8) & 0xFF00) + 
              ((unsigned short)FrameRateMsg.mtext[1] & 0x00FF);

    if(msg_len != 9)
    {
        fprintf(stderr, "thread_sync: FrameRateMsg queue len error,len is %d,but should be 9\n",msg_len);
        return -1;
    }

    if(FrameRateMsg.mtext[2] > 1)
    {
        fprintf(stderr, "thread_sync: FrameRateMsg queue autoFrameRate error,autoFrameRate is %d,"
                         "but should be 0 or 1\n",FrameRateMsg.mtext[2]);
        return -1;
    }

    autoFrameRate = (bool)FrameRateMsg.mtext[2];

    memcpy(&frame_rate,&FrameRateMsg.mtext[3],8);

    if(frame_rate < 0 || frame_rate > 120)
    {
        fprintf(stderr, "thread_sync: FrameRateMsg queue frame_rate error,frame_rate is %f,"
                         "but should be more than 0 and less than 120\n",frame_rate);
        return -1;
    }

    frameRate = frame_rate;

    if(auto_frame_rate == 0)
    {
        syncState = SET_X_HZ;
    }
    else
    {
        syncState = SET_STOP;
    }

    return 0;
}

static int recvUb482TimeStampAndSendToSyncModule(void)
{
    int ret = 0;
    unsigned short msg_len = 0;
    static double time_stamp = 0.0f;
    static double last_time_stamp = 0.0f;

    if(ub482TimeStampMsgId == -1)
    {
        fprintf(stderr, "thread_sync: no ub482TimeStampMsgId queue\n");
        return -1;
    }

    ret = msgrcv(ub482TimeStampMsgId,&ub482TimeStampMsg,sizeof(ub482TimeStampMsg.mtext),1,IPC_NOWAIT);
    if(ret == -1)
    {
        return -1;
    }

    fprintf(stderr, "thread_sync: revc ub482TimeStampMsg queue\n");

    msg_len = ((((unsigned short)ub482TimeStampMsg.mtext[0]) << 8) & 0xFF00) + 
              ((unsigned short)ub482TimeStampMsg.mtext[1] & 0x00FF);

    if(msg_len != 8)
    {
        fprintf(stderr, "thread_sync: ub482TimeStampMsg queue len error,len is %d,but should be 8\n",msg_len);
        return -1;
    }

    memcpy(&time_stamp,&ub482TimeStampMsg.mtext[2],8);

    if(last_time_stamp != 0)
    {
        if(time_stamp - last_time_stamp >= 2)
        {
            fprintf(stderr, "thread_sync: ub482 time stamp is not continous\n");
        }
    }

    last_time_stamp = time_stamp;

    ret = syncSetTimeStamp(time_stamp);
    if(ret < 0)
    {
        fprintf(stderr, "thread_sync: send time stamp to sync module failed\n");
    }

    return ret;
}

static void syncCreateMsgQueue(void)
{
    FrameRateMsgId = msgget((key_t)KEY_FRAME_RATE_MSG, IPC_CREAT | 0777);
    if(FrameRateMsgId == -1)
    {
        fprintf(stderr, "thread_sync: create FrameRateMsg failed\n");
    }
    ub482TimeStampMsgId = msgget((key_t)KEY_UB482_TIME_STAMP_MSG, IPC_CREAT | 0777);
    if(ub482TimeStampMsgId == -1)
    {
        fprintf(stderr, "thread_sync: create ub482TimeStampMsg failed\n");
    }

    syncCamCounterMsgId = msgget((key_t)KEY_SYNC_CAM_COUNTER_MSG, IPC_CREAT | 0777);
    if(syncCamCounterMsgId == -1)
    {
        fprintf(stderr, "thread_sync: create syncCamCounterMsg failed\n");
    }

    memset(&FrameRateMsg,0,sizeof(struct QueueMsgNormal));
    memset(&ub482TimeStampMsg,0,sizeof(struct QueueMsgNormal));
    memset(&syncCamCounterMsg,0,sizeof(struct QueueMsgNormal));
}

void *thread_sync(void *arg)
{
    int ret = 0;
    struct CmdArgs *args = (struct CmdArgs *)arg;

    syncCreateMsgQueue();

    ret = sync_serial_init(args);
    if(ret != 0)
    {
        goto THREAD_EXIT;
    }

    while(1)
    {
        switch((unsigned char)syncState)
        {
            case (unsigned char)SET_STOP:
                ret = syncSetCamTrigStop();
                if(ret >= 0)
                {
                    syncState = SET_1_HZ;
                }
            break;

            case (unsigned char)SET_1_HZ:
                ret = syncSetCamTrigStart();
                if(ret >= 0)
                {
                    syncState = RUNNING;
                }
            break;

            case (unsigned char)SET_X_HZ:

                ret = syncSetCamTrigFreq((unsigned short)(frameRate + 0.5f));
                if(ret >= 0)
                {
                    syncState = RUNNING;
                }

            break;

            case (unsigned char)RUNNING:
                recvFrameRateMsg();                         //接收帧率消息队列
                recvUb482TimeStampAndSendToSyncModule();    //接收UB482时间戳并发送至同步模块
                syncRecvAndParseMessage();                  //接收并解析同步模块的数据
            break;

            default:
            break;
        }

        usleep(1000 * 5);
    }

THREAD_EXIT:
    pthread_exit("thread_sync: open serial port failed\n");
}
























