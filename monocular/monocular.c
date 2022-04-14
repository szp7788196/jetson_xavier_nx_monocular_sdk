#include "monocular.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#include "cmd_parse.h"
#include "cssc132.h"
#include "led.h"
#include "ui3240.h"
#include "net.h"
#include "sync.h"
#include "handler.h"

struct ImageHeap imageHeap = {NULL,0,0,0};
struct ImuAdis16505Heap imuAdis16505Heap = {NULL,0,0,0};
struct ImuMpu9250Heap imuMpu9250Heap = {NULL,0,0,0};
struct GnssUb482Heap gnssUb482Heap = {NULL,0,0,0};

pthread_mutex_t mutexImageHeap;
pthread_mutex_t mutexImuAdis16505Heap;
pthread_mutex_t mutexImuMpu9250Heap;
pthread_mutex_t mutexGnssUb482Heap;
pthread_cond_t condImageHeap;

struct DataHandler dataHandler = {NULL,NULL,NULL,NULL};

unsigned int CRC32(unsigned char *buf, unsigned int size)
{
    unsigned int i = 0;
    unsigned int crc = 0;

    for (i = 0; i < size; i ++)
    {
        crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
    }

    return crc;
}

unsigned short CalCheckSum(unsigned char *buf, unsigned short len)
{
	unsigned short sum = 0;
	unsigned short i = 0;

	for(i = 0; i < len; i ++)
	{
		sum += *(buf + i);
	}

	return sum;
}

unsigned char CalCheckOr(unsigned char *buf, unsigned short len)
{
	unsigned char sum = 0;
	unsigned short i = 0;

	for(i = 0; i < len; i ++)
	{
		sum ^= *(buf + i);
	}

	return sum;
}

static unsigned int getbitu(const unsigned char *buff, int pos, int len) 
{
	unsigned int bits = 0;
	int i;
	
	for(i = pos; i < pos + len; i ++) bits = (bits << 1) + ((buff[i / 8] >> (7 - i % 8)) & 1u);
	
	return bits;
}
static unsigned int rtk_crc24q(const unsigned char *buff, int len) 
{
	unsigned int crc = 0;
	int i;

	for(i = 0; i < len; i ++) crc = ((crc << 8) & 0xFFFFFF) ^ tbl_CRC24Q[(crc >> 16) ^ buff[i]];
	
	return crc;
}

int check_rtcm3(const unsigned char *data, unsigned int data_len) 
{
    int ret = 0;
	int nbyte = 0;
    int len = 0;
	unsigned char buff[4096];

	for(int i = 0; i < data_len; i ++)
	{
		if(nbyte == 0)
		{
			if(data[i] != RTCM3PREAMB)
			{
				fprintf(stderr, "%s: not correct preamb\n",__func__);
				continue;
			}
			
			buff[nbyte ++] = data[i];
		} 
		else
		{
			buff[nbyte ++] = data[i];

			if(nbyte == 3) len = getbitu(buff, 14, 10) + 3;
			if(nbyte >= len + 3)
			{
				if(rtk_crc24q(buff, len) != getbitu(buff, len * 8, 24))
				{
                    ret = -1;
				}
                
				nbyte = 0;
				len = 0;
			}
		}
	}

	if(nbyte != 0)
	{
		fprintf(stderr, "%s: message not complete!\n",__func__);
	}

    return ret;
}

//在str1中查找str2，失败返回0xFF,成功返回str2首个元素在str1中的位置
unsigned short mystrstr(unsigned char *str1, 
                        unsigned char *str2, 
						unsigned short str1_len, 
						unsigned short str2_len)
{
	unsigned short len = str1_len;
	if(str1_len == 0 || str2_len == 0)
	{
		return 0xFFFF;
	}
	else
	{
		while(str1_len >= str2_len)
		{
			str1_len --;
			if (!memcmp(str1, str2, str2_len))
			{
				return len - str1_len - 1;
			}
			str1 ++;
		}
		return 0xFFFF;
	}
}

unsigned short find_str(unsigned char *s_str, 
                        unsigned char *p_str, 
						unsigned short count, 
						unsigned short *seek)
{
	unsigned short _count = 1;
    unsigned short len = 0;
    unsigned char *temp_str = NULL;
    unsigned char *temp_ptr = NULL;
    unsigned char *temp_char = NULL;

	(*seek) = 0;

    if(0 == s_str || 0 == p_str)
    {
        return 0;
    }
        
    for(temp_str = s_str; *temp_str != '\0'; temp_str ++)
    {
        temp_char = temp_str;

        for(temp_ptr = p_str; *temp_ptr != '\0'; temp_ptr ++)
        {
            if(*temp_ptr != *temp_char)
            {
                len = 0;
                break;
            }

            temp_char++;
            len++;
        }

        if(*temp_ptr == '\0')
        {
            if(_count == count)
            {
                return len;
            }
            else
            {
                _count++;
                len = 0;
            }
        }

        (*seek) ++;
    }

    return 0;
}

