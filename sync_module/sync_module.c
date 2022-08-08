#include "sync_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include "monocular.h"
#include "serial.h"
#include "cmd_parse.h"
#include "ringbuf.h"

// IMU协议格式：EB 90 00 [8字节本地时间戳] [8字节gnss时间戳] 40 79 [32字节IMU数据] [4字节counter]
// len = 3 + 8 + 8 + 2 + 32 + 4 = 57
// 时间戳协议格式：EB 90 0X [8字节本地时间戳] [8字节gnss时间戳] [4字节counter]
// len = 3 + 8 + 8 + 4 = 23
static enum SyncState syncState = SET_STOP;
static struct Serial serialSync;
static double frameRate = 30.0f;
static pthread_t tid_serial_recv = 0;
static unsigned int CamTimeStampNumber = 0;

static RingBuf ring_fifo;
static unsigned char rx_fifo[MAX_SYNC_BUF_LEN] = {0};


static int syncParseImuData(unsigned char *inbuf,struct SyncImuData *imu_data);
static int syncParseCamTimeStamp(unsigned char *inbuf,struct SyncCamTimeStamp *cam_time_stamp);


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
                     args->databits3,0,0);
    if(ret)
    {
        fprintf(stderr, "%s: open %s failed\n",__func__,args->serial3);

        sleep(5);

        goto OPEN_SERIAL;
    }

    ringbuf_init(&ring_fifo, rx_fifo, sizeof(rx_fifo));

    return ret;
}

static void *thread_serial_recv(void *arg)
{
    int ret = 0;
    fd_set rd;
    struct Serial *serial = (struct Serial *)arg;
    struct timeval val;
    char rxdata = 0;
    static enum ParserState parser_state = IDLE;

    while(1)
    {
        FD_ZERO(&rd);
        FD_SET(serial->Stream,&rd);
        val.tv_sec  = 2;
		val.tv_usec = 0;

        ret = select(serial->Stream + 1, &rd, NULL, NULL, &val);
        if(ret < 0)
        {
            fprintf(stderr, "%s: select failed\n",__func__);
        }
        else if(ret == 0)
        {
            fprintf(stderr, "%s: select timeout\n",__func__);
        }
        else
        {
            if(FD_ISSET(serial->Stream, &rd))
            {
                ret = read(serial->Stream, &rxdata, 1);
                if(ret == 1)
                {
                    switch(parser_state)
                    {
                        case IDLE:
                            if(rxdata == 0xEB)
                            {
                                parser_state = SYNC1;
                            }
                        break;

                        case SYNC1:
                            if(rxdata == 0x90)
                            {
                                parser_state = SYNC2;
                            }
                        break;

                        case SYNC2:
                            if(rxdata == 0x00)
                            {
                                parser_state = IMU_DATA;
                                ringbuf_clear(&ring_fifo);
                            }
                            else if(rxdata == 0x01)
                            {
                                parser_state = CAM_DATA;
                                ringbuf_clear(&ring_fifo);
                            }
                            else
                            {
                                parser_state = IDLE;
                            }
                        break;

                        case IMU_DATA:
                            ringbuf_put(&ring_fifo, rxdata);

                            if(ringbuf_elements(&ring_fifo) == IMU_DATA_LEN)
                            {
                                struct SyncImuData *sync_imu_data = NULL;

                                sync_imu_data = (struct SyncImuData *)malloc(sizeof(struct SyncImuData));
                                if(sync_imu_data != NULL)
                                {
                                    ret = syncParseImuData(ring_fifo.data,sync_imu_data);
                                    if(ret != 0)
                                    {
                                        free(sync_imu_data);
                                        sync_imu_data = NULL;

                                        fprintf(stderr, "%s: parse imu ads16505 data failed\n",__func__);
                                    }
                                    else
                                    {
                                        ret = xQueueSend((key_t)KEY_IMU_ADS16505_HANDLER_MSG,sync_imu_data,MAX_QUEUE_MSG_NUM);
                                        if(ret == -1)
                                        {
                                            fprintf(stderr, "%s: send sync_imu_data queue msg failed\n",__func__);
                                        }
                                    }
                                }

                                parser_state = IDLE;
                            }
                        break;

                        case CAM_DATA:
                            ringbuf_put(&ring_fifo, rxdata);

                            if(ringbuf_elements(&ring_fifo) == CAM_TRIGGER_DATA_LEN)
                            {
                                struct SyncCamTimeStamp *time_stamp = NULL;

                                time_stamp = (struct SyncCamTimeStamp *)malloc(sizeof(struct SyncCamTimeStamp));
                                if(time_stamp != NULL)
                                {
                                    ret = syncParseCamTimeStamp(ring_fifo.data,time_stamp);
                                    if(ret != 0)
                                    {
                                        free(time_stamp);
                                        time_stamp = NULL;

                                        fprintf(stderr, "%s: parse camera time stamp failed\n",__func__);
                                    }
                                    else
                                    {
                                        syncCamTimeStampHeapPut(time_stamp);

                                        sem_post(&sem_t_SyncCamTimeStampHeap);

                                        ret = xQueueSend((key_t)KEY_SYNC_CAM_TIME_STAMP_MSG,time_stamp,MAX_QUEUE_MSG_NUM);
                                        if(ret == -1)
                                        {
                                            fprintf(stderr, "%s: send time_stamp queue msg failed\n",__func__);
                                        }
                                    }
                                }

                                parser_state = IDLE;
                            }
                        break;

                        default:
                        break;
                    }
                }
            }
        }
    }
}

