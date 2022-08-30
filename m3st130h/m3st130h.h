#ifndef __M3ST130H_H
#define __M3ST130H_H

#include "DVPCamera.h"

#define M3ST130H_MAX_CAPTURE_FAILED_CNT                 5
#define M3ST130H_MIN_USER_CONFIG_FILE_LEN               24
#define M3ST130H_MAX_USER_CONFIG_FILE_LEN               2048

#define M3ST130H_MAX_ROI_W                              1280
#define M3ST130H_MAX_ROI_H                              1024
#define M3ST130H_MIN_AUTO_EXP_TIME                      1
#define M3ST130H_MAX_AUTO_EXP_TIME                      145305
#define M3ST130H_MIN_AUTO_EXP_GAIN                      1
#define M3ST130H_MAX_AUTO_EXP_GAIN                      3
#define M3ST130H_MIN_RGB_GAIN                           0
#define M3ST130H_MAX_RGB_GAIN                           4

struct M3st130hConfig
{
    dvpHandle camera_handle;                                    //相机控制句柄
    unsigned int camera_id;                                     //相机设备id
    dvpCameraInfo camera_info;                                  //相机详细信息
    unsigned short capture_timeout;                             //获取图像超时ms
    unsigned short frame_num;                                   //缓冲区数量
    unsigned short image_heap_depth;                            //提供给算法模块的堆深度
    unsigned short image_width;                                 //图像宽度
    unsigned short image_height;                                //图像高度
    unsigned int image_size;                                    //图像尺寸
    unsigned char bytes_per_pix;                                //每个像素的字节数
    unsigned int frame_buf_size;                                //缓冲区大小
    double trigger_frame_rate;                                  //触发帧率

    // 曝光功能
    unsigned int auto_exposure_target_brightness;               //自动曝光模式下目标亮度
    float analog_gain;                                          //模拟增益
    double exposure_time;                                       //曝光时间us
    dvpRegion exposure_roi;                                     //自动曝光统计区域
    dvpAeMode auto_exposure_mode;                               //自动曝光模式
    dvpAntiFlick anti_flick;                                    //抗频闪状态
    dvpAeOperation auto_exposure_operate_mode;                  //自动曝光操作方式
    dvpAeConfig auto_exposure_conf_info;                        //自动曝光配置信息

    // 色彩调节
    unsigned int color_temperature;                             //色温K
    bool color_temperature_state;                               //色温功能的使能状态
    unsigned int saturation;                                    //饱和度
    bool saturation_state;                                      //饱和度的使能状态
    dvpRegion awb_roi;                                          //自动白平衡统计区域
    dvpAwbOperation awb_operate;                                //自动白平衡的操作方式
    float red_digital_gain;                                     //红色数字增益
    float green_digital_gain;                                   //绿色数字增益
    float blue_digital_gain;                                    //蓝色数字增益
    bool rgb_digital_gain_state;                                //红绿蓝数字增益的使能状态

    // 增强效果
    unsigned int gamma;                                         //伽马值
    bool gamma_state;                                           //伽马的使能状态
    unsigned int contrast;                                      //对比度
    bool contrast_state;                                        //对比度的使能状态
    unsigned int sharpness;                                     //锐度
    bool sharpness_state;                                       //锐度的使能状态
    unsigned int noise_reduct_2d;                               //2D降噪的值
    bool noise_reduct_2d_state;                                 //2D降噪的使能状态
    unsigned int noise_reduct_3d;                               //3D降噪的值
    bool noise_reduct_3d_state;                                 //3D降噪的使能状态
    float black_level;                                          //黑场的值
    bool black_level_state;                                     //黑场功能的使能状态
    bool mono_state;                                            //设置去色的使能状态
    bool inverse_state;                                         //负片的使能状态
    bool flip_horizontal_state;                                 //横向翻转的使能状态
    bool flip_vertical_state;                                   //纵向翻转的使能状态
    bool rotate_state;                                          //图像旋转使能状态
    bool rotate_opposite;                                       //图像旋转方向的标志

    // 图像尺寸
    dvpRegion roi;                                              //roi
    bool roi_state;                                             //roi的使能状态

    // 图像格式
    dvpStreamFormat source_format;                              //原图像格式
    dvpStreamFormat target_format;                              //目标像格式

    // 触发功能
    double soft_trigger_loop_time;                              //软件循环触发的周期值us
    bool soft_trigger_loop_state;                               //软件循环触发使能状态
    double ext_trigger_jitter_filter;                           //外部触发信号消抖时间的值us
    double ext_trigger_delay;                                   //外部触发延时时间us
    double strobe_delay;                                        //strobe信号的延时时间us
    double strobe_duration;                                     //strobe信号的持续时间
    unsigned int frames_per_trigger;                            //单次触发输出帧数
    dvpTriggerSource ext_trigger_source;                        //相机外部触发源
    bool trigger_state;                                         //触发的使能状态
    dvpStrobeDriver strobe_driver;                              //strobe信号的驱动方式
    dvpStrobeOutputType strobe_output_type;                     //strobe输出信号类型
    dvpTriggerInputType ext_trigger_input_type;                 //触发输入信号类型
};


void *thread_m3st130h(void *arg);


#endif
