#ifndef __UB482_H
#define __UB482_H


#define MAX_UB482_BUF_LEN		10240
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

struct EphemHeader
{
    unsigned char cpu_idle;         //处理器空闲时间的最小百分比，每秒计算 1 次
    unsigned char time_ref;         //接收机工作的时间系统（GPST or BDST）
    unsigned char time_status;      //GPS 时间质量。当前取值 Unknown 或 Fine，前者表明接收机还未能计算出准确的 GPS 时间。
    unsigned short week;            //GPS 周数
    unsigned int time;              //GPS 周内秒，精确到 ms
    unsigned char version;          //Unicore 格式版本号保留字段
    unsigned char leap_sec;         //闰秒
    unsigned short output_delay;    //数据输出时间延迟（数据输出与 GNSS 卫星信号采样时间差），单位： μs
};

struct GLOEPHEM                     //GLONASS星历数据
{
    struct EphemHeader header;      //头信息
    unsigned short sloto;           //通道编号，转换为 PRN 号是（Slot + 37）
    unsigned char freqo;            //频率编号，范围为 0 到 20
    unsigned char sat_type;         //卫星类型 0 = GLO_SAT;1 = GLO_SAT_M （M 型卫星）
    unsigned short week;            //星历参考时刻，整周数（GPS Week）
    unsigned int time;              //星历参考时刻 ， ms（相对于GPS 时间）
    unsigned int t_offset;          //GPS 和 GLONAS 时间之间的整数秒。正值表明 GLONASS 时间先于 GPS 时间。
    unsigned short nt;              //当前天数，从每个闰年一月的第一天开始的天计数。
    unsigned int issue;             //相对星历参考时刻的 15 分钟间隔数
    unsigned char health;           //星历健康 0 = GOODl;1 = BAD
    double pos_x;                   //参考时刻卫星的 X 坐标（ PZ-90.02）， m
    double pos_y;                   //参考时刻卫星的 Y 坐标（ PZ-90.02）， m
    double pos_z;                   //参考时刻卫星的 Z 坐标（ PZ-90.02）， m
    double vel_x;                   //参考时刻卫星速度的 X 坐标（PZ-90.02）， m/s
    double vel_y;                   //参考时刻卫星速度的 Y 坐标（PZ-90.02）， m/s
    double vel_z;                   //参考时刻卫星速度的 Z 坐标（PZ-90.02）， m/s
    double ls_acc_x;                //参考时刻日月摄动加速度的 X 坐标（PZ-90.02）， m/s2
    double ls_acc_y;                //参考时刻日月摄动加速度的 Y 坐标（PZ-90.02）， m/s2
    double ls_acc_z;                //参考时刻日月摄动加速度的 Z 坐标（PZ-90.02）， m/s2
    double tau_n;                   //修正第 n个相对于 GLONASS时间 t_c 的卫星时间 t_n， s
    double delat_tau_n;             //第 n 个卫星的 L2 RF 信号相对于L1 RF 信号的传输延迟， s
    double gamma;                   //频率校正， s/s
    unsigned int tk;                //帧起始时刻（从 GLONASS 日开始）， s
    unsigned int p;                 //技术参数
    unsigned int ft;                //用户测距精度预测
    unsigned int age;               //数据龄期， day
    unsigned int flags;             //信息标识
};

