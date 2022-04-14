#ifndef __UB482_H
#define __UB482_H


#define MAX_UB482_BUF_LEN		1024
#define PI                      3.141592653589793f

enum PositionVelocityType
{
    TYPE_NONE                = 0,
    TYPE_FIXEDPOS            = 1,
    TYPE_FIXEDHEIGHT         = 2,
    TYPE_DOPPLER_VELOCITY    = 8,
    TYPE_SINGLE              = 16,
    TYPE_PSRDIFF             = 17,
    TYPE_WAAS                = 18,
    TYPE_L1_FLOAT            = 32,
    TYPE_IONOFREE_FLOAT      = 33,
    TYPE_NARROW_FLOAT        = 34,
    TYPE_L1_INT              = 48,
    TYPE_WIDE_INT            = 49,
    TYPE_NARROW_INT          = 50,
    TYPE_INS                 = 52,
    TYPE_INS_PSRSP           = 53,
    TYPE_INS_PSRDIFF         = 54,
    TYPE_INS_RTKFLOA         = 55,
    TYPE_INS_RTKFIXED        = 56,
};

struct Ub482GnssData 
{
    double time_stamp;                      //时间戳
    enum PositionVelocityType pos_type;     //位置类型
    enum PositionVelocityType vel_type;     //位置速度
    enum PositionVelocityType att_type;     //位置姿态
    double lat;                             //纬度
    double lon;                             //经度
    double height;                          //高度
    double lat_std;                         //纬度误差
    double lon_std;                         //经度误差
    double height_std;                      //高度误差
    double vx;                              //东向速度分量
    double vy;                              //北向速度分量
    double vz;                              //天向速度分量
    double vx_std;                          //东向速度误差
    double vy_std;                          //北向速度误差
    double vz_std;                          //天向速度误差
    float pitch;                            //俯仰角
    float roll;                             //横滚角
    float yaw;                              //航向角
    float pitch_std;                        //俯仰角误差
    float roll_std;                         //横滚角误差
    float yaw_std;                          //航向角误差
};

extern enum PositionVelocityType gnssState;


void *thread_ub482(void *arg);

#endif
