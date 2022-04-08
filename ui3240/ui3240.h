#ifndef __VIDEO_H
#define __VIDEO_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>    
#include <sys/stat.h>
#include <fcntl.h>
#include <ueye.h>
#include "common.h"
#include "cmd_parse.h"


#define UI3240_DEFAULT_CLOLOR_MODE_STRING          "mono8\0"
#define UI3240_MIN_IMAGE_WIDTH                     8
#define UI3240_MIN_IMAGE_HEIGHT                    4
#define UI3240_MIN_USER_CONFIG_FILE_LEN            24
#define UI3240_MAX_USER_CONFIG_FILE_LEN            1024

#define UI3240_MAX_CAPTURE_FAILED_CNT              5


struct Ui3240Config
{
    HIDS camera_handle;                     //相机控制句柄
    int camera_id;                          //相机设备id
    unsigned char master;                   //主镜头
    unsigned char stereo;                   //双镜头
    unsigned char capture_mode;             //获取图像模式，0：队列模式；1：事件等待模式
    unsigned short capture_timeout;         //获取图像超时ms
    SENSORINFO camera_sensor_info;          //相机传感器信息
    int image_width;                        //图像宽
    int image_height;                       //图像高
    int image_left;                         //左边距
    int image_top;                          //上边距
    unsigned short frame_num;               //缓冲区数量
    unsigned short image_heap_depth;        //提供给算法模块的堆深度
    char **frame_buf;                       //帧缓冲区
    int *frame_buf_id;                      //帧缓冲区id
    unsigned int frame_buf_size;            //缓冲区大小
    unsigned short color_mode;              //色彩模式
    unsigned char bits_per_pixel;           //每像素所占bits
    unsigned int subsampling;               //子采样模式(间隔采样)倍率
    unsigned int binning;                   //丢弃像素
    double sensor_scaling;                  //缩放
    bool auto_gain;                         //自动增益
    unsigned char master_gain;              //主增益
    unsigned char red_gain;                 //红增益
    unsigned char green_gain;               //绿增益
    unsigned char blue_gain;                //蓝增益
    bool gain_boost;                        //增益增强
    bool auto_exposure;                     //自动曝光模式
    double exposure;                        //手动曝光时间ms
    bool auto_white_balance;                //自动白平衡
    char white_balance_red_offset;          //白平衡红偏移
    char white_balance_blue_offset;         //白平衡蓝偏移
    bool auto_frame_rate;                   //自动采集帧率
    double frame_rate;                      //固定帧率Hz
    unsigned short pixel_clock;             //像素时钟MHz
    bool ext_trigger_mode;                  //外部触发模式
    unsigned short ext_trigger_delay;       //外部触发延时us
    int flash_delay;                        //闪光灯延时us
    unsigned int flash_duration;            //闪光灯持续时长us
    bool flip_upd;                          //图像上下翻转
    bool flip_lr;                           //图像左右翻转
};


void *thread_ui3240(void *arg);
void printUi3240Config(struct Ui3240Config *config);

#endif
