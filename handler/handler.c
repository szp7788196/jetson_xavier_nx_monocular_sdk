#include "handler.h"
#include "monocular.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *thread_image_handler(void *arg)
{
    int ret = 0;
    int cpd = 0;
    struct ImageUnit *image_unit = NULL;
    struct timespec start_tm;
	struct timespec end_tm;
    int timeout_ms = 10;

    while(1)
    {
        clock_gettime(CLOCK_REALTIME, &start_tm);
        end_tm = ns_to_tm(tm_to_ns(start_tm) + timeout_ms * 1000000);

        if(sem_timedwait(&sem_t_ImageUnitHeap, &end_tm) == 0)
        {
            pthread_mutex_lock(&mutexImageUnitHeap);

            if(imageUnitHeap.cnt > 0)
            {
                cpd = copyImageUnit(imageUnitHeap.heap[imageUnitHeap.get_ptr],&image_unit);

                imageUnitHeap.get_ptr = (imageUnitHeap.get_ptr + 1) % imageUnitHeap.depth;

		        imageUnitHeap.cnt -= 1;

                cpd = 1;
            }

            pthread_mutex_unlock(&mutexImageUnitHeap);
        }

        if(dataHandler.image_handler != NULL)
        {
            if(cpd == 1)
            {
                cpd = 0;

                ret = dataHandler.image_handler(image_unit);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.image_handler error\n",__func__);
                }
            }
        }

        if(image_unit != NULL)
        {
            freeImageUnit(&image_unit);
            image_unit = NULL;
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
                ret = dataHandler.imu_ads16505_handler(sync_imu_data);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.sync_imu_data error\n",__func__);
                }
            }

            free(sync_imu_data);
            sync_imu_data = NULL;
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
                ret = dataHandler.imu_mpu9250_handler(mpu9250_sample_data);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.mpu9250_sample_data error\n",__func__);
                }
            }

            free(mpu9250_sample_data);
            mpu9250_sample_data = NULL;
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
                ret = dataHandler.gnss_ub482_handler(ub482_gnss_data);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.ub482_gnss_data error\n",__func__);
                }
            }

            free(ub482_gnss_data);
            ub482_gnss_data = NULL;
        }
    }
}

void *thread_ephemeris_ub482_handler(void *arg)
{
    int ret = 0;
    struct Ephemeris *ephemeris = NULL;

    while(1)
    {
        ret = xQueueReceive((key_t)KEY_EPHEMERIS_MSG,(void **)&ephemeris,1);
        if(ret == -1)
        {
            fprintf(stderr, "%s: recv KEY_EPHEMERIS_MSG error\n",__func__);
        }
        else
        {
            if(dataHandler.ephemeris_ub482_handler != NULL)
            {
                ret = dataHandler.ephemeris_ub482_handler(ephemeris);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.ephemeris error\n",__func__);
                }
            }

            switch(ephemeris->flag)
            {
                case 0x01:
                    ephemeris->ephem = (struct GLOEPHEM *)ephemeris->ephem;
                break;

                case 0x02:
                    ephemeris->ephem = (struct GPSEPHEM *)ephemeris->ephem;
                break;

                case 0x04:
                    ephemeris->ephem = (struct BDSEPHEM *)ephemeris->ephem;
                break;

                case 0x08:
                    ephemeris->ephem = (struct GALEPHEM *)ephemeris->ephem;
                break;

                default:
                break;
            }

            free(ephemeris->ephem);
            ephemeris->ephem = NULL;

            free(ephemeris);
            ephemeris = NULL;
        }
    }
}

void *thread_rangeh_ub482_handler(void *arg)
{
    int ret = 0;
    struct Rangeh *rangeh = NULL;
    unsigned char i = 0;

    while(1)
    {
        ret = xQueueReceive((key_t)KEY_RANGEH_MSG,(void **)&rangeh,1);
        if(ret == -1)
        {
            fprintf(stderr, "%s: recv KEY_RANGEH_MSG error\n",__func__);
        }
        else
        {
            if(dataHandler.rangeh_ub482_handler != NULL)
            {
                ret = dataHandler.rangeh_ub482_handler(rangeh);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: dataHandler.rangeh error\n",__func__);
                }
            }

            for(i = 0; i < rangeh->satellite_num; i ++)
            {
                free(rangeh->data[i]);
                rangeh->data[i] = NULL;
            }

            free(rangeh->data);
            rangeh->data = NULL;

            free(rangeh);
            rangeh = NULL;
        }
    }
}