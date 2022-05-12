#include "monocular.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "ui3240.h"
#include <signal.h>

int imageHandler(struct ImageHeapUnit *image)
{
    int ret = 0;
/*
    ret = imageBufCompressToJpeg("/home/szp/test_image.jpg",80,(unsigned char *)image->image->image,1280,720,1);
    if(ret != 0)
    {
        fprintf(stderr, "%s: compress image buf to jpeg picture failed\n",__func__);
    }
 */
    printf("\r\n");
    printf("======================= image timestamp start =======================\n");
    printf("* image->time_stamp->time_stamp_local : %lf\n",image->time_stamp->time_stamp_local);
    printf("* image->time_stamp->time_stamp_gnss  : %lf\n",image->time_stamp->time_stamp_gnss);
    printf("* image->time_stamp->counter          : %d\n",image->time_stamp->counter);
    printf("* image->image->counter               : %d\n",image->image->counter);
//    printf("* image->put_ptr                      : %d\n",image->put_ptr);
    printf("======================== image timestamp end ========================\n");

    return ret;
}

int imuAds16505Handler(struct SyncImuData *imuAds16505)
{
    int ret = 0;

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

    sleep(1);

    return ret;
}

int imuMpu9250Handler(struct Mpu9250SampleData *imuMpu9250)
{
    int ret = 0;

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

    sleep(1);

    return ret;
}

int gnssUb482Handler(struct Ub482GnssData *gnssUb482)
{
    int ret = 0;

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

    return ret;
}

int main(int argc, char **argv)
{
    int ret = 0;
    char imei[16] = {0};

    monocular_sdk_init(argc, argv);

    monocular_sdk_register_handler(imageHandler,NULL,NULL,NULL);

    signal(SIGINT, pthread_ui3240_exit);

/*     ret = queryEC20_IMEI(imei);
    if(ret == 0)
    {
        printf("******************************* %s\n",imei);
    } */

    while(1)
    {
        usleep(1000 * 1000);
    }

	return 0;
}