struct GPSEPHEM                     //GPS星历数据
{
    struct EphemHeader header;      //头信息
    unsigned char prn;              //卫星 PRN 编号（GPS： 1 到 32）
    float tow;                      //子帧 0 的时间戳， s
    unsigned int health;            //健康状态-ICD-GPS-200a 中定义的6位健康代码
    unsigned int iode_1;            //星历数据 1 龄期
    unsigned int iode_2;            //星历数据 2 龄期 = GPS 的 IODE1
    unsigned int week;              //GPS 周数（GPS Week）
    unsigned int z_week;            //Z 计数的周数，为星历表的子帧 1的周数。 “TOW 周”（字段#7）来源于此
    double toe;                     //星历的参考时间， s
    double a;                       //卫星轨道长半轴， m
    double delat_n;                 //卫星平均角速度的改正值， rad/s
    double m_0;                     //TOE 时间的平近点角， rad
    double ecc;                     //卫星轨道偏心率
    double omega;                   //近地点幅角， rad
    double cuc;                     //纬度幅角（余弦振幅， rad）
    double cus;                     //纬度幅角（正弦振幅， rad）
    double crc;                     //轨道半径（余弦振幅， m）
    double crs;                     //轨道半径（正弦振幅， m）
    double cic;                     //倾角（余弦振幅， rad）
    double cis;                     //倾角（正弦振幅， rad）
    double i_0;                     //TOE 时间轨道倾角， rad
    double idot;                    //轨道倾角变化率， rad/s
    double omega_0;                 //升交点赤经， rad
    double omega_dot;               //升交点赤经变化率， rad/s
    unsigned int iodc;              //时钟数据龄期
    double toc;                     //卫星钟差参考时间， s
    double tgd;                     //群延迟， s
    double af_0;                    //卫星钟差参数， s
    double af_1;                    //卫星钟速参数， s/s
    double af_2;                    //卫星钟漂参数， s/s/s
    unsigned char as;               //反欺骗：0 = FALSE;1 = TRUE
    double n;                       //改正平均角速度， rad/s
    double ura;                     //用户距离精度， m2。ICD 中给出了一种算法将原始星历中传输的URAI指数转化为名义标准差值。我们输出这一名义值的平方（方差）。
};

struct BDSEPHEM                     //北斗星历数据
{
    struct EphemHeader header;      //头信息
    unsigned char prn;              //卫星 PRN 编号（ BDS： 1 到63）
    double tow;                     //子帧1的时间标识（基于GPS时间）， s
    unsigned int health;            //健康状态–在北斗 ICD 中定义的一个 1 比特的健康代码
    unsigned int iode_1;            //星历数据龄期
    unsigned int iode_2;            //星历数据龄期
    unsigned int week;              //GPS 周计数（GPS Week）
    unsigned int z_week;            //基于 GPS 周的 Z 计数周数，为星历子帧 1 的周数。
    double toe;                     //星历参考时刻（基于 GPS 时间）， s
    double a;                       //轨道长半轴， m
    double delat_n;                 //卫星平均角速度的改正值 ，rad/s
    double m_0;                     //参考时间的平近点角， rad
    double ecc;                     //偏心率
    double omega;                   //近地点幅角， rad
    double cuc;                     //纬度幅角（余弦振幅， rad）
    double cus;                     //纬度幅角（正弦振幅， rad）
    double crc;                     //轨道半径（余弦振幅， m）
    double crs;                     //轨道半径（正弦振幅， m）
    double cic;                     //倾角（余弦振幅， rad）
    double cis;                     //倾角（正弦振幅， rad）
    double i_0;                     //TOE 时间轨道倾角， rad
    double idot;                    //轨道倾角变化率， rad/s
    double omega_0;                 //升交点赤经， rad
    double omega_dot;               //升交点赤经变化率， rad/s
    unsigned int iodc;              //时钟数据龄期
    double toc;                     //卫星钟差参考时间， s
    double tgd_1;                   //B1 群延迟（B1 星上设备时延差）， s
    double tgd_2;                   //B2 群延迟（B2 星上设备时延差）， s
    double af_0;                    //卫星钟差参数， s
    double af_1;                    //卫星钟速参数， s/s
    double af_2;                    //卫星钟漂参数， s/s/s
    unsigned char as;               //反欺骗：0 = FALSE;1 = TRUE
    double n;                       //改正平均角速度， rad/s
    double ura;                     //用户距离精度， m2。ICD 中给出了一种算法将原始星历中传输的URAI指数转化为名义标准差值。我们输出这一名义值的平方（方差）。
};

