#include "sync_module.h"

// IMU协议格式：EB 90 00 [8字节本地时间戳] [8字节gnss时间戳] 40 79 [32字节IMU数据] [4字节counter]
// len = 3 + 8 + 8 + 2 + 32 + 4 = 57
// 时间戳协议格式：EB 90 0X [8字节本地时间戳] [8字节gnss时间戳] [4字节counter]
// len = 3 + 8 + 8 + 4 = 23
static enum SyncState syncState = SET_STOP;
static struct Serial serialSync;
static double frameRate = 30.0f;
static unsigned short syncHeapDepth = 0;


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
        fprintf(stderr, "%s: open %s failed\n",__func__,args->serial3);

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

static int syncParseImuData(unsigned char *inbuf,struct SyncImuData *imu_data)
{
    int ret = 0;
    unsigned short check_sum_cal = 0;
    unsigned short check_sum_recv = 0;
    static unsigned int last_counter = 0;
    unsigned int time_stamp = 0;

    if(*(inbuf + POS_IMU_HEAD + 0) != 0x40 || *(inbuf + POS_IMU_HEAD + 1) != 0x79)
    {
        fprintf(stderr, "%s: imu data head err,is not 0x40 0x79\n",__func__);
        return -1;
    }

    check_sum_cal = CalCheckSum(inbuf + POS_IMU_PAYLOAD, IMU_PAYLOAD_LEN - 2);
    check_sum_recv = ((((unsigned short)(*(inbuf + POS_IMU_CHECK + 0))) << 8) & 0xFF00) + 
                     ((((unsigned short)(*(inbuf + POS_IMU_CHECK + 1))) << 0) & 0x00FF);
    if(check_sum_cal != check_sum_recv)
    {
        fprintf(stderr, "%s: imu data check sum err,cal:%04X,recv:%04X\n",__func__,check_sum_cal,check_sum_recv);
        return -1;
    }

    imu_data->counter = ((((unsigned int)(*(inbuf + POS_IMU_CNT2 + 0))) << 24) & 0xFF000000) + 
                        ((((unsigned int)(*(inbuf + POS_IMU_CNT2 + 1))) << 16) & 0x00FF0000) + 
                        ((((unsigned int)(*(inbuf + POS_IMU_CNT2 + 2))) <<  8) & 0x0000FF00) + 
                        ((((unsigned int)(*(inbuf + POS_IMU_CNT2 + 3))) <<  0) & 0x000000FF);
    if(last_counter > 0)
    {
        if(imu_data->counter - last_counter != 1)
        {
            fprintf(stderr, "%s: imu data counter err,is not continous\n",__func__);
        }
    }

    last_counter = imu_data->counter;

    time_stamp = ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 3))) << 24) & 0xFF000000) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 2))) << 16) & 0x00FF0000) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 1))) <<  8) & 0x0000FF00) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 0))) <<  0) & 0x000000FF);

    imu_data->time_stamp_local = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    time_stamp = ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 3))) << 24) & 0xFF000000) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 2))) << 16) & 0x00FF0000) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 1))) <<  8) & 0x0000FF00) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 0))) <<  0) & 0x000000FF);

    imu_data->time_stamp_gnss = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    imu_data->gx = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 2))) << 24) & 0xFF000000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 3))) << 16) & 0x00FF0000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 0))) <<  8) & 0x0000FF00) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 1))) <<  0) & 0x000000FF);

    imu_data->gy = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 6))) << 24) & 0xFF000000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 7))) << 16) & 0x00FF0000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 4))) <<  8) & 0x0000FF00) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 5))) <<  0) & 0x000000FF);

    imu_data->gz = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 10))) << 24) & 0xFF000000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 11))) << 16) & 0x00FF0000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD +  8))) <<  8) & 0x0000FF00) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD +  9))) <<  0) & 0x000000FF);

    imu_data->ax = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 14))) << 24) & 0xFF000000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 15))) << 16) & 0x00FF0000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 12))) <<  8) & 0x0000FF00) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 13))) <<  0) & 0x000000FF);

    imu_data->ay = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 18))) << 24) & 0xFF000000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 19))) << 16) & 0x00FF0000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 16))) <<  8) & 0x0000FF00) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 17))) <<  0) & 0x000000FF);

    imu_data->az = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 22))) << 24) & 0xFF000000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 23))) << 16) & 0x00FF0000) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 20))) <<  8) & 0x0000FF00) + 
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 21))) <<  0) & 0x000000FF);

    imu_data->temperature = ((((unsigned short)(*(inbuf + POS_IMU_PAYLOAD + 24))) << 8) & 0xFF00) + 
                            ((((unsigned short)(*(inbuf + POS_IMU_PAYLOAD + 25))) << 0) & 0x00FF);

    return ret;
}