int search_str(unsigned char *source, unsigned char *target)
{
	unsigned short seek = 0;
    unsigned short len;

    len = find_str(source, target, 1, &seek);

    if(len == 0)
    {
        return -1;
    }
    else
    {
        return len;
    }
}

unsigned short get_str1(unsigned char *source, 
                        unsigned char *begin, 
						unsigned short count1, 
						unsigned char *end, 
						unsigned short count2, 
						unsigned char *out)
{
	unsigned short i;
    unsigned short len1;
    unsigned short len2;
    unsigned short index1 = 0;
    unsigned short index2 = 0;
    unsigned short length = 0;

    len1 = find_str(source, begin, count1, &index1);
    len2 = find_str(source, end, count2, &index2);
    length = index2 - index1 - len1;

    if((len1 != 0) && (len2 != 0))
    {
        for( i = 0; i < index2 - index1 - len1; i ++)
            out[i] = source[index1 + len1 + i];
        out[i] = '\0';
    }
    else
    {
        out[0] = '\0';
    }

    return length;
}

unsigned short get_str2(unsigned char *source, 
                        unsigned char *begin, 
						unsigned short count, 
						unsigned short length, 
						unsigned char *out)
{
	unsigned short i = 0;
    unsigned short len1 = 0;
    unsigned short index1 = 0;

    len1 = find_str(source, begin, count, &index1);

    if(len1 != 0)
    {
        for(i = 0; i < length; i ++)
            out[i] = source[index1 + len1 + i];
        out[i] = '\0';
    }
    else
    {
        out[0] = '\0';
    }

    return length;
}

//小写字母转换为大写字母。
int my_toupper(int ch)
{

	if((unsigned int)(ch - 'a') < 26)
    {
        ch += 'A' - 'a';
    }

	return ch;
}

/*
// C prototype : void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 存放目标字符串
// [IN] pbSrc - 输入16进制数的起始地址
// [IN] nLen - 16进制数的字节数
// return value:
// remarks : 将16进制数转化为字符串
*/
void HexToStr(char *pbDest, unsigned char *pbSrc, unsigned short len)
{
	char ddl,ddh;
	int i;

	for (i = 0; i < len; i ++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i * 2] = ddh;
		pbDest[i * 2 + 1] = ddl;
	}

	pbDest[len * 2] = '\0';
}

/*
// C prototype : void StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 输出缓冲区
// [IN] pbSrc - 字符串
// [IN] nLen - 16进制数的字节数(字符串的长度/2)
// return value:
// remarks : 将字符串转化为16进制数
*/
void StrToHex(unsigned char *pbDest, char *pbSrc, unsigned short len)
{
	char h1,h2;
	unsigned char s1,s2;
	int i;

	for (i = 0; i < len; i ++)
	{
		h1 = pbSrc[2 * i];
		h2 = pbSrc[2 * i + 1];

		s1 = my_toupper(h1) - 0x30;
		if (s1 > 9)
		s1 -= 7;

		s2 = my_toupper(h2) - 0x30;
		if (s2 > 9)
		s2 -= 7;

		pbDest[i] = s1 * 16 + s2;
	}
}

int xQueueSend(key_t queue_key,void *msg_to_queue,unsigned short queue_depth)
{
	int ret = 0;
	unsigned char i = 0;
	int msg_id = -1;
	struct QueueMsg msg;
	struct msqid_ds queue_info;
	char *pointer = NULL;

	msg_id = msgget(queue_key, IPC_CREAT | 0777);

	if(msg_id == -1)
	{
		fprintf(stderr, "%s: create queue msg id failed\n",__func__);
		return -1;
	}

	ret = msgctl(msg_id, IPC_STAT, &queue_info);
	if(ret == -1)
	{
		fprintf(stderr, "%s: query queue info failed\n",__func__);
		return -1;
	}

	if(queue_info.msg_qnum >= queue_depth)
	{
		ret = xQueueReceive(queue_key,(void **)&pointer,0);
		if(ret == -1)
		{
			free(pointer);
			pointer = NULL;
		}
	}

	memset(&msg,0,sizeof(struct QueueMsg));

	if(msg_to_queue == NULL)
	{
		fprintf(stderr, "%s: the msg to be added cannot be null\n",__func__);
		return -1;
	}

	msg.mtype = 1;

	for(i = 0; i < 8; i ++)
	{
		msg.mtext[i] = ((((long)msg_to_queue) >> (64 - (i + 1) * 8)) & 0x00000000000000FF);
	}

	ret = msgsnd(msg_id,&msg,sizeof(msg.mtext),0);
	if(ret == -1)
	{
		fprintf(stderr, "%s: send msg to queue failed\n",__func__);
		return -1;
	}

	return ret;
}

