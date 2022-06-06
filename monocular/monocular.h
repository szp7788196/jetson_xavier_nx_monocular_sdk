#ifndef __MONOCULAR_H
#define __MONOCULAR_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "mpu9250.h"
#include "sync_module.h"
#include "ub482.h"
#include "serial.h"


#define MAX_QUEUE_MSG_NUM				100

#define KEY_UB482_GPGGA_MSG         	1000
#define KEY_NTRIP_RTCM_MSG              1001
#define KEY_FRAME_RATE_MSG          	1002
#define KEY_UB482_TIME_STAMP_MSG    	1003
#define KEY_SYNC_CAM_TIME_STAMP_MSG    	1004
#define KEY_CAMERA_RESET_MSG			1005
#define KEY_SYNC_MODULE_RESET_MSG		1006
#define KEY_IMAGE_HANDLER_MSG			1007
#define KEY_IMU_ADS16505_HANDLER_MSG	1008
#define KEY_IMU_MPU9250_HANDLER_MSG		1009
#define KEY_GNSS_UB482_HANDLER_MSG		1010
#define KEY_SYNC_1HZ_SUCCESS_MSG		1011
#define KEY_CAMERA_READY_MSG			1012

#define NTRIP_RTCM_MSG_MAX_LEN          1024

#define NOT_SYNC_THRESHOLD      		30

#define RTCM3PREAMB 					0xD3


#define CAP(val, min, max)\
    if(val < min)\
    {\
        val = min;\
    }\
    else if(val > max)\
    {\
        val = max;\
    }

enum CameraState
{
    INIT_CONFIG             = 0,            //初始化配置
    CONNECT_CAMERA          = 1,            //链接相机
    LOAD_DEFAULT_CONFIG     = 2,            //加载默认配置
    QUERY_CAMERA_CONFIG     = 3,            //获取相机配置
    LOAD_USER_CONFIG        = 4,            //加载用户配置
    SET_USER_CONFIG         = 5,            //设置用户配置
    ALLOC_FRAME_BUFFER      = 6,            //申请帧缓存
    SET_TRIGGER_MODE        = 7,            //设置触发模式 外部触发 自由模式
	START_CAPTURE           = 8,            //采集图像
    CAPTURE_IMAGE           = 9,            //采集图像
    DISCONNECT_CAMERA       = 10,            //断开相机链接
};

struct QueueMsg
{
    long int mtype;                             //消息类型
    char mtext[8];             					//消息内容
};

struct NormalMsg
{
	unsigned short len;
	unsigned char *msg;
};

struct ImageBuffer
{
    char *image;
    unsigned int size;
	unsigned int counter;
	unsigned int number;
};

struct ImageHeapUnit
{
	struct ImageBuffer *image;
	struct SyncCamTimeStamp *time_stamp;
};

struct ImageHeap
{
	struct ImageHeapUnit **heap;
	unsigned short depth;
	unsigned short cnt;
	unsigned short get_ptr;
	unsigned short put_ptr;
};

struct ImuAdis16505Heap
{
	struct SyncImuData **heap;
	unsigned short depth;
	unsigned short cnt;
	unsigned short get_ptr;
	unsigned short put_ptr;
};

struct ImuMpu9250Heap
{
	struct Mpu9250SampleData **heap;
	unsigned short depth;
	unsigned short cnt;
	unsigned short get_ptr;
	unsigned short put_ptr;
};

struct GnssUb482Heap
{
	struct Ub482GnssData **heap;
	unsigned short depth;
	unsigned short cnt;
	unsigned short get_ptr;
	unsigned short put_ptr;
};

typedef int (*ImageHandler)(struct ImageHeapUnit *);
typedef int (*ImuAds16505Handler)(struct SyncImuData *);
typedef int (*ImuMpu9250Handler)(struct Mpu9250SampleData *);
typedef int (*GnssUb482Handler)(struct Ub482GnssData *);

struct DataHandler
{
	ImageHandler image_handler;
	ImuAds16505Handler imu_ads16505_handler;
	ImuMpu9250Handler imu_mpu9250_handler;
	GnssUb482Handler gnss_ub482_handler;
};


static const unsigned int crc32tab[] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
	0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
	0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
	0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
	0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
	0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
	0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
	0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
	0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
	0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
	0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
	0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
	0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
	0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
	0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
	0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
	0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
	0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
	0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
	0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
	0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
	0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
	0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
	0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
	0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
	0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
	0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
	0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
	0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
	0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
	0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
	0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
	0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
	0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
	0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
	0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
	0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
	0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
	0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
	0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
	0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
	0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
	0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
	0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
	0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
	0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
	0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
	0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
	0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
	0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
	0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
	0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

