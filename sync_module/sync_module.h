#ifndef __SYNC_MODULE_H
#define __SYNC_MODULE_H


#define FPGA_CLOCK_HZ           20000000
#define MAX_SYNC_BUF_LEN        1024

#define POS_LOCAL_TIME_STAMP    0
#define POS_GNSS_TIME_STAMP     8
#define POS_IMU_HEAD            16
#define POS_IMU_PAYLOAD         18
#define POS_IMU_CNT1            46
#define POS_IMU_CHECK           48
#define POS_IMU_CNT2            50
#define IMU_PAYLOAD_LEN         32
#define POS_CAM_TIME_STAMP_CNT  16

#define IMU_DATA_LEN            54
#define CAM_TRIGGER_DATA_LEN    20


enum SyncState
{
    SET_STOP    = 0,            //停止触发
    SET_1_HZ    = 1,            //设置为1Hz触发
    SET_X_HZ    = 2,            //设置为xHz触发
    RUNNING     = 3,            //运行
};

enum ParserState {
    IDLE        = 0,
    SYNC1       = 1,
    SYNC2       = 2,
    IMU_DATA    = 3,
    CAM_DATA    = 4,
    COUNTER     = 5,
};

struct SyncImuData
{
    double time_stamp_local;
    double time_stamp_gnss;
    unsigned int counter;

    int gx;
    int gy;
    int gz;

    int ax;
    int ay;
    int az;

    unsigned short temperature;
};

struct SyncCamTimeStamp
{
    double time_stamp_local;
    double time_stamp_gnss;
    unsigned int counter;
    unsigned int number;
};


void *thread_sync_module(void *arg);

#endif