int xQueueReceive(key_t queue_key,void **msg_from_queue,unsigned char block)
{
	int ret = 0;
	int msg_id = -1;
	struct QueueMsg msg;

	msg_id = msgget(queue_key, IPC_CREAT | 0777);

	if(msg_id == -1)
	{
		fprintf(stderr, "%s: create queue msg id failed\n",__func__);
		return -1;
	}

	memset(&msg,0,sizeof(struct QueueMsg));

	if(block != 0)
	{
		ret = msgrcv(msg_id,&msg,sizeof(msg.mtext),1,0);
	}
	else
	{
		ret = msgrcv(msg_id,&msg,sizeof(msg.mtext),1,IPC_NOWAIT);
	}
	
    if(ret == -1)
    {
        return -1;
    }

	*msg_from_queue = (void *)((((long)msg.mtext[0] << 56) & 0xFF00000000000000) + 
                               (((long)msg.mtext[1] << 48) & 0x00FF000000000000) + 
                               (((long)msg.mtext[2] << 40) & 0x0000FF0000000000) + 
                               (((long)msg.mtext[3] << 32) & 0x000000FF00000000) + 
                               (((long)msg.mtext[4] << 24) & 0x00000000FF000000) + 
                               (((long)msg.mtext[5] << 16) & 0x0000000000FF0000) + 
                               (((long)msg.mtext[6] <<  8) & 0x000000000000FF00) + 
                               (((long)msg.mtext[7] <<  0) & 0x00000000000000FF));

	return ret;
}

void clearSystemQueueMsg(void)
{
	int ret = 0;
	char *pointer = NULL;

	do
	{
		ret = xQueueReceive((key_t)KEY_UB482_GPGGA_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_NTRIP_RTCM_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_FRAME_RATE_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_UB482_TIME_STAMP_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_SYNC_CAM_TIME_STAMP_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_CAMERA_RESET_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_SYNC_MODULE_RESET_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_IMAGE_HANDLER_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_IMU_ADS16505_HANDLER_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_IMU_MPU9250_HANDLER_MSG,(void **)&pointer,0);
	}
	while(ret != -1);

	do
	{
		ret = xQueueReceive((key_t)KEY_GNSS_UB482_HANDLER_MSG,(void **)&pointer,0);
	}
	while(ret != -1);
}

void freeImageHeap(void)
{
	int i = 0;

	if(imageHeap.heap != NULL && imageHeap.depth != 0)
    {
        for(i = 0; i < imageHeap.depth; i ++)
        {
            if(imageHeap.heap[i] != NULL)
            {
                if(imageHeap.heap[i]->image != NULL)
                {
                    if(imageHeap.heap[i]->image->image != NULL)
                    {
                        free(imageHeap.heap[i]->image->image);
                        imageHeap.heap[i]->image->image = NULL;
                    }

                    free(imageHeap.heap[i]->image);
                    imageHeap.heap[i]->image = NULL;
                }

				if(imageHeap.heap[i]->time_stamp != NULL)
                {
                    free(imageHeap.heap[i]->time_stamp);
                    imageHeap.heap[i]->time_stamp = NULL;
                }

                free(imageHeap.heap[i]);
                imageHeap.heap[i] = NULL;
            }
        }

        imageHeap.heap = NULL;
		imageHeap.cnt = 0;
		imageHeap.depth = 0;
        imageHeap.put_ptr = 0;
        imageHeap.get_ptr = 0;
    }
}

int allocateImageHeap(unsigned short depth,unsigned int image_size)
{
	int i = 0;

	if(imageHeap.heap != NULL)
	{
		fprintf(stderr, "%s: imageHeap.heap does not null\n",__func__);
		return -1;
	}

	if(depth & (depth - 1))		//判断depth是否为2的N次幂
	{
		fprintf(stderr, "%s: depth dose not N-th power of 2\n",__func__);
		return -1;
	}

	imageHeap.depth = depth;

	imageHeap.heap = calloc(imageHeap.depth, sizeof(struct ImageHeapUnit));

	if(imageHeap.heap == NULL)
	{
		fprintf(stderr, "%s: calloc imageHeap.heap failed\n",__func__);
		return -1;
	}

	for(i = 0; i < imageHeap.depth; i ++)
	{	
		imageHeap.heap[i] = NULL;
		imageHeap.heap[i] = (struct ImageHeapUnit *)malloc(sizeof(struct ImageHeapUnit));
		if(imageHeap.heap[i] == NULL)
		{
			fprintf(stderr, "%s: malloc imageHeap.heap[%d] failed\n",__func__,i);
			return -1;
		}

		imageHeap.heap[i]->image = NULL;
		imageHeap.heap[i]->image = (struct ImageBuffer *)malloc(sizeof(struct ImageBuffer));
		if(imageHeap.heap[i]->image == NULL)
		{
			fprintf(stderr, "%s: malloc imageHeap.heap[%d]->image failed\n",__func__,i);
			return -1;
		}

		memset(imageHeap.heap[i]->image,0,sizeof(struct ImageBuffer));

		imageHeap.heap[i]->image->size = image_size;

		imageHeap.heap[i]->image->image = (char *)malloc(image_size * sizeof(char));
		if(imageHeap.heap[i]->image->image == NULL)
		{
			fprintf(stderr, "%s: malloc imageHeap.heap[%d]->image->image failed\n",__func__,i);
			return -1;
		}

		memset(imageHeap.heap[i]->image->image,0,image_size);

		imageHeap.heap[i]->time_stamp = NULL;
		if(imageHeap.heap[i]->time_stamp != NULL)
		{
			fprintf(stderr, "%s: imageHeap.heap[%d]->time_stamp does not null\n",__func__,i);
			return -1;
		}

		imageHeap.heap[i]->time_stamp = NULL;
		imageHeap.heap[i]->time_stamp = (struct SyncCamTimeStamp *)malloc(sizeof(struct SyncCamTimeStamp));
		if(imageHeap.heap[i]->time_stamp == NULL)
		{
			fprintf(stderr, "%s: malloc imageHeap.heap[%d]->time_stamp failed\n",__func__,i);
			return -1;
		}

		memset(imageHeap.heap[i]->time_stamp,0,sizeof(struct SyncCamTimeStamp));
	}

	return 0;
}