static const unsigned int tbl_CRC24Q[] =
{
    0x000000,0x864CFB,0x8AD50D,0x0C99F6,0x93E6E1,0x15AA1A,0x1933EC,0x9F7F17,
    0xA18139,0x27CDC2,0x2B5434,0xAD18CF,0x3267D8,0xB42B23,0xB8B2D5,0x3EFE2E,
    0xC54E89,0x430272,0x4F9B84,0xC9D77F,0x56A868,0xD0E493,0xDC7D65,0x5A319E,
    0x64CFB0,0xE2834B,0xEE1ABD,0x685646,0xF72951,0x7165AA,0x7DFC5C,0xFBB0A7,
    0x0CD1E9,0x8A9D12,0x8604E4,0x00481F,0x9F3708,0x197BF3,0x15E205,0x93AEFE,
    0xAD50D0,0x2B1C2B,0x2785DD,0xA1C926,0x3EB631,0xB8FACA,0xB4633C,0x322FC7,
    0xC99F60,0x4FD39B,0x434A6D,0xC50696,0x5A7981,0xDC357A,0xD0AC8C,0x56E077,
    0x681E59,0xEE52A2,0xE2CB54,0x6487AF,0xFBF8B8,0x7DB443,0x712DB5,0xF7614E,
    0x19A3D2,0x9FEF29,0x9376DF,0x153A24,0x8A4533,0x0C09C8,0x00903E,0x86DCC5,
    0xB822EB,0x3E6E10,0x32F7E6,0xB4BB1D,0x2BC40A,0xAD88F1,0xA11107,0x275DFC,
    0xDCED5B,0x5AA1A0,0x563856,0xD074AD,0x4F0BBA,0xC94741,0xC5DEB7,0x43924C,
    0x7D6C62,0xFB2099,0xF7B96F,0x71F594,0xEE8A83,0x68C678,0x645F8E,0xE21375,
    0x15723B,0x933EC0,0x9FA736,0x19EBCD,0x8694DA,0x00D821,0x0C41D7,0x8A0D2C,
    0xB4F302,0x32BFF9,0x3E260F,0xB86AF4,0x2715E3,0xA15918,0xADC0EE,0x2B8C15,
    0xD03CB2,0x567049,0x5AE9BF,0xDCA544,0x43DA53,0xC596A8,0xC90F5E,0x4F43A5,
    0x71BD8B,0xF7F170,0xFB6886,0x7D247D,0xE25B6A,0x641791,0x688E67,0xEEC29C,
    0x3347A4,0xB50B5F,0xB992A9,0x3FDE52,0xA0A145,0x26EDBE,0x2A7448,0xAC38B3,
    0x92C69D,0x148A66,0x181390,0x9E5F6B,0x01207C,0x876C87,0x8BF571,0x0DB98A,
    0xF6092D,0x7045D6,0x7CDC20,0xFA90DB,0x65EFCC,0xE3A337,0xEF3AC1,0x69763A,
    0x578814,0xD1C4EF,0xDD5D19,0x5B11E2,0xC46EF5,0x42220E,0x4EBBF8,0xC8F703,
    0x3F964D,0xB9DAB6,0xB54340,0x330FBB,0xAC70AC,0x2A3C57,0x26A5A1,0xA0E95A,
    0x9E1774,0x185B8F,0x14C279,0x928E82,0x0DF195,0x8BBD6E,0x872498,0x016863,
    0xFAD8C4,0x7C943F,0x700DC9,0xF64132,0x693E25,0xEF72DE,0xE3EB28,0x65A7D3,
    0x5B59FD,0xDD1506,0xD18CF0,0x57C00B,0xC8BF1C,0x4EF3E7,0x426A11,0xC426EA,
    0x2AE476,0xACA88D,0xA0317B,0x267D80,0xB90297,0x3F4E6C,0x33D79A,0xB59B61,
    0x8B654F,0x0D29B4,0x01B042,0x87FCB9,0x1883AE,0x9ECF55,0x9256A3,0x141A58,
    0xEFAAFF,0x69E604,0x657FF2,0xE33309,0x7C4C1E,0xFA00E5,0xF69913,0x70D5E8,
    0x4E2BC6,0xC8673D,0xC4FECB,0x42B230,0xDDCD27,0x5B81DC,0x57182A,0xD154D1,
    0x26359F,0xA07964,0xACE092,0x2AAC69,0xB5D37E,0x339F85,0x3F0673,0xB94A88,
    0x87B4A6,0x01F85D,0x0D61AB,0x8B2D50,0x145247,0x921EBC,0x9E874A,0x18CBB1,
    0xE37B16,0x6537ED,0x69AE1B,0xEFE2E0,0x709DF7,0xF6D10C,0xFA48FA,0x7C0401,
    0x42FA2F,0xC4B6D4,0xC82F22,0x4E63D9,0xD11CCE,0x575035,0x5BC9C3,0xDD8538
};

