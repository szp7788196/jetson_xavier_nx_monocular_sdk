#ifndef __MPU9250_H
#define __MPU9250_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include "cmd_parse.h"
#include "monocular.h"


/* 定义设备类型 */
#define IOC_MPU9250_MAGIC           'M'
/* 初始化设备 */
#define IOC_MPU9250_INIT            _IO(IOC_MPU9250_MAGIC, 0)
/* 读寄存器 */
#define IOC_R_MPU9250_DATA          _IOR(IOC_MPU9250_MAGIC, 1, int)
/* 写寄存器 */
#define IOC_W_MPU9250_CONFIG        _IOW(IOC_MPU9250_MAGIC, 2, int)
#define IOC_MPU9250_MAX_NUM         3


struct Mpu9250SampleData
{
	short accel_x;
    short accel_y;
    short accel_z;
    short temperature;
    short gyro_x;
    short gyro_y;
    short gyro_z;
    short magne_x;
    short magne_y;
    short magne_z;

};

struct Mpu9250Config
{
    int gyro_range;         //:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
    int accel_range;        //0,±2g;1,±4g;2,±8g;3,±16g
    int sample_rate;        //4~1000(Hz)
    int read_mode;          //0,被动读取;1,中断读取;2,定时器读取;1和2支持mmap;

};


void *thread_mpu9250(void *config);

#endif
