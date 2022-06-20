#include "mpu9250.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "cmd_parse.h"
#include "monocular.h"


void *thread_mpu9250(void *arg)
{
    int ret = 0;
    int fd;
    fd_set readfds;
    struct timeval timeout;
    struct CmdArgs *args = (struct CmdArgs *)arg;
    struct Mpu9250Config config;

    config.gyro_range  = args->gyro_range;
    config.accel_range = args->accel_range;
    config.sample_rate = args->sample_rate;
    config.read_mode   = args->read_mode;

    /* 打开设备 */
	fd = open("/dev/mpu9250", O_RDWR | O_NONBLOCK);
	if(0 > fd)
    {
        fprintf(stderr, "%s: open /dev/mpu9250 failed\n",__func__);
		goto THREAD_EXIT;
	}

    ret = ioctl(fd,IOC_W_MPU9250_CONFIG,&config);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl operate failed\n",__func__);
    }

    allocateImuMpu9250Heap(args->imu_heap_depth);

    while(1)
    {
        FD_ZERO(&readfds);
		FD_SET(fd, &readfds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 500000; /* 500ms */

        ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
        switch(ret)
        {
            case -1:
                fprintf(stderr, "%s: select err\n",__func__);
            break;

            case 0:
                fprintf(stderr, "%s: select time out\n",__func__);
            break;

            default:
            {
                struct Mpu9250SampleData *mpu9250_sample_data = NULL;

                mpu9250_sample_data = (struct Mpu9250SampleData *)malloc(sizeof(struct Mpu9250SampleData));
                if(mpu9250_sample_data != NULL)
                {
                    ret = read(fd, mpu9250_sample_data, sizeof(struct Mpu9250SampleData));
                    if(ret < 0)
                    {
                        free(mpu9250_sample_data);
                        mpu9250_sample_data = NULL;

                        fprintf(stderr, "%s: read mpu9250 failed\n",__func__);
                    }
                    else
                    {
                        imuMpu9250HeapPut(mpu9250_sample_data);

                        ret = xQueueSend((key_t)KEY_IMU_MPU9250_HANDLER_MSG,mpu9250_sample_data,MAX_QUEUE_MSG_NUM);
                        if(ret == -1)
                        {
                            fprintf(stderr, "%s: send mpu9250_sample_data queue msg failed\n",__func__);
                        }
                    }
                }
            }

            break;
        }
    }

THREAD_EXIT:
    pthread_exit("thread_mpu9250: open serial port failed\n");
}