extern struct ImageHeap imageHeap;
extern struct ImuAdis16505Heap imuAdis16505Heap;
extern struct ImuMpu9250Heap imuMpu9250Heap;
extern struct GnssUb482Heap gnssUb482Heap;

extern pthread_mutex_t mutexImageHeap;
extern pthread_mutex_t mutexImuAdis16505Heap;
extern pthread_mutex_t mutexImuMpu9250Heap;
extern pthread_mutex_t mutexGnssUb482Heap;
extern pthread_cond_t condImageHeap;

extern struct DataHandler dataHandler;

unsigned int CRC32(unsigned char *buf, unsigned int size);
unsigned short CalCheckSum(unsigned char *buf, unsigned short len);
unsigned char CalCheckOr(unsigned char *buf, unsigned short len);
int check_rtcm3(const unsigned char *data, unsigned int data_len);
unsigned short mystrstr(unsigned char *str1,
                        unsigned char *str2,
						unsigned short str1_len,
						unsigned short str2_len);
unsigned short find_str(unsigned char *s_str,
                        unsigned char *p_str,
						unsigned short count,
						unsigned short *seek);
int search_str(unsigned char *source, unsigned char *target);
unsigned short get_str1(unsigned char *source,
                        unsigned char *begin,
						unsigned short count1,
						unsigned char *end,
						unsigned short count2,
						unsigned char *out);
unsigned short get_str2(unsigned char *source,
                        unsigned char *begin,
						unsigned short count,
						unsigned short length,
						unsigned char *out);
int my_toupper(int ch);
void HexToStr(char *pbDest, unsigned char *pbSrc, unsigned short len);
void StrToHex(unsigned char *pbDest, char *pbSrc, unsigned short len);
int AT_SendCmd(struct Serial *sn,
               char* cmd,
               char *result,
			   char *rsp_buf,
               unsigned int waittime,
               unsigned char retry,
               unsigned int timeout);
int queryEC20_IMEI(char *imei);
int xQueueSend(key_t queue_key,void *msg_to_queue,unsigned short queue_depth);
int xQueueReceive(key_t queue_key,void **msg_from_queue,unsigned char block);
void freeImageHeap(void);
void clearSystemQueueMsg(void);
int allocateImageHeap(unsigned short depth,unsigned int image_size);
void freeImuAdis16505Heap(void);
int allocateImuAdis16505Heap(unsigned short depth);
void freeImuMpu9250Heap(void);
int allocateImuMpu9250Heap(unsigned short depth);
void freeGnssUb482Heap(void);
int allocateGnssUb482Heap(unsigned short depth);
int gnssUb482HeapGet(struct Ub482GnssData *data);
int imageHeapGet(struct ImageHeapUnit *data);
int imuAdis16505HeapPut(struct SyncImuData *data);
int imuAdis16505HeapGet(struct SyncImuData *data);
int imuMpu9250HeapPut(struct Mpu9250SampleData *data);
int imuMpu9250HeapGet(struct Mpu9250SampleData *data);
int gnssUb482HeapPut(struct Ub482GnssData *data);
int gnssUb482HeapGet(struct Ub482GnssData *data);
int imageBufCompressToJpeg(char * file_name,
                           int quality,
                           unsigned char *image_buffer,
                           unsigned int image_width,
                           unsigned int image_height,
						   unsigned char format);
int monocular_sdk_init(int argc, char **argv);
void monocular_sdk_register_handler(ImageHandler image_handler,
                                    ImuAds16505Handler imu_ads16505_handler,
									ImuMpu9250Handler imu_mpu9250_handler,
									GnssUb482Handler gnss_ub482_handler);

#endif
