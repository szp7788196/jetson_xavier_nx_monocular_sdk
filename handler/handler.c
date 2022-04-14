#include "handler.h"
#include "monocular.h"
#include <stdio.h>

void *thread_image_handler(void *arg)
{
    int ret = 0;
    struct ImageHeapUnit *image_heap_unit = NULL;

    while(1)
    {
        ret = xQueueReceive((key_t)KEY_IMAGE_HANDLER_MSG,(void **)&image_heap_unit,1);
        if(ret == -1)
        {
            fprintf(stderr, "%s: recv KEY_IMAGE_HANDLER_MSG error\n",__func__);
        }
        else
        {
            if(dataHandler.image_handler != NULL)
            {
                pthread_mutex_lock(&mutexImageHeap);

                ret = dataHandler.image_handler(image_heap_unit);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.image_handler error\n",__func__);
                }

                pthread_mutex_unlock(&mutexImageHeap);
            }
        }
    }
}

void *thread_imu_ads16505_handler(void *arg)
{
    int ret = 0;
    struct SyncImuData *sync_imu_data = NULL;

    while(1)
    {
        ret = xQueueReceive((key_t)KEY_IMU_ADS16505_HANDLER_MSG,(void **)&sync_imu_data,1);
        if(ret == -1)
        {
            fprintf(stderr, "%s: recv KEY_IMU_ADS16505_HANDLER_MSG error\n",__func__);
        }
        else
        {
            if(dataHandler.imu_ads16505_handler != NULL)
            {
                pthread_mutex_lock(&mutexImuAdis16505Heap);

                ret = dataHandler.imu_ads16505_handler(sync_imu_data);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.sync_imu_data error\n",__func__);
                }

                pthread_mutex_unlock(&mutexImuAdis16505Heap);
            }
        }
    }
}

void *thread_imu_mpu9250_handler(void *arg)
{
    int ret = 0;
    struct Mpu9250SampleData *mpu9250_sample_data = NULL;

    while(1)
    {
        ret = xQueueReceive((key_t)KEY_IMU_MPU9250_HANDLER_MSG,(void **)&mpu9250_sample_data,1);
        if(ret == -1)
        {
            fprintf(stderr, "%s: recv KEY_IMU_MPU9250_HANDLER_MSG error\n",__func__);
        }
        else
        {
            if(dataHandler.imu_mpu9250_handler != NULL)
            {
                pthread_mutex_lock(&mutexImuMpu9250Heap);

                ret = dataHandler.imu_mpu9250_handler(mpu9250_sample_data);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.mpu9250_sample_data error\n",__func__);
                }

                pthread_mutex_unlock(&mutexImuMpu9250Heap);
            }
        }
    }
}

void *thread_gnss_ub482_handler(void *arg)
{
    int ret = 0;
    struct Ub482GnssData *ub482_gnss_data = NULL;

    while(1)
    {
        ret = xQueueReceive((key_t)KEY_GNSS_UB482_HANDLER_MSG,(void **)&ub482_gnss_data,1);
        if(ret == -1)
        {
            fprintf(stderr, "%s: recv KEY_GNSS_UB482_HANDLER_MSG error\n",__func__);
        }
        else
        {
            if(dataHandler.gnss_ub482_handler != NULL)
            {
                pthread_mutex_lock(&mutexGnssUb482Heap);

                ret = dataHandler.gnss_ub482_handler(ub482_gnss_data);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.ub482_gnss_data error\n",__func__);
                }

                pthread_mutex_unlock(&mutexGnssUb482Heap);
            }
        }
    }
}