void freeImuAdis16505Heap(void)
{
	int i = 0;

	if(imuAdis16505Heap.heap != NULL && imuAdis16505Heap.depth != 0)
	{
		for(i = 0; i < imuAdis16505Heap.depth; i ++)
		{
			if(imuAdis16505Heap.heap[i] != NULL)
			{
				free(imuAdis16505Heap.heap[i]);
				imuAdis16505Heap.heap[i] = NULL;
			}
		}

		imuAdis16505Heap.heap = NULL;
		imuAdis16505Heap.cnt = 0;
		imuAdis16505Heap.depth = 0;
        imuAdis16505Heap.put_ptr = 0;
        imuAdis16505Heap.get_ptr = 0;
	}
}

int allocateImuAdis16505Heap(unsigned short depth)
{
	int i = 0;

	if(imuAdis16505Heap.heap != NULL)
	{
		fprintf(stderr, "%s: imuAdis16505Heap.heap does not null\n",__func__);
		return -1;
	}

	if(depth & (depth - 1))		//判断depth是否为2的N次幂
	{
		fprintf(stderr, "%s: depth dose not N-th power of 2\n",__func__);
		return -1;
	}

	imuAdis16505Heap.depth = depth;

	imuAdis16505Heap.heap = calloc(imuAdis16505Heap.depth, sizeof(struct SyncImuData));

	if(imuAdis16505Heap.heap == NULL)
	{
		fprintf(stderr, "%s: calloc imuAdis16505Heap.heap failed\n",__func__);
		return -1;
	}

	for(i = 0; i < imuAdis16505Heap.depth; i ++)
	{
		imuAdis16505Heap.heap[i] = NULL;
		imuAdis16505Heap.heap[i] = (struct SyncImuData *)malloc(sizeof(struct SyncImuData));
		if(imuAdis16505Heap.heap[i] == NULL)
		{
			fprintf(stderr, "%s: malloc imuAdis16505Heap.heap[%d] failed\n",__func__,i);
			return -1;
		}
	}

	return 0;
}

void freeImuMpu9250Heap(void)
{
	int i = 0;

	if(imuMpu9250Heap.heap != NULL && imuMpu9250Heap.depth != 0)
	{
		for(i = 0; i < imuMpu9250Heap.depth; i ++)
		{
			if(imuMpu9250Heap.heap[i] != NULL)
			{
				free(imuMpu9250Heap.heap[i]);
				imuMpu9250Heap.heap[i] = NULL;
			}
		}

		imuMpu9250Heap.heap = NULL;
		imuMpu9250Heap.cnt = 0;
		imuMpu9250Heap.depth = 0;
        imuMpu9250Heap.put_ptr = 0;
        imuMpu9250Heap.get_ptr = 0;
	}
}

int allocateImuMpu9250Heap(unsigned short depth)
{
	int i = 0;

	if(imuMpu9250Heap.heap != NULL)
	{
		fprintf(stderr, "%s: imuMpu9250Heap.heap does not null\n",__func__);
		return -1;
	}

	if(depth & (depth - 1))		//判断depth是否为2的N次幂
	{
		fprintf(stderr, "%s: depth dose not N-th power of 2\n",__func__);
		return -1;
	}

	imuMpu9250Heap.depth = depth;

	imuMpu9250Heap.heap = calloc(imuMpu9250Heap.depth, sizeof(struct Mpu9250SampleData));

	if(imuMpu9250Heap.heap == NULL)
	{
		fprintf(stderr, "%s: calloc imuMpu9250Heap.heap failed\n",__func__);
		return -1;
	}

	for(i = 0; i < depth; i ++)
	{
		imuMpu9250Heap.heap[i] = NULL;
		imuMpu9250Heap.heap[i] = (struct Mpu9250SampleData *)malloc(sizeof(struct Mpu9250SampleData));
		if(imuMpu9250Heap.heap[i] == NULL)
		{
			fprintf(stderr, "%s: malloc imuMpu9250Heap.heap[%d] failed\n",__func__,i);
			return -1;
		}
	}

	return 0;
}