static int syncParseCamTimeStamp(unsigned char *inbuf,struct SyncCamTimeStamp *cam_time_stamp)
{
    int ret = 0;
    static unsigned int last_counter = 0;
    unsigned int time_stamp = 0;

    cam_time_stamp->counter = ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 0))) << 24) & 0xFF000000) + 
                               ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 1))) << 16) & 0x00FF0000) + 
                               ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 2))) <<  8) & 0x0000FF00) + 
                               ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 3))) <<  0) & 0x000000FF);
    if(last_counter > 0)
    {
        if(cam_time_stamp->counter - last_counter != 1)
        {
            fprintf(stderr, "%s: camare time stamp counter err,is not continous\n",__func__);
        }
    }

    last_counter = cam_time_stamp->counter;

    time_stamp = ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 3))) << 24) & 0xFF000000) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 2))) << 16) & 0x00FF0000) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 1))) <<  8) & 0x0000FF00) + 
                 ((((unsigned int)(*(inbuf + POS_LOCAL_TIME_STAMP + 0))) <<  0) & 0x000000FF);

    cam_time_stamp->time_stamp_local = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    time_stamp = ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 3))) << 24) & 0xFF000000) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 2))) << 16) & 0x00FF0000) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 1))) <<  8) & 0x0000FF00) + 
                 ((((unsigned int)(*(inbuf + POS_GNSS_TIME_STAMP + 0))) <<  0) & 0x000000FF);

    cam_time_stamp->time_stamp_gnss = (double)time_stamp / (double)FPGA_CLOCK_HZ;

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
                        pthread_mutex_lock(&mutexImuAdis16505Heap); 
                        ret = syncParseImuData(&recv_buf[head_pos],imuAdis16505Heap.heap[imuAdis16505Heap.put_ptr]);
                        if(ret != 0)
                        {
                            fprintf(stderr, "%s: parse imu data failed\n",__func__);
                        }
                        else
                        {
                            imuAdis16505Heap.put_ptr ++;
                            if(imuAdis16505Heap.put_ptr >= syncHeapDepth)
                            {
                                imuAdis16505Heap.put_ptr = 0;
                            }
                        }
                        pthread_mutex_unlock(&mutexImuAdis16505Heap);
                    }
                    else if(recv_buf[head_pos + 2] == 0x01)
                    {
                        struct SyncCamTimeStamp *time_stamp = NULL;

                        time_stamp = (struct SyncCamTimeStamp *)malloc(sizeof(struct SyncCamTimeStamp));
                        if(time_stamp != NULL)
                        {
                            ret = syncParseCamTimeStamp(&recv_buf[head_pos],time_stamp);
                            if(ret != 0)
                            {
                                fprintf(stderr, "%s: parse camera time stamp failed\n",__func__);
                            }
                            else
                            {
                                ret = xQueueSend((key_t)KEY_SYNC_CAM_TIME_STAMP_MSG,time_stamp);
                                if(ret == -1)
                                {
                                    fprintf(stderr, "%s: send sync camera time stamp queue msg failed\n",__func__);
                                }
                            }
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
    double *frame_rate;

    ret = xQueueReceive((key_t)KEY_FRAME_RATE_MSG,(void **)&frame_rate,0);
    if(ret == -1)
    {
        return -1;
    }

    fprintf(stderr, "%s: recv frame rate queue msg,frame_rate = %f\n",__func__,*frame_rate);

    if(*frame_rate < 0 || *frame_rate > 120)
    {
        fprintf(stderr, "%s: FrameRateMsg queue frame_rate error,frame_rate is %f,"
                         "but should be more than 0 and less than 120\n",__func__,*frame_rate);
        return -1;
    }

    frameRate = *frame_rate;

    free(frame_rate);
    frame_rate = NULL;

    return 0;
}

static int recvResetMsg(void)
{
    int ret = 0;
    unsigned char *reset = NULL;

    ret = xQueueReceive((key_t)KEY_SYNC_MODULE_RESET_MSG,(void **)&reset,0);
    if(ret == -1)
    {
        return -1;
    }

    free(reset);
    reset = NULL;

    return ret;
}

static int recvUb482TimeStampAndSendToSyncModule(void)
{
    int ret = 0;
    static double time_stamp = 0.0f;
    static double last_time_stamp = 0.0f;
    double *ub482_time_stamp = NULL;

    ret = xQueueReceive((key_t)KEY_UB482_TIME_STAMP_MSG,(void **)&ub482_time_stamp,0);
    if(ret == -1)
    {
        return -1;
    }

    fprintf(stderr, "%s: recv ub482 time stamp queue msg\n",__func__);

    time_stamp = *ub482_time_stamp;

    free(ub482_time_stamp);
    ub482_time_stamp = NULL;

    if(last_time_stamp != 0)
    {
        if(time_stamp - last_time_stamp >= 2)
        {
            fprintf(stderr, "%s: ub482 time stamp is not continous\n",__func__);
        }
    }

    last_time_stamp = time_stamp;

    ret = syncSetTimeStamp(time_stamp);
    if(ret < 0)
    {
        fprintf(stderr, "%s: send time stamp to sync module failed\n",__func__);
    }

    return ret;
}

void *thread_sync(void *arg)
{
    int ret = 0;
    struct CmdArgs *args = (struct CmdArgs *)arg;

    syncHeapDepth = args->sync_heap_depth;

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
                ret = recvFrameRateMsg();                   //接收帧率消息队列
                if(ret == 0)
                {
                    syncState = SET_X_HZ;
                }
                ret = recvResetMsg();                       //接收复位消息队列
                if(ret == 0)
                {
                    syncState = SET_STOP;
                }
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
