static int syncSetCamTrigFreq(unsigned short freq_hz)
{
    int ret = 0;
    int i = 0;
    unsigned int  num = 0;
    unsigned char msg[7] = {0xEB, 0x90, 0x02, 0x00, 0x03, 0xD0, 0x8F};
    unsigned char tmp[4] = {0};
    unsigned int freq = (unsigned int)freq_hz;

    // num = FPGA_CLOCK_HZ / 2 / freq_hz - 1;

    num = freq;

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
    // unsigned long time = (unsigned long )(t * 1000.0f) * FPGA_CLOCK_HZ;
    unsigned long time = (unsigned long )t * FPGA_CLOCK_HZ;

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
    unsigned long time_stamp = 0;
    int i = 0;

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
            fprintf(stderr, "%s: imu data counter err,is not continous,current = %d,last = %d\n",__func__,imu_data->counter,last_counter);
        }
    }

    last_counter = imu_data->counter;

    time_stamp = ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 0))) << 56) & 0xFF00000000000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 1))) << 48) & 0x00FF000000000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 2))) << 40) & 0x0000FF0000000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 3))) << 32) & 0x000000FF00000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 4))) << 24) & 0x00000000FF000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 5))) << 16) & 0x0000000000FF0000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 6))) <<  8) & 0x000000000000FF00) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 7))) <<  0) & 0x00000000000000FF);

    imu_data->time_stamp_local = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    time_stamp = ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 0))) << 56) & 0xFF00000000000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 1))) << 48) & 0x00FF000000000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 2))) << 40) & 0x0000FF0000000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 3))) << 32) & 0x000000FF00000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 4))) << 24) & 0x00000000FF000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 5))) << 16) & 0x0000000000FF0000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 6))) <<  8) & 0x000000000000FF00) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 7))) <<  0) & 0x00000000000000FF);

    imu_data->time_stamp_gnss = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    imu_data->gx = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 4))) << 24) & 0xFF000000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 5))) << 16) & 0x00FF0000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 2))) <<  8) & 0x0000FF00) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 3))) <<  0) & 0x000000FF);

    imu_data->gy = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 8))) << 24) & 0xFF000000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 9))) << 16) & 0x00FF0000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 6))) <<  8) & 0x0000FF00) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 7))) <<  0) & 0x000000FF);

    imu_data->gz = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 12))) << 24) & 0xFF000000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 13))) << 16) & 0x00FF0000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 10))) <<  8) & 0x0000FF00) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 11))) <<  0) & 0x000000FF);

    imu_data->ax = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 16))) << 24) & 0xFF000000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 17))) << 16) & 0x00FF0000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 14))) <<  8) & 0x0000FF00) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 15))) <<  0) & 0x000000FF);

    imu_data->ay = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 20))) << 24) & 0xFF000000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 21))) << 16) & 0x00FF0000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 18))) <<  8) & 0x0000FF00) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 19))) <<  0) & 0x000000FF);

    imu_data->az = ((((int)(*(inbuf + POS_IMU_PAYLOAD + 24))) << 24) & 0xFF000000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 25))) << 16) & 0x00FF0000) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 22))) <<  8) & 0x0000FF00) +
                   ((((int)(*(inbuf + POS_IMU_PAYLOAD + 23))) <<  0) & 0x000000FF);

    imu_data->temperature = ((((unsigned short)(*(inbuf + POS_IMU_PAYLOAD + 26))) << 8) & 0xFF00) +
                            ((((unsigned short)(*(inbuf + POS_IMU_PAYLOAD + 27))) << 0) & 0x00FF);

    return ret;
}