void freeGnssUb482Heap(void)
{
	int i = 0;

	if(gnssUb482Heap.heap != NULL && gnssUb482Heap.depth != 0)
	{
		for(i = 0; i < gnssUb482Heap.depth; i ++)
		{
			if(gnssUb482Heap.heap[i] != NULL)
			{
				free(gnssUb482Heap.heap[i]);
				gnssUb482Heap.heap[i] = NULL;
			}
		}

		gnssUb482Heap.heap = NULL;
		gnssUb482Heap.cnt = 0;
		gnssUb482Heap.depth = 0;
        gnssUb482Heap.put_ptr = 0;
        gnssUb482Heap.get_ptr = 0;
	}
}

int allocateGnssUb482Heap(unsigned short depth)
{
	int i = 0;

	if(gnssUb482Heap.heap != NULL)
	{
		fprintf(stderr, "%s: gnssUb482Heap.heap does not null\n",__func__);
		return -1;
	}

	if(depth & (depth - 1))		//判断depth是否为2的N次幂
	{
		fprintf(stderr, "%s: depth dose not N-th power of 2\n",__func__);
		return -1;
	}

	gnssUb482Heap.depth = depth;

	gnssUb482Heap.heap = calloc(gnssUb482Heap.depth, sizeof(struct Ub482GnssData));

	if(gnssUb482Heap.heap == NULL)
	{
		fprintf(stderr, "%s: calloc gnssUb482Heap.heap failed\n",__func__);
		return -1;
	}

	for(i = 0; i < depth; i ++)
	{
		gnssUb482Heap.heap[i] = NULL;
		gnssUb482Heap.heap[i] = (struct Ub482GnssData *)malloc(sizeof(struct Ub482GnssData));
		if(gnssUb482Heap.heap[i] == NULL)
		{
			fprintf(stderr, "%s: malloc gnssUb482Heap.heap[%d] failed\n",__func__,i);
			return -1;
		}
	}

	return 0;
}

int imageHeapGet(struct ImageHeapUnit *data)
{
	if(data == NULL)
	{
		fprintf(stderr, "%s: data is null\n",__func__);
		return -1;
	}

	if(imageHeap.cnt == 0)
	{
		return -1;
	}

	pthread_mutex_lock(&mutexImageHeap); 

	if(imageHeap.cnt > 0)
	{
		memcpy(data,imageHeap.heap[imageHeap.get_ptr],sizeof(struct ImageHeapUnit));

		imageHeap.get_ptr = (imageHeap.get_ptr + 1) % imageHeap.depth;

		imageHeap.cnt -= 1;
	}

	pthread_mutex_unlock(&mutexImageHeap);

	return 0;
}

int imuAdis16505HeapPut(struct SyncImuData *data)
{
	int ret = 0;

	if(data == NULL)
	{
		fprintf(stderr, "%s: data is null\n",__func__);
		return -1;
	}

	pthread_mutex_lock(&mutexImuAdis16505Heap); 

	memcpy(imuAdis16505Heap.heap[imuAdis16505Heap.put_ptr],data,sizeof(struct SyncImuData));

	ret = xQueueSend((key_t)KEY_IMU_ADS16505_HANDLER_MSG,
	                 imuAdis16505Heap.heap[imuAdis16505Heap.put_ptr],
					 imuAdis16505Heap.depth);
	if(ret == -1)
	{
		fprintf(stderr, "%s: send imuAdis16505Heap.heap[imuAdis16505Heap.put_ptr] queue msg failed\n",__func__);
	}

	imuAdis16505Heap.put_ptr = (imuAdis16505Heap.put_ptr + 1) % imuAdis16505Heap.depth;

	imuAdis16505Heap.cnt += 1;
	if(imuAdis16505Heap.cnt >= imuAdis16505Heap.depth)
	{
		imuAdis16505Heap.cnt = imuAdis16505Heap.depth;

		imuAdis16505Heap.get_ptr = imuAdis16505Heap.put_ptr;
	}

	pthread_mutex_unlock(&mutexImuAdis16505Heap);

	return 0;
}

int imuAdis16505HeapGet(struct SyncImuData *data)
{
	if(data == NULL)
	{
		fprintf(stderr, "%s: data is null\n",__func__);
		return -1;
	}

	if(imuAdis16505Heap.cnt == 0)
	{
		return -1;
	}

	pthread_mutex_lock(&mutexImuAdis16505Heap); 

	if(imuAdis16505Heap.cnt > 0)
	{
		memcpy(data,imuAdis16505Heap.heap[imuAdis16505Heap.get_ptr],sizeof(struct SyncImuData));

		imuAdis16505Heap.get_ptr = (imuAdis16505Heap.get_ptr + 1) % imuAdis16505Heap.depth;

		imuAdis16505Heap.cnt -= 1;
	}

	pthread_mutex_unlock(&mutexImuAdis16505Heap);

	return 0;
}

