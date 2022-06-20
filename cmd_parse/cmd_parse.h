#ifndef __CMD_PARSE_H
#define __CMD_PARSE_H

#include "serial.h"

struct CmdArgs
{
  const char *server;
  const char *port;
  const char *mount_point;
  const char *user_name;
  const char *password;

  const char *serial1;
  enum SerialBaudrate baudrate1;
  enum SerialDatabits databits1;
  enum SerialStopbits stopbits1;
  enum SerialParity parity1;
  enum SerialProtocol protocol1;

  const char *serial2;
  enum SerialBaudrate baudrate2;
  enum SerialDatabits databits2;
  enum SerialStopbits stopbits2;
  enum SerialParity parity2;
  enum SerialProtocol protocol2;

  const char *serial3;
  enum SerialBaudrate baudrate3;
  enum SerialDatabits databits3;
  enum SerialStopbits stopbits3;
  enum SerialParity parity3;
  enum SerialProtocol protocol3;

  unsigned char capture_mode;          //获取图像模式，0：队列模式；1：事件等待模式
  unsigned short capture_timeout;      //获取图像超时ms
  unsigned char frame_num;             //缓冲取数量

  int gyro_range;                      //0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
  int accel_range;                     //0,±2g;1,±4g;2,±8g;3,±16g
  int sample_rate;                     //4~1000(Hz)
  int read_mode;                       //0,被动读取;1,中断读取;2,定时器读取;1和2支持mmap;

  char *camera1;                       //mipi相机1
  char *camera2;                       //mipi相机2
  char *camera1_ctrl;                  //mipi相机1控制节点
  char *camera2_ctrl;                  //mipi相机2控制节点

  char *usb_cam_def_conf_file;         //usb相机默认配置文件
  char *usb_cam_user_conf_file;        //usb相机用户配置文件
  char *mipi_cam_user_conf_file;       //mipi相机用户配置文件

  unsigned char camera_module;         //相机型号,0:UI3420; 1:CSSC132
  unsigned short imu_heap_depth;       //imu堆深度
  unsigned short sync_heap_depth;      //同步模块imu堆深度
  unsigned short image_heap_depth;     //图像堆深度
  unsigned short gnss_heap_depth;      //gnss数据堆深度
  unsigned short ts_heap_depth;        //同步模块曝光时间戳堆深度
};


extern struct CmdArgs cmdArgs;


int cmdParse(int argc, char **argv, struct CmdArgs *args);

#endif