struct GALEPHEM                     //伽利略星历数据
{
    struct EphemHeader header;      //头信息
    unsigned char satid;            //卫星 ID 编号
    unsigned char fnav_received;    //接收到FNAV星历数据的标识
    unsigned char inav_received;    //接收到INAV星历数据的标识
    unsigned char e1b_health;       //E1b健康状态 （ 当INAVReceived 值为“真”时有效）
    unsigned char e5a_health;       //E1b健康状态 （ 当FNAVReceived 值为“真”时有效）
    unsigned char e5b_health;       //E5b健康状态 （ 当FNAVReceived 值为“真”时有效）
    unsigned char e1b_dvs;          //E1b数据有效状态 （ 当INAVReceived 值为“真”时有效）
    unsigned char e5a_dvs;          //E5a数据有效状态 （ 当FNAVReceived 值为“真”时有效）
    unsigned char e5b_dvs;          //E5b数据有效状态 （ 当INAVReceived 值为“真”时有效）
    unsigned char sisa;             //空间信号精度
    unsigned int iodnav;            //星历数据期号
    unsigned int t0e;               //星历的参考时间，单位：秒
    double a;                       //卫星轨道长半轴 （ 根数）， m
    double delat_n;                 //卫星平均角速度的改正值， rad/s
    double m_0;                     //参考时间的平近点角， rad
    double ecc;                     //偏心率
    double omega;                   //近地点幅角， rad
    double cuc;                     //纬度幅角（余弦振幅， rad）
    double cus;                     //纬度幅角（正弦振幅， rad）
    double crc;                     //轨道半径（余弦振幅， m）
    double crs;                     //轨道半径（正弦振幅， m）
    double cic;                     //倾角（余弦振幅， rad）
    double cis;                     //倾角（正弦振幅， rad）
    double i_0;                     //TOE 时间轨道倾角， rad
    double idot;                    //轨道倾角变化率， rad/s
    double omega_0;                 //升交点赤经， rad
    double omega_dot;               //升交点赤经变化率， rad/s
    unsigned int fnavt0c;           //卫星钟差参数 ， s，（当FNAVReceived 值为“真”时有效）
    double fnavaf_0;                //卫星钟差参数， s，（当FNAVReceived 值为“真”时有效）
    double fnavaf_1;                //卫星钟差参数， s/s，（当FNAVReceived 值为“真”时有效）
    double fnavaf_2;                //卫星钟差参数， s/s^2 ，（当FNAVReceived 值为“真”时有效）
    unsigned int inavt0c;           //卫星钟差参数 ， s，（当INAVReceived 值为“真”时有效）
    double inavaf_0;                //卫星钟差参数， s，（当INAVReceived 值为“真”时有效）
    double inavaf_1;                //卫星钟差参数， s/s，（当INAVReceived 值为“真”时有效）
    double inavaf_2;                //卫星钟差参数， s/s^2 ，（当INAVReceived 值为“真”时有效）
    double e1e5a_bgd;               //E1, E5a 广播群延迟
    double e1e5b_bgd;               //E1, E5b 广播群延迟， （当INAVReceived 值为“真”时有效）
};

struct Ephemeris
{
    unsigned char flag;             //bit0 = 1 GLONASS 星历数据有效
                                    //bit1 = 1 GPS 星历数据有效
                                    //bit2 = 1 北斗 星历数据有效
                                    //bit3 = 1 伽利略 星历数据有效
    void *ephem;
};

struct RangehData
{
    unsigned short prn;             //卫星PRN号（GPS ：1到32 ，GLONASS: 38到61，BDS1到63，SBAS120到141及183到187，QZSS193到197）
    unsigned short glofreq;         //（GLONASS 频率+ 7），GPS，BDS和Galileo不使用
    double psr;                     //码伪距测量值（米）
    float psr_std;                  //码伪距标准差（米）
    double adr;                     //载波相位，周（积分多普勒）
    float adr_std;                  //载波相位标准差（周）
    float dopp;                     //瞬时多普勒（Hz）
    float c_no;                     //载噪比 C/No = 10[log10(S/N0)]（dBHz）
    float locktime;                 //秒，连续跟踪时间（无周跳）
    unsigned int ch_tr_status;      //通道跟踪状态
};

struct Rangeh
{
    double time_stamp;               //时间戳
    unsigned char satellite_num;     //卫星数量
    struct RangehData **data;
};

extern enum PositionVelocityType gnssState;


void *thread_ub482(void *arg);

#endif