int imuMpu9250HeapPut(struct Mpu9250SampleData *data)
{
	int ret = 0;

	if(data == NULL)
	{
		fprintf(stderr, "%s: data is null\n",__func__);
		return -1;
	}

	pthread_mutex_lock(&mutexImuMpu9250Heap); 

	memcpy(imuMpu9250Heap.heap[imuMpu9250Heap.put_ptr],data,sizeof(struct Mpu9250SampleData));

	ret = xQueueSend((key_t)KEY_IMU_MPU9250_HANDLER_MSG,
	                 imuMpu9250Heap.heap[imuMpu9250Heap.put_ptr],
					 imuMpu9250Heap.depth);
	if(ret == -1)
	{
		fprintf(stderr, "%s: send imuMpu9250Heap.heap[imuMpu9250Heap.put_ptr] queue msg failed\n",__func__);
	}

	imuMpu9250Heap.put_ptr = (imuMpu9250Heap.put_ptr + 1) % imuMpu9250Heap.depth;

	imuMpu9250Heap.cnt += 1;
	if(imuMpu9250Heap.cnt >= imuMpu9250Heap.depth)
	{
		imuMpu9250Heap.cnt = imuMpu9250Heap.depth;

		imuMpu9250Heap.get_ptr = imuMpu9250Heap.put_ptr;
	}
	pthread_mutex_unlock(&mutexImuMpu9250Heap);

	return 0;
}

int imuMpu9250HeapGet(struct Mpu9250SampleData *data)
{
	if(data == NULL)
	{
		fprintf(stderr, "%s: data is null\n",__func__);
		return -1;
	}

	if(imuMpu9250Heap.cnt == 0)
	{
		return -1;
	}

	pthread_mutex_lock(&mutexImuMpu9250Heap); 

	if(imuMpu9250Heap.cnt > 0)
	{
		memcpy(data,imuMpu9250Heap.heap[imuMpu9250Heap.get_ptr],sizeof(struct Mpu9250SampleData));

		imuMpu9250Heap.get_ptr = (imuMpu9250Heap.get_ptr + 1) % imuMpu9250Heap.depth;

		imuMpu9250Heap.cnt -= 1;
	}

	pthread_mutex_unlock(&mutexImuMpu9250Heap);

	return 0;
}

int gnssUb482HeapPut(struct Ub482GnssData *data)
{
	int ret = 0;

	if(data == NULL)
	{
		fprintf(stderr, "%s: data is null\n",__func__);
		return -1;
	}

	pthread_mutex_lock(&mutexGnssUb482Heap); 

	memcpy(gnssUb482Heap.heap[gnssUb482Heap.put_ptr],data,sizeof(struct Ub482GnssData));

	ret = xQueueSend((key_t)KEY_GNSS_UB482_HANDLER_MSG,
	                 gnssUb482Heap.heap[gnssUb482Heap.put_ptr],
					 gnssUb482Heap.depth);
	if(ret == -1)
	{
		fprintf(stderr, "%s: send gnssUb482Heap.heap[gnssUb482Heap.put_ptr] queue msg failed\n",__func__);
	}

	gnssUb482Heap.put_ptr = (gnssUb482Heap.put_ptr + 1) % gnssUb482Heap.depth;

	gnssUb482Heap.cnt += 1;
	if(gnssUb482Heap.cnt >= gnssUb482Heap.depth)
	{
		gnssUb482Heap.cnt = gnssUb482Heap.depth;

		gnssUb482Heap.get_ptr = gnssUb482Heap.put_ptr;
	}
	pthread_mutex_unlock(&mutexGnssUb482Heap);

	return 0;
}

int gnssUb482HeapGet(struct Ub482GnssData *data)
{
	if(data == NULL)
	{
		fprintf(stderr, "%s: data is null\n",__func__);
		return -1;
	}

	if(gnssUb482Heap.cnt == 0)
	{
		return -1;
	}

	pthread_mutex_lock(&mutexGnssUb482Heap); 

	if(gnssUb482Heap.cnt > 0)
	{
		memcpy(data,gnssUb482Heap.heap[gnssUb482Heap.get_ptr],sizeof(struct Ub482GnssData));

		gnssUb482Heap.get_ptr = (gnssUb482Heap.get_ptr + 1) % gnssUb482Heap.depth;

		gnssUb482Heap.cnt -= 1;
	}

	pthread_mutex_unlock(&mutexGnssUb482Heap);

	return 0;
}

