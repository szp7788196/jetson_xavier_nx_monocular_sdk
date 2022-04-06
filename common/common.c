#include "common.h"


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
