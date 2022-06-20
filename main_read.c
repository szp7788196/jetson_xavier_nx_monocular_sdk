#include "monocular.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


static struct ImageUnit *image = NULL;
static struct SyncImuData *imuAds16505 = NULL;
static struct Mpu9250SampleData *imuMpu9250 = NULL;
static struct Ub482GnssData *gnssUb482 = NULL;

int main(int argc, char **argv)
{
    int ret = 0;

    monocular_sdk_init(argc, argv);

    image       = (struct ImageUnit *)malloc(sizeof(struct ImageUnit));
    imuAds16505 = (struct SyncImuData *)malloc(sizeof(struct SyncImuData));
    imuMpu9250  = (struct Mpu9250SampleData *)malloc(sizeof(struct Mpu9250SampleData));
    gnssUb482   = (struct Ub482GnssData *)malloc(sizeof(struct Ub482GnssData));

    if(image == NULL || imuAds16505 == NULL || imuMpu9250 == NULL || gnssUb482 == NULL)
    {
        fprintf(stderr, "%s: alloc data memory failed\n",__func__);

        return -1;
    }

    while(1)
    {
        ret = imageHeapGet(image);
        if(ret == 0)
        {
            ret = imageBufCompressToJpeg("/home/szp/test_image.jpg",
                                        80,
                                        (unsigned char *)image->image->image,
                                        1280,
                                        720,
                                        1);
            if(ret != 0)
            {
                fprintf(stderr, "%s: compress image buf to jpeg picture failed\n",__func__);
            }

            printf("\r\n");
            printf("======================= image timestamp start =======================\n");
            printf("* image->time_stamp->time_stamp_local : %lf\n",image->time_stamp->time_stamp_local);
            printf("* image->time_stamp->time_stamp_gnss  : %lf\n",image->time_stamp->time_stamp_gnss);
            printf("* image->time_stamp->counter          : %d\n",image->time_stamp->counter);
            printf("======================== image timestamp end ========================\n");
        }

        ret = imuAdis16505HeapGet(imuAds16505);
        if(ret == 0)
        {
            printf("\r\n");
            printf("======================== imu ads16505 start =========================\n");
            printf("* imuAds16505->time_stamp_local : %lf\n",imuAds16505->time_stamp_local);
            printf("* imuAds16505->time_stamp_gnss  : %lf\n",imuAds16505->time_stamp_gnss);
            printf("* imuAds16505->counter          : %d\n",imuAds16505->counter);
            printf("* imuAds16505->gx               : %d\n",imuAds16505->gx);
            printf("* imuAds16505->gy               : %d\n",imuAds16505->gy);
            printf("* imuAds16505->gz               : %d\n",imuAds16505->gz);
            printf("* imuAds16505->ax               : %d\n",imuAds16505->ax);
            printf("* imuAds16505->ay               : %d\n",imuAds16505->ay);
            printf("* imuAds16505->az               : %d\n",imuAds16505->az);
            printf("========================= imu ads16505 end ==========================\n");
        }

        ret = imuMpu9250HeapGet(imuMpu9250);
        if(ret == 0)
        {
            printf("\r\n");
            printf("========================= imu mpu9250 start =========================\n");
            printf("* imuMpu9250->accel_x     : %d\n",imuMpu9250->accel_x);
            printf("* imuMpu9250->accel_y     : %d\n",imuMpu9250->accel_y);
            printf("* imuMpu9250->accel_z     : %d\n",imuMpu9250->accel_z);
            printf("* imuMpu9250->temperature : %d\n",imuMpu9250->temperature);
            printf("* imuMpu9250->gyro_x      : %d\n",imuMpu9250->gyro_x);
            printf("* imuMpu9250->gyro_y      : %d\n",imuMpu9250->gyro_y);
            printf("* imuMpu9250->gyro_z      : %d\n",imuMpu9250->gyro_z);
            printf("* imuMpu9250->magne_x     : %d\n",imuMpu9250->magne_x);
            printf("* imuMpu9250->magne_y     : %d\n",imuMpu9250->magne_y);
            printf("* imuMpu9250->magne_z     : %d\n",imuMpu9250->magne_z);
            printf("========================== imu mpu9250 end ==========================\n");
        }

        ret = gnssUb482HeapGet(gnssUb482);
        if(ret == 0)
        {
            printf("\r\n");
            printf("========================== gnss ub482 start =========================\n");
            printf("* gnssUb482->time_stamp : %lf\n",gnssUb482->time_stamp);
            printf("* gnssUb482->pos_type   : %d\n",(unsigned char)gnssUb482->pos_type);
            printf("* gnssUb482->vel_type   : %d\n",(unsigned char)gnssUb482->vel_type);
            printf("* gnssUb482->att_type   : %d\n",(unsigned char)gnssUb482->att_type);
            printf("* gnssUb482->lat        : %lf\n",gnssUb482->lat);
            printf("* gnssUb482->lon        : %lf\n",gnssUb482->lon);
            printf("* gnssUb482->height     : %lf\n",gnssUb482->height);
            printf("* gnssUb482->lat_std    : %lf\n",gnssUb482->lat_std);
            printf("* gnssUb482->lon_std    : %lf\n",gnssUb482->lon_std);
            printf("* gnssUb482->height_std : %lf\n",gnssUb482->height_std);
            printf("* gnssUb482->vx         : %lf\n",gnssUb482->vx);
            printf("* gnssUb482->vy         : %lf\n",gnssUb482->vy);
            printf("* gnssUb482->vz         : %lf\n",gnssUb482->vz);
            printf("* gnssUb482->vx_std     : %lf\n",gnssUb482->vx_std);
            printf("* gnssUb482->vy_std     : %lf\n",gnssUb482->vy_std);
            printf("* gnssUb482->vz_std     : %lf\n",gnssUb482->vz_std);
            printf("* gnssUb482->pitch      : %f\n",gnssUb482->pitch);
            printf("* gnssUb482->roll       : %f\n",gnssUb482->roll);
            printf("* gnssUb482->yaw        : %f\n",gnssUb482->yaw);
            printf("* gnssUb482->pitch_std  : %f\n",gnssUb482->pitch_std);
            printf("* gnssUb482->roll_std   : %f\n",gnssUb482->roll_std);
            printf("* gnssUb482->yaw_std    : %f\n",gnssUb482->yaw_std);
            printf("========================== gnss ub482 end ===========================\n");
        }

        usleep(1000 * 500);
    }

	return 0;
}