/*
* YUV422打包数据,UYVY,转换为RGB565,
* inBuf -- YUV data
* outBuf -- RGB565 data
* image_width,image_height -- image width and height
*/
static int convert_UYVY_To_RGB(unsigned char *in_buf, 
                        unsigned char *out_buf, 
						int image_width, 
						int image_height)
{
    int rows ,cols;	                        /* 行列标志 */
	int y, u, v, r, g, b;	                /* yuv rgb 相关分量 */
	unsigned char *yuv_data, *rgb_data;	    /* YUV和RGB数据指针 */
	int y_pos, u_pos, v_pos;	            /* Y U V在数据缓存中的偏移 */
	unsigned int i = 0;

	yuv_data = in_buf;
	rgb_data = out_buf;

#if 0
	/*  YUYV */
	y_pos = 0;
	u_pos = y_pos + 1;
	v_pos = u_pos + 2;

	/* YVYU */
	y_pos = 0;
	v_pos = y_pos + 1;
	u_pos = v_pos + 2;
#endif

#if 1   /* UYVY */
	y_pos = 1;
	u_pos = y_pos - 1;
	v_pos = y_pos + 1;
#endif

	/* 每个像素两个字节 */
	for(rows = 0; rows < image_height; rows ++)
	{
		for(cols = 0; cols < image_width; cols ++)
		{
			/* 矩阵推到，百度 */
			y = yuv_data[y_pos];
			u = yuv_data[u_pos] - 128;
			v = yuv_data[v_pos] - 128;

			r = y + v + ((v * 103) >> 8);
			g = y - ((u * 88) >> 8) - ((v * 183) >> 8);
			b = y + u + ((u * 198) >> 8);

			r = r > 255?255:(r < 0?0:r);
			g = g > 255?255:(g < 0?0:g);
			b = b > 255?255:(b < 0?0:b);

			/* 从低到高r g b */
//			*(rgb_data ++) = (((g & 0x1c) << 3) | (b >> 3));	/* g低5位，b高5位 */
//			*(rgb_data ++) = ((r & 0xf8) | (g >> 5));	/* r高5位，g高3位 */

            *(rgb_data ++) = r;
            *(rgb_data ++) = g;
            *(rgb_data ++) = b;

			/* 两个字节数据中包含一个Y */
			y_pos += 2;
			//y_pos++;
			i ++;
			/* 每两个Y更新一次UV */
			if(!(i & 0x01))	
			{
				u_pos = y_pos - 1;
				v_pos = y_pos + 1;
			}
		}
	}

	return 0;
}

/*
* 将图片压缩为jpeg格式
* file_name -- 图片存储路径和文件名称
* quality -- 压缩质量 一般为80
* image_buffer -- RGB或灰度数据
* image_width,image_height -- image width and height
* format -- image_buffer的格式,0:RGB格式;1:灰度格式
*/
int imageBufCompressToJpeg(char * file_name,
                           int quality,
                           unsigned char *image_buffer,
                           unsigned int image_width,
                           unsigned int image_height,
						   unsigned char format)
{
    int ret = 0;
    struct jpeg_compress_struct com_cinfo;
    struct jpeg_error_mgr com_jerr;
    FILE * outfile;      /* target file */
    JSAMPROW row_pointer[1];  /* pointer to JSAMPLE row[s] 一行位图 */
    int row_stride;      /* physical row width in image buffer */
    unsigned char *rgb_buf = NULL;

	if(format == 0)
	{
		rgb_buf = (unsigned char *)malloc(image_width * image_height * 3);
		if(rgb_buf == NULL)
		{
			fprintf(stderr, "%s: malloc rgb_buf failed\n",__func__);
			return -1;
		}

		convert_UYVY_To_RGB(image_buffer,rgb_buf,image_width,image_height);
	}
	else
	{
		rgb_buf = image_buffer;
	}

    //Step 1: 申请并初始化jpeg压缩对象，同时要指定错误处理器
    com_cinfo.err = jpeg_std_error(&com_jerr);

    //Now we can initialize the JPEG compression object.
    jpeg_create_compress(&com_cinfo);

    outfile = fopen(file_name, "wb");

    if(outfile == NULL) 
    {
        fprintf(stderr, "%s: can't open %s\n",__func__,file_name);
        return -1;
    }

    jpeg_stdio_dest(&com_cinfo, outfile);

    //Step 3: 设置压缩参数
    com_cinfo.image_width = image_width;
    com_cinfo.image_height = image_height;
	if(format == 0)
	{
		com_cinfo.input_components = 3;             //3表示彩色位图，如果是灰度图则为1
		com_cinfo.in_color_space = JCS_RGB;         //JCS_RGB表示彩色图像,JCS_GRAYSCALE为灰度图
	}
	else
	{
		com_cinfo.input_components = 1;             //3表示彩色位图，如果是灰度图则为1
		com_cinfo.in_color_space = JCS_GRAYSCALE;   //JCS_RGB表示彩色图像,JCS_GRAYSCALE为灰度图
	}

    jpeg_set_defaults(&com_cinfo);
    jpeg_set_quality(&com_cinfo, quality, TRUE);

    //Step 4: Start compressor
    jpeg_start_compress(&com_cinfo, TRUE);

    //Step 5: while (scan lines remain to be written)
	if(format == 0)
	{
		row_stride = image_width * 3;                 //每一行的字节数,如果不是索引图,此处需要乘以3
	}
    else
	{
		row_stride = image_width * 1;                 //每一行的字节数,如果不是索引图,此处需要乘以3
	}

    while(com_cinfo.next_scanline < com_cinfo.image_height)
    {
        row_pointer[0] = &rgb_buf[com_cinfo.next_scanline * row_stride];  // image_buffer指向要压缩的数据  
        jpeg_write_scanlines(&com_cinfo, row_pointer, 1);
    }

	if(format == 0)
	{
		free(rgb_buf);
    	rgb_buf = NULL;
	}

    //Step 6: Finish compression
    jpeg_finish_compress(&com_cinfo);
    fclose(outfile);

    //Step 7: release JPEG compression object
    jpeg_destroy_compress(&com_cinfo);

    return ret;
}

