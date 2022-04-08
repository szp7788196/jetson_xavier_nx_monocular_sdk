#include "common.h"

struct ImageHeap imageHeap = {NULL,0,0};
struct ImuAdis16505Heap imuAdis16505Heap = {NULL,0,0};
struct ImuMpu9250Heap imuMpu9250Heap = {NULL,0,0};

pthread_mutex_t mutexImageHeap;
pthread_mutex_t mutexImuAdis16505Heap;
pthread_cond_t condImageHeap;

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
unsigned short mystrstr(unsigned char *str1, unsigned char *str2, unsigned short str1_len, unsigned short str2_len)
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

unsigned short find_str(unsigned char *s_str, unsigned char *p_str, unsigned short count, unsigned short *seek)
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

unsigned short get_str1(unsigned char *source, unsigned char *begin, unsigned short count1, unsigned char *end, unsigned short count2, unsigned char *out)
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

unsigned short get_str2(unsigned char *source, unsigned char *begin, unsigned short count, unsigned short length, unsigned char *out)
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

int xQueueSend(key_t queue_key,void *msg_to_queue)
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

	if(queue_info.msg_qnum >= MAX_QUEUE_MSG_NUM)
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

void freeImageHeap(unsigned short depth)
{
	int i = 0;

	if(imageHeap.heap != NULL)
    {
        for(i = 0; i < depth; i ++)
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

	imageHeap.heap = calloc(depth, sizeof(struct ImageHeapUnit));

	if(imageHeap.heap == NULL)
	{
		fprintf(stderr, "%s: calloc imageHeap.heap failed\n",__func__);
		return -1;
	}

	for(i = 0; i < depth; i ++)
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

void freeImuAdis16505Heap(unsigned short depth)
{
	int i = 0;

	if(imuAdis16505Heap.heap != NULL)
	{
		for(i = 0; i < depth; i ++)
		{
			if(imuAdis16505Heap.heap[i] != NULL)
			{
				free(imuAdis16505Heap.heap[i]);
				imuAdis16505Heap.heap[i] = NULL;
			}
		}

		imuAdis16505Heap.heap = NULL;
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

	imuAdis16505Heap.heap = calloc(depth, sizeof(struct SyncImuData));

	if(imuAdis16505Heap.heap == NULL)
	{
		fprintf(stderr, "%s: calloc imuAdis16505Heap.heap failed\n",__func__);
		return -1;
	}

	for(i = 0; i < depth; i ++)
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

void freeImuMpu9250Heap(unsigned short depth)
{
	int i = 0;

	if(imuMpu9250Heap.heap != NULL)
	{
		for(i = 0; i < depth; i ++)
		{
			if(imuMpu9250Heap.heap[i] != NULL)
			{
				free(imuMpu9250Heap.heap[i]);
				imuMpu9250Heap.heap[i] = NULL;
			}
		}

		imuMpu9250Heap.heap = NULL;
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

	imuMpu9250Heap.heap = calloc(depth, sizeof(struct Mpu9250SampleData));

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
}
