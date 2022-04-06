#ifndef __SYNC_MODULE_H
#define __SYNC_MODULE_H

#include <stdbool.h>
#include <sys/time.h>
#include "common.h"
#include "serial.h"
#include "cmd_parse.h"


#define FPGA_CLOCK_HZ       20000000
#define MAX_SYNC_BUF_LEN    1024

#define POS_LOCAL_TIME_STAMP    3
#define POS_GNSS_TIME_STAMP     11
#define POS_IMU_HEAD            19
#define POS_IMU_PAYLOAD         21
#define POS_IMU_CNT1            49
#define POS_IMU_CHECK           51
#define POS_IMU_CNT2            53
#define IMU_PAYLOAD_LEN         32
#define POS_CAM_TIME_STAMP_CNT  19


enum SyncState
{
    SET_STOP    = 0,            //停止触发
    SET_1_HZ    = 1,            //设置为1Hz触发
    SET_X_HZ    = 2,            //设置为xHz触发
    RUNNING     = 3,            //运行
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
};


void *thread_sync(void *arg);




















#endif