static void syncAndMutexCreate(void)
{
    pthread_mutex_init(&mutexImageHeap, NULL);
    pthread_mutex_init(&mutexImuAdis16505Heap, NULL);
    pthread_mutex_init(&mutexImuMpu9250Heap, NULL);
    pthread_mutex_init(&mutexGnssUb482Heap, NULL);
    pthread_cond_init(&condImageHeap, NULL);
}

static int pthreadCreate(void *args)
{
    int ret = 0;
    static pthread_t tid_ub482 = 0;
    static pthread_t tid_net = 0;
    static pthread_t tid_mpu9250 = 0;
    static pthread_t tid_ui3240 = 0;
    static pthread_t tid_cssc132 = 0;
    static pthread_t tid_sync_module = 0;
    static pthread_t tid_sync = 0;
    static pthread_t tid_led = 0;

	static pthread_t tid_image_handler = 0;
	static pthread_t tid_imu_ads16505_handler = 0;
	static pthread_t tid_imu_mpu9250_handler = 0;
	static pthread_t tid_gnss_ub482_handler = 0;

    ret = pthread_create(&tid_ub482,NULL,thread_ub482,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_ub482 failed\n",__func__);
    }

    ret = pthread_create(&tid_net,NULL,thread_net,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_net failed\n",__func__);
    }

    ret = pthread_create(&tid_mpu9250,NULL,thread_mpu9250,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_mpu9250 failed\n",__func__);
    }

    if(cmdArgs.camera_module == 0)
    {
        ret = pthread_create(&tid_ui3240,NULL,thread_ui3240,&cmdArgs);
        if(0 != ret)
        {
            fprintf(stderr, "%s: create thread_ui3240 failed\n",__func__);
        }
    }
    else
    {
        ret = pthread_create(&tid_cssc132,NULL,thread_cssc132,&cmdArgs);
        if(0 != ret)
        {
            fprintf(stderr, "%s: create thread_cssc132 failed\n",__func__);
        }
    }

    ret = pthread_create(&tid_sync_module,NULL,thread_sync_module,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_sync_module failed\n",__func__);
    }

    ret = pthread_create(&tid_sync,NULL,thread_sync,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_sync failed\n",__func__);
    }

    ret = pthread_create(&tid_led,NULL,thread_led,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_led failed\n",__func__);
    }

	ret = pthread_create(&tid_image_handler,NULL,thread_image_handler,NULL);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_image_handler failed\n",__func__);
    }

	ret = pthread_create(&tid_imu_ads16505_handler,NULL,thread_imu_ads16505_handler,NULL);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_imu_ads16505_handler failed\n",__func__);
    }

	ret = pthread_create(&tid_imu_mpu9250_handler,NULL,thread_imu_mpu9250_handler,NULL);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_imu_mpu9250_handler failed\n",__func__);
    }

	ret = pthread_create(&tid_gnss_ub482_handler,NULL,thread_gnss_ub482_handler,NULL);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_gnss_ub482_handler failed\n",__func__);
    }

    return ret;
}

int monocular_sdk_init(int argc, char **argv)
{
    int ret = 0;

    ret = cmdParse(argc, argv, &cmdArgs);           //解析命令
    if(ret != 1)
    {
        fprintf(stderr, "%s: parse shell cmd failed\n",__func__);
    }

    clearSystemQueueMsg();

    syncAndMutexCreate();
    pthreadCreate(&cmdArgs);

    return ret;
}

void monocular_sdk_register_handler(ImageHandler image_handler,
                                    ImuAds16505Handler imu_ads16505_handler,
									ImuMpu9250Handler imu_mpu9250_handler,
									GnssUb482Handler gnss_ub482_handler)
{
	dataHandler.image_handler        = image_handler;
	dataHandler.imu_ads16505_handler = imu_ads16505_handler;
	dataHandler.imu_mpu9250_handler  = imu_mpu9250_handler;
	dataHandler.gnss_ub482_handler   = gnss_ub482_handler;
}