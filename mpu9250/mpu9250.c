#include "mpu9250.h"

struct Mpu9250SampleData mpu9250SampleData;

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

    /* 构造超时时间 */
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; /* 500ms */

    while(1)
    {
        FD_ZERO(&readfds);
		FD_SET(fd, &readfds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 50000; /* 500ms */

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
                ret = read(fd, &mpu9250SampleData, sizeof(struct Mpu9250SampleData));
                if(ret < 0)
                {
                    fprintf(stderr, "%s: read mpu9250 failed\n",__func__);
                }
                else
                {
                    imuMpu9250HeapPut(&mpu9250SampleData);

/*                     printf("*******************************************\n");
                    printf("* accel_x     : %d\n",mpu9250SampleData.accel_x);
                    printf("* accel_y     : %d\n",mpu9250SampleData.accel_y);
                    printf("* accel_z     : %d\n",mpu9250SampleData.accel_z);
                    printf("* temperature : %d\n",mpu9250SampleData.temperature);
                    printf("* gyro_x      : %d\n",mpu9250SampleData.gyro_x);
                    printf("* gyro_y      : %d\n",mpu9250SampleData.gyro_y);
                    printf("* gyro_z      : %d\n",mpu9250SampleData.gyro_z);
                    printf("* magne_x     : %d\n",mpu9250SampleData.magne_x);
                    printf("* magne_y     : %d\n",mpu9250SampleData.magne_y);
                    printf("* magne_z     : %d\n",mpu9250SampleData.magne_z);
                    printf("*******************************************\n"); */
                }
            break;
        }
    }

THREAD_EXIT:
    pthread_exit("thread_mpu9250: open serial port failed\n");
}