static int syncParseCamTimeStamp(unsigned char *inbuf,struct SyncCamTimeStamp *cam_time_stamp)
{
    int ret = 0;
    static unsigned int last_counter = 0;
    static double last_time_stamp_local = 0.0f;
    unsigned long time_stamp = 0;
    unsigned char i = 0;

    cam_time_stamp->counter = ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 0))) << 24) & 0xFF000000) +
                              ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 1))) << 16) & 0x00FF0000) +
                              ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 2))) <<  8) & 0x0000FF00) +
                              ((((unsigned int)(*(inbuf + POS_CAM_TIME_STAMP_CNT + 3))) <<  0) & 0x000000FF);
    if(last_counter > 0)
    {
        if(cam_time_stamp->counter - last_counter != 1)
        {
            fprintf(stderr, "%s: camera time stamp counter err,is not continous,current = %d,last = %d\n",__func__,cam_time_stamp->counter,last_counter);
        }
    }

    last_counter = cam_time_stamp->counter;

    time_stamp = ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 0))) << 56) & 0xFF00000000000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 1))) << 48) & 0x00FF000000000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 2))) << 40) & 0x0000FF0000000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 3))) << 32) & 0x000000FF00000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 4))) << 24) & 0x00000000FF000000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 5))) << 16) & 0x0000000000FF0000) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 6))) <<  8) & 0x000000000000FF00) +
                 ((((unsigned long)(*(inbuf + POS_LOCAL_TIME_STAMP + 7))) <<  0) & 0x00000000000000FF);

    cam_time_stamp->time_stamp_local = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    last_time_stamp_local = cam_time_stamp->time_stamp_local;

    time_stamp = ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 0))) << 56) & 0xFF00000000000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 1))) << 48) & 0x00FF000000000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 2))) << 40) & 0x0000FF0000000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 3))) << 32) & 0x000000FF00000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 4))) << 24) & 0x00000000FF000000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 5))) << 16) & 0x0000000000FF0000) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 6))) <<  8) & 0x000000000000FF00) +
                 ((((unsigned long)(*(inbuf + POS_GNSS_TIME_STAMP + 7))) <<  0) & 0x00000000000000FF);

    cam_time_stamp->time_stamp_gnss = (double)time_stamp / (double)FPGA_CLOCK_HZ;

    cam_time_stamp->number = CamTimeStampNumber ++;

/*     if(cam_time_stamp->number == 1000)
    {
        syncSetCamTrigStop();
    } */

    // fprintf(stderr, "%s: time_stamp ======================================= %lf\n",__func__,cam_time_stamp->time_stamp_local);
    // fprintf(stdout, "%s: cam_time_stamp->number ======================== %d\n",__func__,cam_time_stamp->number);

    return ret;
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

static int recvCameraReadyMsg(void)
{
    int ret = 0;
    char *ready = NULL;

    ret = xQueueReceive((key_t)KEY_CAMERA_READY_MSG,(void **)&ready,0);
    if(ret == -1)
    {
        return -1;
    }

    free(ready);
    ready = NULL;

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

void *thread_sync_module(void *arg)
{
    int ret = 0;
    struct CmdArgs *args = (struct CmdArgs *)arg;

    ret = sync_serial_init(args);
    if(ret != 0)
    {
        goto THREAD_EXIT;
    }

    allocateSyncCamTimeStampHeap(args->ts_heap_depth);

    ret = pthread_create(&tid_serial_recv,NULL,thread_serial_recv,&serialSync);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_serial_recv failed\n",__func__);
    }

    while(1)
    {
        switch((unsigned char)syncState)
        {
            case (unsigned char)SET_STOP:
                    syncSetCamTrigStop();

                    usleep(1000 * 1500);

                    CamTimeStampNumber = 0;

                    syncState = SET_1_HZ;
            break;

            case (unsigned char)SET_1_HZ:
                ret = syncSetCamTrigStart();
                //ret = syncSetCamTrigFreq((unsigned short)(5 + 0.5f));
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
                recvUb482TimeStampAndSendToSyncModule();    //接收UB482时间戳并发送至同步模块

                ret = recvFrameRateMsg();                   //接收帧率消息队列
                if(ret == 0)
                {
                    syncState = SET_X_HZ;
                }
                ret = recvResetMsg();                       //接收复位消息队列
                if(ret != -1)
                {
                    syncState = SET_STOP;
                }
            break;

            default:
            break;
        }

        usleep(1000 * 10);
    }

THREAD_EXIT:
    pthread_exit("thread_sync: open serial port failed\n");
}
