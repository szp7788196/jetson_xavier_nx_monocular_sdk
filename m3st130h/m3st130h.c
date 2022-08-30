#include "m3st130h.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "monocular.h"
#include "cmd_parse.h"

static struct M3st130hConfig cameraConfig;
static struct M3st130hConfig userM3st130hConfig;

static void initCameraConfig(struct M3st130hConfig *config,struct CmdArgs *args)
{
    if(config == NULL || args == NULL)
    {
        fprintf(stderr, "%s: input paras is NULL\n",__func__);
        return;
    }

    memset(config,0,sizeof(struct M3st130hConfig));

    config->capture_timeout 	= args->capture_timeout;
	config->frame_num 			= args->frame_num;
	config->image_heap_depth 	= args->image_heap_depth;
}

static void printCameraConfig(struct M3st130hConfig config)
{
    printf("|==================== M3st130hConfig begin ====================\n");
    printf("| camera_handle                     = %d\n",config.camera_handle);
    printf("| camera_id                         = %d\n",config.camera_id);
    printf("| capture_timeout                   = %d\n",config.capture_timeout);
    printf("| frame_num                         = %d\n",config.frame_num);
    printf("| image_heap_depth                  = %d\n",config.image_heap_depth);
    printf("| image_width                       = %d\n",config.image_width);
    printf("| image_height                      = %d\n",config.image_height);
    printf("| image_size                        = %d\n",config.image_size);
    printf("| bytes_per_pix                     = %d\n",config.bytes_per_pix);
    printf("| frame_buf_size                    = %d\n",config.frame_buf_size);
	printf("| camera_info.Vendor                = %s\n",config.camera_info.Vendor);
	printf("| camera_info.LinkName              = %s\n",config.camera_info.LinkName);
	printf("| camera_info.SensorInfo            = %s\n",config.camera_info.SensorInfo);
	printf("| camera_info.HardwareVersion       = %s\n",config.camera_info.HardwareVersion);
	printf("| camera_info.FirmwareVersion       = %s\n",config.camera_info.FirmwareVersion);
	printf("| camera_info.KernelVersion         = %s\n",config.camera_info.KernelVersion);
	printf("| camera_info.DscamVersion          = %s\n",config.camera_info.DscamVersion);
	printf("| camera_info.FriendlyName          = %s\n",config.camera_info.FriendlyName);
	printf("| camera_info.PortInfo              = %s\n",config.camera_info.PortInfo);
	printf("| camera_info.SerialNumber          = %s\n",config.camera_info.SerialNumber);
	printf("| camera_info.CameraInfo            = %s\n",config.camera_info.CameraInfo);
	printf("| camera_info.UserID                = %s\n",config.camera_info.UserID);
	printf("| camera_info.OriginalSerialNumber  = %s\n",config.camera_info.OriginalSerialNumber);
	printf("| auto_exposure_target_brightness   = %d\n",config.auto_exposure_target_brightness);
	printf("| analog_gain                       = %.3f\n",config.analog_gain);
	printf("| exposure_time                     = %.3f\n",config.exposure_time);
	printf("| exposure_roi.X                    = %d\n",config.exposure_roi.X);
	printf("| exposure_roi.Y                    = %d\n",config.exposure_roi.Y);
	printf("| exposure_roi.W                    = %d\n",config.exposure_roi.W);
	printf("| exposure_roi.H                    = %d\n",config.exposure_roi.H);
	printf("| auto_exposure_mode                = %d\n",(unsigned char)config.auto_exposure_mode);
	printf("| anti_flick                        = %d\n",(unsigned char)config.anti_flick);
	printf("| auto_exposure_operate_mode        = %d\n",(unsigned char)config.auto_exposure_operate_mode);
	printf("| auto_exposure_conf_info.ExpMin    = %.3f\n",config.auto_exposure_conf_info.fExposureMin);
	printf("| auto_exposure_conf_info.ExpMax    = %.3f\n",config.auto_exposure_conf_info.fExposureMax);
	printf("| auto_exposure_conf_info.GainMin   = %.3f\n",config.auto_exposure_conf_info.fGainMin);
	printf("| auto_exposure_conf_info.GainMax   = %.3f\n",config.auto_exposure_conf_info.fGainMax);
	printf("| color_temperature                 = %d\n",config.color_temperature);
	printf("| color_temperature_state           = %d\n",(unsigned char)config.color_temperature_state);
	printf("| saturation                        = %d\n",config.saturation);
	printf("| saturation_state                  = %d\n",(unsigned char)config.saturation_state);
	printf("| awb_roi.X                         = %d\n",config.awb_roi.X);
	printf("| awb_roi.Y                         = %d\n",config.awb_roi.Y);
	printf("| awb_roi.W                         = %d\n",config.awb_roi.W);
	printf("| awb_roi.H                         = %d\n",config.awb_roi.H);
	printf("| awb_operate                       = %d\n",(unsigned char)config.awb_operate);
	printf("| red_digital_gain                  = %.3f\n",config.red_digital_gain);
	printf("| green_digital_gain                = %.3f\n",config.green_digital_gain);
	printf("| blue_digital_gain                 = %.3f\n",config.blue_digital_gain);
	printf("| rgb_digital_gain_state            = %d\n",(unsigned char)config.rgb_digital_gain_state);
	printf("| gamma                             = %d\n",config.gamma);
	printf("| gamma_state                       = %d\n",(unsigned char)config.gamma_state);
	printf("| contrast                          = %d\n",config.contrast);
	printf("| contrast_state                    = %d\n",(unsigned char)config.contrast_state);
	printf("| sharpness                         = %d\n",config.sharpness);
	printf("| sharpness_state                   = %d\n",(unsigned char)config.sharpness_state);
	printf("| noise_reduct_2d                   = %d\n",config.noise_reduct_2d);
	printf("| noise_reduct_2d_state             = %d\n",(unsigned char)config.noise_reduct_2d_state);
	printf("| noise_reduct_3d                   = %d\n",config.noise_reduct_3d);
	printf("| noise_reduct_3d_state             = %d\n",(unsigned char)config.noise_reduct_3d_state);
	printf("| black_level                       = %.3f\n",config.black_level);
	printf("| black_level_state                 = %d\n",(unsigned char)config.black_level_state);
	printf("| mono_state                        = %d\n",(unsigned char)config.mono_state);
	printf("| inverse_state                     = %d\n",(unsigned char)config.inverse_state);
	printf("| flip_horizontal_state             = %d\n",(unsigned char)config.flip_horizontal_state);
	printf("| flip_vertical_state               = %d\n",(unsigned char)config.flip_vertical_state);
	printf("| rotate_state                      = %d\n",(unsigned char)config.rotate_state);
	printf("| rotate_opposite                   = %d\n",(unsigned char)config.rotate_opposite);
	printf("| roi.X                             = %d\n",config.roi.X);
	printf("| roi.Y                             = %d\n",config.roi.Y);
	printf("| roi.W                             = %d\n",config.roi.W);
	printf("| roi.H                             = %d\n",config.roi.H);
	printf("| roi_state                         = %d\n",(unsigned char)config.roi_state);
	printf("| source_format                     = %d\n",(unsigned char)config.source_format);
	printf("| target_format                     = %d\n",(unsigned char)config.target_format);
	printf("| soft_trigger_loop_time            = %.3f\n",config.soft_trigger_loop_time);
	printf("| soft_trigger_loop_state           = %d\n",(unsigned char)config.soft_trigger_loop_state);
	printf("| ext_trigger_jitter_filter         = %.3f\n",config.ext_trigger_jitter_filter);
	printf("| ext_trigger_delay                 = %.3f\n",config.ext_trigger_delay);
	printf("| strobe_delay                      = %.3f\n",config.strobe_delay);
	printf("| strobe_duration                   = %.3f\n",config.strobe_duration);
	printf("| frames_per_trigger                = %d\n",(unsigned char)config.frames_per_trigger);
	printf("| ext_trigger_source                = %d\n",(unsigned char)config.ext_trigger_source);
	printf("| trigger_state                     = %d\n",(unsigned char)config.trigger_state);
	printf("| strobe_driver                     = %d\n",(unsigned char)config.strobe_driver);
	printf("| strobe_output_type                = %d\n",(unsigned char)config.strobe_output_type);
	printf("| ext_trigger_input_type            = %d\n",(unsigned char)config.ext_trigger_input_type);
    printf("| trigger_frame_rate                = %.3f\n",config.trigger_frame_rate);

    printf("|===================== M3st130hConfig end =====================\n");
}

static int connectCamrea(struct M3st130hConfig *config)
{
    dvpStatus dvp_status = DVP_STATUS_OK;
    unsigned int cam_cnt = 0;

    dvp_status = dvpRefresh(&cam_cnt);
    if(dvp_status != DVP_STATUS_OK || cam_cnt != 1)
    {
        fprintf(stderr,"%s: no camera found\n",__func__);
        return -1;
    }

    dvp_status = dvpEnum(0, &config->camera_info);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get camera info failed\n",__func__);
        return -1;
    }

    dvp_status = dvpOpenByName(config->camera_info.FriendlyName, OPEN_NORMAL, &config->camera_handle);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: open camera: %s failed\n",__func__,config->camera_info.FriendlyName);
        return -1;
    }

    return 0;
}

static void disconnectCamera(struct M3st130hConfig config)
{
    dvpStatus dvp_status = DVP_STATUS_OK;

    dvp_status = dvpStop(config.camera_handle);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: stop video stream failed\n",__func__);
    }

    dvp_status = dvpClose(config.camera_handle);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: close camera failed\n",__func__);
    }
}

static int loadCameraDefaultConfig(struct M3st130hConfig config,const char *filename)
{
    dvpStatus dvp_status = DVP_STATUS_OK;

    dvp_status = dvpLoadConfig(config.camera_handle,filename);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: load camera def config failed\n",__func__);
        return -1;
    }

    return 0;
}

static int queryCameraConfig(struct M3st130hConfig *config)
{
    dvpStatus dvp_status = DVP_STATUS_OK;

    // 曝光功能
    dvp_status = dvpGetAeTarget(config->camera_handle,&config->auto_exposure_target_brightness);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get auto_exposure_target_brightness failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetAnalogGain(config->camera_handle,&config->analog_gain);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get analog_gain failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetExposure(config->camera_handle,&config->exposure_time);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get exposure_time failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetAeRoi(config->camera_handle,&config->exposure_roi);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get exposure_roi failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetAeMode(config->camera_handle,&config->auto_exposure_mode);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get auto_exposure_mode failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetAntiFlick(config->camera_handle,&config->anti_flick);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get anti_flick failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetAeOperation(config->camera_handle,&config->auto_exposure_operate_mode);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get auto_exposure_operate_mode failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetAeConfig(config->camera_handle,&config->auto_exposure_conf_info);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get auto_exposure_conf_info failed\n",__func__);
        return -1;
    }

    // 色彩调节
    dvp_status = dvpGetColorTemperature(config->camera_handle,&config->color_temperature);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get color_temperature failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetColorTemperatureState(config->camera_handle,&config->color_temperature_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get color_temperature_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetSaturation(config->camera_handle,&config->saturation);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get saturation failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetSaturationState(config->camera_handle,&config->saturation_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get saturation_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetAwbRoi(config->camera_handle,&config->awb_roi);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get awb_roi failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetAwbOperation(config->camera_handle,&config->awb_operate);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get awb_operate failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetRgbGain(config->camera_handle,
                               &config->red_digital_gain,
                               &config->green_digital_gain,
                               &config->blue_digital_gain);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get rgb_digital_gain failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetRgbGainState(config->camera_handle,&config->rgb_digital_gain_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get rgb_digital_gain_state failed\n",__func__);
        return -1;
    }

    // 增强效果
    dvp_status = dvpGetGamma(config->camera_handle,&config->gamma);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get gamma failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetGammaState(config->camera_handle,&config->gamma_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get gamma_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetContrast(config->camera_handle,&config->contrast);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get contrast failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetContrastState(config->camera_handle,&config->contrast_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get contrast_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetSharpness(config->camera_handle,&config->sharpness);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get sharpness failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetSharpnessState(config->camera_handle,&config->sharpness_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get sharpness_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetNoiseReduct2d(config->camera_handle,&config->noise_reduct_2d);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get noise_reduct_2d failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetNoiseReduct2dState(config->camera_handle,&config->noise_reduct_2d_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get noise_reduct_2d_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetNoiseReduct3d(config->camera_handle,&config->noise_reduct_3d);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get noise_reduct_3d failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetNoiseReduct3dState(config->camera_handle,&config->noise_reduct_3d_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get noise_reduct_3d_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetBlackLevel(config->camera_handle,&config->black_level);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get black_level failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetBlackLevelState(config->camera_handle,&config->black_level_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get black_level_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetMonoState(config->camera_handle,&config->mono_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get mono_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetInverseState(config->camera_handle,&config->inverse_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get inverse_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetFlipHorizontalState(config->camera_handle,&config->flip_horizontal_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get flip_horizontal_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetFlipVerticalState(config->camera_handle,&config->flip_vertical_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get flip_vertical_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetRotateState(config->camera_handle,&config->rotate_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get rotate_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetRotateOpposite(config->camera_handle,&config->rotate_opposite);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get rotate_opposite failed\n",__func__);
        return -1;
    }

    // 图像尺寸
    dvp_status = dvpGetRoi(config->camera_handle,&config->roi);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get roi failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetRoiState(config->camera_handle,&config->roi_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get roi_state failed\n",__func__);
        return -1;
    }

    // 图像格式
    dvp_status = dvpGetSourceFormat(config->camera_handle,&config->source_format);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get source_format failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetTargetFormat(config->camera_handle,&config->target_format);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get target_format failed\n",__func__);
        return -1;
    }

    // 触发功能
    dvp_status = dvpGetSoftTriggerLoop(config->camera_handle,&config->soft_trigger_loop_time);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get soft_trigger_loop_time failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetSoftTriggerLoopState(config->camera_handle,&config->soft_trigger_loop_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get soft_trigger_loop_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetTriggerJitterFilter(config->camera_handle,&config->ext_trigger_jitter_filter);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get ext_trigger_jitter_filter failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetTriggerDelay(config->camera_handle,&config->ext_trigger_delay);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get ext_trigger_delay failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetStrobeDelay(config->camera_handle,&config->strobe_delay);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get strobe_delay failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetStrobeDuration(config->camera_handle,&config->strobe_duration);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get strobe_duration failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetFramesPerTrigger(config->camera_handle,&config->frames_per_trigger);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get frames_per_trigger failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetTriggerSource(config->camera_handle,&config->ext_trigger_source);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get ext_trigger_source failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetTriggerState(config->camera_handle,&config->trigger_state);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get trigger_state failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetStrobeDriver(config->camera_handle,&config->strobe_driver);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get strobe_driver failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetStrobeOutputType(config->camera_handle,&config->strobe_output_type);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get strobe_output_type failed\n",__func__);
        return -1;
    }

    dvp_status = dvpGetTriggerInputType(config->camera_handle,&config->ext_trigger_input_type);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: get ext_trigger_input_type failed\n",__func__);
        return -1;
    }

    return 0;
}

static int loadCameraUserConfig(struct M3st130hConfig *def_config,struct M3st130hConfig *user_config,char *filename)
{
    FILE *fp;
    char str[128] = {0};
    unsigned int file_len = 0l;
    char *file_buf = NULL;
    char *msg = NULL;
    unsigned short pos = 0;
    char temp_buf[32] = {0};
    long int temp = 0;
    double temp_f = 0.0;
    char *endptr = NULL;

    fp = fopen(filename, "rt");
    if(fp == NULL)
    {
        fprintf(stderr, "%s: Could not open camera user parameters file\n",__func__);
		return -1;
    }

    fseek(fp,0,SEEK_END);

    file_len = ftell(fp);
    if(file_len < 1)
    {
        fprintf(stderr, "%s: query camera user parameters file length failed\n",__func__);
        fclose(fp);
		return -1;
    }

    if(file_len > M3ST130H_MAX_USER_CONFIG_FILE_LEN || file_len < M3ST130H_MIN_USER_CONFIG_FILE_LEN)
    {
        fprintf(stderr, "%s: camera user parameters file length error\n",__func__);
        fclose(fp);
		return -1;
    }

    file_buf = (char *)malloc(sizeof(char) * (file_len + 1));
    if(file_buf == NULL)
    {
        fprintf(stderr, "%s: alloc user parameters file buf failed\n",__func__);
        fclose(fp);
		return -1;
    }

    memset(file_buf,0,file_len + 1);

    fseek(fp,0,SEEK_SET);

    while(fgets(str, 100, fp) != NULL)
    {
        strcat(file_buf,str);
    }

    fclose(fp);

    if(strstr(file_buf, "start:") == NULL && strstr(file_buf, "end;") == NULL)
    {
        fprintf(stderr, "%s: camera user parameters file missing head or tail\n",__func__);
        fclose(fp);
		return -1;
    }

    memcpy(user_config,def_config,sizeof(struct M3st130hConfig));

    msg = file_buf;

    // 曝光功能
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"auto_exposure_target_brightness", file_len, strlen("auto_exposure_target_brightness"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->auto_exposure_target_brightness = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"analog_gain", file_len, strlen("analog_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->analog_gain = temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"exposure_time", file_len, strlen("exposure_time"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->exposure_time = temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"exposure_roi_x", file_len, strlen("exposure_roi_x"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->exposure_roi.X = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"exposure_roi_y", file_len, strlen("exposure_roi_y"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->exposure_roi.Y = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"exposure_roi_w", file_len, strlen("exposure_roi_w"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->exposure_roi.W = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"exposure_roi_h", file_len, strlen("exposure_roi_h"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->exposure_roi.H = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"auto_exposure_mode", file_len, strlen("auto_exposure_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->auto_exposure_mode = (dvpAeMode)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"anti_flick", file_len, strlen("anti_flick"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->anti_flick = (dvpAntiFlick)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"auto_exposure_operate_mode", file_len, strlen("auto_exposure_operate_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->auto_exposure_operate_mode = (dvpAeOperation)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"auto_exposure_conf_info_min_time", file_len, strlen("auto_exposure_conf_info_min_time"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->auto_exposure_conf_info.fExposureMin = temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"auto_exposure_conf_info_max_time", file_len, strlen("auto_exposure_conf_info_max_time"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->auto_exposure_conf_info.fExposureMax = temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"auto_exposure_conf_info_min_gain", file_len, strlen("auto_exposure_conf_info_min_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->auto_exposure_conf_info.fGainMin = (float)temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"auto_exposure_conf_info_max_gain", file_len, strlen("auto_exposure_conf_info_max_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->auto_exposure_conf_info.fGainMax = (float)temp_f;
        }
    }

    // 色彩调节
    pos = mystrstr((unsigned char *)msg, (unsigned char *)"color_temperature", file_len, strlen("color_temperature"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->color_temperature = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"color_temperature_state", file_len, strlen("color_temperature_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->color_temperature_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"saturation", file_len, strlen("saturation"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->saturation = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"saturation_state", file_len, strlen("saturation_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->saturation_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"awb_roi_x", file_len, strlen("awb_roi_x"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->awb_roi.X = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"awb_roi_y", file_len, strlen("awb_roi_y"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->awb_roi.Y = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"awb_roi_w", file_len, strlen("awb_roi_w"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->awb_roi.W = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"awb_roi_h", file_len, strlen("awb_roi_h"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->awb_roi.H = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"awb_operate", file_len, strlen("awb_operate"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->awb_operate = (dvpAwbOperation)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"red_digital_gain", file_len, strlen("red_digital_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->red_digital_gain = (float)temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"green_digital_gain", file_len, strlen("green_digital_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->green_digital_gain = (float)temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"blue_digital_gain", file_len, strlen("blue_digital_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->blue_digital_gain = (float)temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"rgb_digital_gain_state", file_len, strlen("rgb_digital_gain_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->rgb_digital_gain_state = (bool)temp;
        }
    }

    // 增强效果
    pos = mystrstr((unsigned char *)msg, (unsigned char *)"gamma", file_len, strlen("gamma"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->gamma = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"gamma_state", file_len, strlen("gamma_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->gamma_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"contrast", file_len, strlen("contrast"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->contrast = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"contrast_state", file_len, strlen("contrast_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->contrast_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"sharpness", file_len, strlen("sharpness"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->sharpness = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"sharpness_state", file_len, strlen("sharpness_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->sharpness_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"noise_reduct_2d", file_len, strlen("noise_reduct_2d"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->noise_reduct_2d = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"noise_reduct_2d_state", file_len, strlen("noise_reduct_2d_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->noise_reduct_2d_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"noise_reduct_3d", file_len, strlen("noise_reduct_3d"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->noise_reduct_3d = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"noise_reduct_3d_state", file_len, strlen("noise_reduct_3d_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->noise_reduct_3d_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"black_level", file_len, strlen("black_level"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->black_level = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"black_level_state", file_len, strlen("black_level_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->black_level_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"mono_state", file_len, strlen("mono_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->mono_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"inverse_state", file_len, strlen("inverse_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->inverse_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"flip_horizontal_state", file_len, strlen("flip_horizontal_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->flip_horizontal_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"flip_vertical_state", file_len, strlen("flip_vertical_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->flip_vertical_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"rotate_state", file_len, strlen("rotate_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->rotate_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"rotate_opposite", file_len, strlen("rotate_opposite"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->rotate_opposite = (bool)temp;
        }
    }

    // 图像尺寸
    pos = mystrstr((unsigned char *)msg, (unsigned char *)"roi_x", file_len, strlen("roi_x"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->roi.X = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"roi_y", file_len, strlen("roi_y"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->roi.Y = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"roi_w", file_len, strlen("roi_w"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->roi.W = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"roi_h", file_len, strlen("roi_h"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->roi.H = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"roi_state", file_len, strlen("roi_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->roi_state = (bool)temp;
        }
    }

    // 图像格式
    pos = mystrstr((unsigned char *)msg, (unsigned char *)"source_format", file_len, strlen("source_format"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->source_format = (dvpStreamFormat)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"target_format", file_len, strlen("target_format"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->target_format = (dvpStreamFormat)temp;
        }
    }

    // 触发功能
    pos = mystrstr((unsigned char *)msg, (unsigned char *)"soft_trigger_loop_time", file_len, strlen("soft_trigger_loop_time"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->soft_trigger_loop_time = temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"soft_trigger_loop_state", file_len, strlen("soft_trigger_loop_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->soft_trigger_loop_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"ext_trigger_jitter_filter", file_len, strlen("ext_trigger_jitter_filter"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->ext_trigger_jitter_filter = temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"ext_trigger_delay", file_len, strlen("ext_trigger_delay"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->ext_trigger_delay = temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"strobe_delay", file_len, strlen("strobe_delay"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->strobe_delay = temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"strobe_duration", file_len, strlen("strobe_duration"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            user_config->strobe_duration = temp_f;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"frames_per_trigger", file_len, strlen("frames_per_trigger"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->frames_per_trigger = (unsigned int)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"ext_trigger_source", file_len, strlen("ext_trigger_source"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->ext_trigger_source = (dvpTriggerSource)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"trigger_state", file_len, strlen("trigger_state"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->trigger_state = (bool)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"strobe_driver", file_len, strlen("strobe_driver"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->strobe_driver = (dvpStrobeDriver)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"strobe_output_type", file_len, strlen("strobe_output_type"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->strobe_output_type = (dvpStrobeOutputType)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"ext_trigger_input_type", file_len, strlen("ext_trigger_input_type"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            user_config->ext_trigger_input_type = (dvpTriggerInputType)temp;
        }
    }

    pos = mystrstr((unsigned char *)msg, (unsigned char *)"trigger_frame_rate", file_len, strlen("trigger_frame_rate"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = atof(temp_buf);
            user_config->trigger_frame_rate = temp;
        }
    }

    return 0;
}

static int setCameraUserConfig(struct M3st130hConfig *config,struct M3st130hConfig *user_config)
{
    dvpStatus dvp_status = DVP_STATUS_OK;
    unsigned char rgb_gain_flag = 0;
    dvpIntDescr i_describe = {0,0,0,0,0};
    dvpFloatDescr f_describe = {0.0f,0.0f,0.0f,0.0f,0.0f};
    dvpDoubleDescr d_describe = {0.0f,0.0f,0.0f,0.0f,0.0f};

    // 曝光功能
    if(config->auto_exposure_target_brightness != user_config->auto_exposure_target_brightness)
    {
        dvp_status = dvpGetAeTargetDescr(config->camera_handle,&i_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetAeTargetDescr failed\n",__func__);
        }
        else
        {
            if(user_config->auto_exposure_target_brightness < i_describe.iMin ||
               user_config->auto_exposure_target_brightness > i_describe.iMax)
            {
                fprintf(stderr,"%s: auto_exposure_target_brightness is Illegal,should be (%d~%d) in step:%d\n",
                        __func__,
                        i_describe.iMin,
                        i_describe.iMax,
                        i_describe.iStep);
            }
            else
            {
                config->auto_exposure_target_brightness = user_config->auto_exposure_target_brightness;

                dvp_status = dvpSetAeTarget(config->camera_handle,config->auto_exposure_target_brightness);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set auto_exposure_target_brightness failed\n",__func__);
                }
            }
        }
    }

    if(config->analog_gain != user_config->analog_gain)
    {
        dvp_status = dvpGetAnalogGainDescr(config->camera_handle,&f_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetAnalogGainDescr failed\n",__func__);
        }
        else
        {
            if(user_config->analog_gain < f_describe.fMin ||
               user_config->analog_gain > f_describe.fMax)
            {
                fprintf(stderr,"%s: analog_gain is Illegal,should be (%.3f~%.3f) in step:%.3f\n",
                        __func__,
                        f_describe.fMin,
                        f_describe.fMax,
                        f_describe.fStep);
            }
            else
            {
                config->analog_gain = user_config->analog_gain;

                dvp_status = dvpSetAnalogGain(config->camera_handle,config->analog_gain);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set analog_gain failed\n",__func__);
                }
            }
        }
    }

    if(config->exposure_time != user_config->exposure_time)
    {
        dvp_status = dvpGetExposureDescr(config->camera_handle,&d_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetExposureDescr failed\n",__func__);
        }
        else
        {
            if(user_config->exposure_time < d_describe.fMin ||
               user_config->exposure_time > d_describe.fMax)
            {
                fprintf(stderr,"%s: exposure_time is Illegal,should be (%.3f~%.3f) in step:%.3f\n",
                        __func__,
                        d_describe.fMin,
                        d_describe.fMax,
                        d_describe.fStep);
            }
            else
            {
                config->exposure_time = user_config->exposure_time;

                dvp_status = dvpSetExposure(config->camera_handle,config->exposure_time);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set exposure_time failed\n",__func__);
                }
            }
        }
    }

    if(config->exposure_roi.X != user_config->exposure_roi.X ||
       config->exposure_roi.Y != user_config->exposure_roi.Y ||
       config->exposure_roi.W != user_config->exposure_roi.W ||
       config->exposure_roi.H != user_config->exposure_roi.H)
    {
        if(user_config->exposure_roi.X % 2 != 0 ||
           user_config->exposure_roi.Y % 2 != 0 ||
           user_config->exposure_roi.W % 2 != 0 ||
           user_config->exposure_roi.H % 2 != 0 ||
           user_config->exposure_roi.X + user_config->exposure_roi.W > M3ST130H_MAX_ROI_W ||
           user_config->exposure_roi.Y + user_config->exposure_roi.H > M3ST130H_MAX_ROI_H)
        {
            fprintf(stderr,"%s: exposure_roi is Illegal, "
                    "all the data should be an integer multiple of 2, "
                    "(X + W) should be less than 1280 and (Y + H) should be less than 1024\n",__func__);
        }
        else
        {
            config->exposure_roi.X = user_config->exposure_roi.X;
            config->exposure_roi.Y = user_config->exposure_roi.Y;
            config->exposure_roi.W = user_config->exposure_roi.W;
            config->exposure_roi.H = user_config->exposure_roi.H;

            dvp_status = dvpSetAeRoi(config->camera_handle,config->exposure_roi);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set exposure_roi failed\n",__func__);
            }
        }
    }

    if(config->auto_exposure_mode != user_config->auto_exposure_mode)
    {
        if(user_config->auto_exposure_mode < AE_MODE_AE_AG ||
           user_config->auto_exposure_mode > AE_MODE_AG_ONLY)
        {
            fprintf(stderr,"%s: auto_exposure_mode is Illegal,should be in range AE_MODE_AE_AG(0) to AE_MODE_AG_ONLY(3)\n",__func__);
        }
        else
        {
            config->auto_exposure_mode = user_config->auto_exposure_mode;

            dvp_status = dvpSetAeMode(config->camera_handle,config->auto_exposure_mode);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set auto_exposure_mode failed\n",__func__);
            }
        }
    }

    if(config->anti_flick != user_config->anti_flick)
    {
        if(user_config->anti_flick < AE_MODE_AE_AG ||
           user_config->anti_flick > AE_MODE_AG_ONLY)
        {
            fprintf(stderr,"%s: anti_flick is Illegal,should be in range ANTIFLICK_DISABLE(0) to ANTIFLICK_60HZ(2)\n",__func__);
        }
        else
        {
            config->anti_flick = user_config->anti_flick;

            dvp_status = dvpSetAntiFlick(config->camera_handle,config->anti_flick);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set anti_flick failed\n",__func__);
            }
        }
    }

    if(config->auto_exposure_operate_mode != user_config->auto_exposure_operate_mode)
    {
        if(user_config->auto_exposure_operate_mode < AE_MODE_AE_AG ||
           user_config->auto_exposure_operate_mode > AE_MODE_AG_ONLY)
        {
            fprintf(stderr,"%s: auto_exposure_operate_mode is Illegal,should be in range AE_OP_OFF(0) to AE_OP_CONTINUOUS(2)\n",__func__);
        }
        else
        {
            config->auto_exposure_operate_mode = user_config->auto_exposure_operate_mode;

            dvp_status = dvpSetAeOperation(config->camera_handle,config->auto_exposure_operate_mode);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set auto_exposure_operate_mode failed\n",__func__);
            }
        }
    }

    if(config->auto_exposure_conf_info.fExposureMin != user_config->auto_exposure_conf_info.fExposureMin ||
       config->auto_exposure_conf_info.fExposureMax != user_config->auto_exposure_conf_info.fExposureMax ||
       config->auto_exposure_conf_info.fGainMin != user_config->auto_exposure_conf_info.fGainMin ||
       config->auto_exposure_conf_info.fGainMax != user_config->auto_exposure_conf_info.fGainMax)
    {
        if(user_config->auto_exposure_conf_info.fExposureMin < M3ST130H_MIN_AUTO_EXP_TIME ||
           user_config->auto_exposure_conf_info.fExposureMax > M3ST130H_MAX_AUTO_EXP_TIME ||
           user_config->auto_exposure_conf_info.fGainMin < M3ST130H_MIN_AUTO_EXP_GAIN ||
           user_config->auto_exposure_conf_info.fGainMax > M3ST130H_MAX_AUTO_EXP_GAIN ||
           user_config->auto_exposure_conf_info.fExposureMin > user_config->auto_exposure_conf_info.fExposureMax ||
           user_config->auto_exposure_conf_info.fGainMin > user_config->auto_exposure_conf_info.fGainMax)
        {
            fprintf(stderr,"%s: auto_exposure_conf_info data is Illegal, \n"
                    "                     fExposureMin = %.3f should be greater than %d and less than fExposureMax, \n"
                    "                     fExposureMax = %.3f should be less than %d, \n"
                    "                     fGainMin     = %.3f should be greater than %d and less than fGainMax, \n"
                    "                     fGainMax     = %.3f should be less than %d\n",
                    __func__,
                    user_config->auto_exposure_conf_info.fExposureMin,
                    M3ST130H_MIN_AUTO_EXP_TIME,
                    user_config->auto_exposure_conf_info.fExposureMax,
                    M3ST130H_MAX_AUTO_EXP_TIME,
                    user_config->auto_exposure_conf_info.fGainMin,
                    M3ST130H_MIN_AUTO_EXP_GAIN,
                    user_config->auto_exposure_conf_info.fGainMax,
                    M3ST130H_MAX_AUTO_EXP_GAIN);
        }
        else
        {
            config->auto_exposure_conf_info.fExposureMin = user_config->auto_exposure_conf_info.fExposureMin;
            config->auto_exposure_conf_info.fExposureMax = user_config->auto_exposure_conf_info.fExposureMax;
            config->auto_exposure_conf_info.fGainMin = user_config->auto_exposure_conf_info.fGainMin;
            config->auto_exposure_conf_info.fGainMax = user_config->auto_exposure_conf_info.fGainMax;

            dvp_status =dvpSetAeConfig(config->camera_handle,config->auto_exposure_conf_info);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set auto_exposure_conf_info failed\n",__func__);
            }
        }
    }

    // 色彩调节
    if(config->color_temperature != user_config->color_temperature)
    {
        dvp_status = dvpGetColorTemperatureDescr(config->camera_handle,&i_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetColorTemperatureDescr failed\n",__func__);
        }
        else
        {
            if(user_config->color_temperature < i_describe.iMin ||
               user_config->color_temperature > i_describe.iMax)
            {
                fprintf(stderr,"%s: color_temperature is Illegal,should be (%d~%d) in step:%d\n",
                        __func__,
                        i_describe.iMin,
                        i_describe.iMax,
                        i_describe.iStep);
            }
            else
            {
                config->color_temperature = user_config->color_temperature;

                dvp_status = dvpSetColorTemperature(config->camera_handle,config->color_temperature);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set color_temperature failed\n",__func__);
                }
            }
        }
    }

    if(config->color_temperature_state != user_config->color_temperature_state)
    {
        config->color_temperature_state = user_config->color_temperature_state;

        dvp_status = dvpSetColorTemperatureState(config->camera_handle,config->color_temperature_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set color_temperature_state failed\n",__func__);
        }
    }

    if(config->saturation != user_config->saturation)
    {
        dvp_status = dvpGetSaturationDescr(config->camera_handle,&i_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSaturationDescr failed\n",__func__);
        }
        else
        {
            if(user_config->saturation < i_describe.iMin ||
               user_config->saturation > i_describe.iMax)
            {
                fprintf(stderr,"%s: saturation is Illegal,should be (%d~%d) in step:%d\n",
                        __func__,
                        i_describe.iMin,
                        i_describe.iMax,
                        i_describe.iStep);
            }
            else
            {
                config->saturation = user_config->saturation;

                dvp_status = dvpSetSaturation(config->camera_handle,config->saturation);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set saturation failed\n",__func__);
                }
            }
        }
    }

    if(config->saturation_state != user_config->saturation_state)
    {
        config->saturation_state = user_config->saturation_state;

        dvp_status = dvpSetSaturationState(config->camera_handle,config->saturation_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set saturation_state failed\n",__func__);
        }
    }

    if(config->awb_roi.X != user_config->awb_roi.X ||
       config->awb_roi.Y != user_config->awb_roi.Y ||
       config->awb_roi.W != user_config->awb_roi.W ||
       config->awb_roi.H != user_config->awb_roi.H)
    {
        if(user_config->awb_roi.X % 2 != 0 ||
           user_config->awb_roi.Y % 2 != 0 ||
           user_config->awb_roi.W % 2 != 0 ||
           user_config->awb_roi.H % 2 != 0 ||
           user_config->awb_roi.X + user_config->awb_roi.W > M3ST130H_MAX_ROI_W ||
           user_config->awb_roi.Y + user_config->awb_roi.H > M3ST130H_MAX_ROI_H)
        {
            fprintf(stderr,"%s: awb_roi is Illegal, "
                    "all the data should be an integer multiple of 2, "
                    "(X + W) should be less than 1280 and (Y + H) should be less than 1024\n",__func__);
        }
        else
        {
            config->awb_roi.X = user_config->awb_roi.X;
            config->awb_roi.Y = user_config->awb_roi.Y;
            config->awb_roi.W = user_config->awb_roi.W;
            config->awb_roi.H = user_config->awb_roi.H;

            dvp_status = dvpSetAwbRoi(config->camera_handle,config->awb_roi);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set awb_roi failed\n",__func__);
            }
        }
    }

    if(config->awb_operate != user_config->awb_operate)
    {
        if(user_config->awb_operate < AE_MODE_AE_AG ||
           user_config->awb_operate > AE_MODE_AG_ONLY)
        {
            fprintf(stderr,"%s: awb_operate is Illegal,should be in range AWB_OP_OFF(0) to AWB_OP_CONTINUOUS(2)\n",__func__);
        }
        else
        {
            config->awb_operate = user_config->awb_operate;

            dvp_status = dvpSetAwbOperation(config->camera_handle,config->awb_operate);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set awb_operate failed\n",__func__);
            }
        }
    }

    if(config->red_digital_gain != user_config->red_digital_gain)
    {
        if(user_config->red_digital_gain < M3ST130H_MIN_RGB_GAIN ||
           user_config->red_digital_gain > M3ST130H_MAX_RGB_GAIN)
        {
            fprintf(stderr,"%s: red_digital_gain is Illegal,should be in range 0 to 4\n",__func__);
        }
        else
        {
            config->red_digital_gain = user_config->red_digital_gain;
            rgb_gain_flag |= 0x01;
        }
    }

    if(config->green_digital_gain != user_config->green_digital_gain)
    {
        if(user_config->green_digital_gain < M3ST130H_MIN_RGB_GAIN ||
           user_config->green_digital_gain > M3ST130H_MAX_RGB_GAIN)
        {
            fprintf(stderr,"%s: green_digital_gain is Illegal,should be in range 0 to 4\n",__func__);
        }
        else
        {
            config->green_digital_gain = user_config->green_digital_gain;
            rgb_gain_flag |= 0x02;
        }
    }

    if(config->blue_digital_gain != user_config->blue_digital_gain)
    {
        if(user_config->blue_digital_gain < M3ST130H_MIN_RGB_GAIN ||
           user_config->blue_digital_gain > M3ST130H_MAX_RGB_GAIN)
        {
            fprintf(stderr,"%s: blue_digital_gain is Illegal,should be in range 0 to 4\n",__func__);
        }
        else
        {
            config->blue_digital_gain = user_config->blue_digital_gain;
            rgb_gain_flag |= 0x04;
        }
    }

    if(rgb_gain_flag != 0)
    {
        dvp_status = dvpSetRgbGain(config->camera_handle,
                                   config->red_digital_gain,
                                   config->green_digital_gain,
                                   config->blue_digital_gain);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set rgb_digital_gain failed\n",__func__);
        }
    }

    if(config->rgb_digital_gain_state != user_config->rgb_digital_gain_state)
    {
        config->rgb_digital_gain_state = user_config->rgb_digital_gain_state;

        dvp_status = dvpSetRgbGainState(config->camera_handle,config->rgb_digital_gain_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set rgb_digital_gain_state failed\n",__func__);
        }
    }

    // 增强效果
    if(config->gamma != user_config->gamma)
    {
        dvp_status = dvpGetGammaDescr(config->camera_handle,&i_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSaturationDescr failed\n",__func__);
        }
        else
        {
            if(user_config->gamma < i_describe.iMin ||
               user_config->gamma > i_describe.iMax)
            {
                fprintf(stderr,"%s: gamma is Illegal,should be (%d~%d) in step:%d\n",
                        __func__,
                        i_describe.iMin,
                        i_describe.iMax,
                        i_describe.iStep);
            }
            else
            {
                config->gamma = user_config->gamma;

                dvp_status = dvpSetGamma(config->camera_handle,config->gamma);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set gamma failed\n",__func__);
                }
            }
        }
    }

    if(config->gamma_state != user_config->gamma_state)
    {
        config->gamma_state = user_config->gamma_state;

        dvp_status = dvpSetGammaState(config->camera_handle,config->gamma_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set gamma_state failed\n",__func__);
        }
    }

    if(config->contrast != user_config->contrast)
    {
        dvp_status = dvpGetContrastDescr(config->camera_handle,&i_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSaturationDescr failed\n",__func__);
        }
        else
        {
            if(user_config->contrast < i_describe.iMin ||
               user_config->contrast > i_describe.iMax)
            {
                fprintf(stderr,"%s: contrast is Illegal,should be (%d~%d) in step:%d\n",
                        __func__,
                        i_describe.iMin,
                        i_describe.iMax,
                        i_describe.iStep);
            }
            else
            {
                config->contrast = user_config->contrast;

                dvp_status = dvpSetContrast(config->camera_handle,config->contrast);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set contrast failed\n",__func__);
                }
            }
        }
    }

    if(config->contrast_state != user_config->contrast_state)
    {
        config->contrast_state = user_config->contrast_state;

        dvp_status = dvpSetContrastState(config->camera_handle,config->contrast_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set contrast_state failed\n",__func__);
        }
    }

    if(config->sharpness != user_config->sharpness)
    {
        dvp_status = dvpGetSharpnessDescr(config->camera_handle,&i_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSharpnessDescr failed\n",__func__);
        }
        else
        {
            if(user_config->sharpness < i_describe.iMin ||
               user_config->sharpness > i_describe.iMax)
            {
                fprintf(stderr,"%s: sharpness is Illegal,should be (%d~%d) in step:%d\n",
                        __func__,
                        i_describe.iMin,
                        i_describe.iMax,
                        i_describe.iStep);
            }
            else
            {
                config->sharpness = user_config->sharpness;

                dvp_status = dvpSetSharpness(config->camera_handle,config->sharpness);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set sharpness failed\n",__func__);
                }
            }
        }
    }

    if(config->sharpness_state != user_config->sharpness_state)
    {
        config->sharpness_state = user_config->sharpness_state;

        dvp_status = dvpSetSharpnessState(config->camera_handle,config->sharpness_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set sharpness_state failed\n",__func__);
        }
    }

    if(config->noise_reduct_2d != user_config->noise_reduct_2d)
    {
        dvp_status = dvpGetNoiseReduct2dDescr(config->camera_handle,&i_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetNoiseReduct2dDescr failed\n",__func__);
        }
        else
        {
            if(user_config->noise_reduct_2d < i_describe.iMin ||
               user_config->noise_reduct_2d > i_describe.iMax)
            {
                fprintf(stderr,"%s: noise_reduct_2d is Illegal,should be (%d~%d) in step:%d\n",
                        __func__,
                        i_describe.iMin,
                        i_describe.iMax,
                        i_describe.iStep);
            }
            else
            {
                config->noise_reduct_2d = user_config->noise_reduct_2d;

                dvp_status = dvpSetNoiseReduct2d(config->camera_handle,config->noise_reduct_2d);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set noise_reduct_2d failed\n",__func__);
                }
            }
        }
    }

    if(config->noise_reduct_2d_state != user_config->noise_reduct_2d_state)
    {
        config->noise_reduct_2d_state = user_config->noise_reduct_2d_state;

        dvp_status = dvpSetNoiseReduct2dState(config->camera_handle,config->noise_reduct_2d_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set noise_reduct_2d_state failed\n",__func__);
        }
    }

    if(config->noise_reduct_3d != user_config->noise_reduct_3d)
    {
        dvp_status = dvpGetNoiseReduct3dDescr(config->camera_handle,&i_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetNoiseReduct3dDescr failed\n",__func__);
        }
        else
        {
            if(user_config->noise_reduct_3d < i_describe.iMin ||
               user_config->noise_reduct_3d > i_describe.iMax)
            {
                fprintf(stderr,"%s: noise_reduct_3d is Illegal,should be (%d~%d) in step:%d\n",
                        __func__,
                        i_describe.iMin,
                        i_describe.iMax,
                        i_describe.iStep);
            }
            else
            {
                config->noise_reduct_3d = user_config->noise_reduct_3d;

                dvp_status = dvpSetNoiseReduct3d(config->camera_handle,config->noise_reduct_3d);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set noise_reduct_3d failed\n",__func__);
                }
            }
        }
    }

    if(config->noise_reduct_3d_state != user_config->noise_reduct_3d_state)
    {
        config->noise_reduct_3d_state = user_config->noise_reduct_3d_state;

        dvp_status = dvpSetNoiseReduct3dState(config->camera_handle,config->noise_reduct_3d_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set noise_reduct_3d_state failed\n",__func__);
        }
    }

    if(config->black_level != user_config->black_level)
    {
        dvp_status = dvpGetBlackLevelDescr(config->camera_handle,&f_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetBlackLevelDescr failed\n",__func__);
        }
        else
        {
            if(user_config->black_level < f_describe.fMin ||
               user_config->black_level > f_describe.fMax)
            {
                fprintf(stderr,"%s: black_level is Illegal,should be (%.3f~%.3f) in step:%.3f\n",
                        __func__,
                        f_describe.fMin,
                        f_describe.fMax,
                        f_describe.fStep);
            }
            else
            {
                config->black_level = user_config->black_level;

                dvp_status = dvpSetBlackLevel(config->camera_handle,config->black_level);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set black_level failed\n",__func__);
                }
            }
        }
    }

    if(config->black_level_state != user_config->black_level_state)
    {
        config->black_level_state = user_config->black_level_state;

        dvp_status = dvpSetBlackLevelState(config->camera_handle,config->black_level_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set black_level_state failed\n",__func__);
        }
    }

    if(config->mono_state != user_config->mono_state)
    {
        config->mono_state = user_config->mono_state;

        dvp_status = dvpSetMonoState(config->camera_handle,config->mono_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set mono_state failed\n",__func__);
        }
    }

    if(config->inverse_state != user_config->inverse_state)
    {
        config->inverse_state = user_config->inverse_state;

        dvp_status = dvpSetInverseState(config->camera_handle,config->inverse_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set inverse_state failed\n",__func__);
        }
    }

    if(config->flip_horizontal_state != user_config->flip_horizontal_state)
    {
        config->flip_horizontal_state = user_config->flip_horizontal_state;

        dvp_status = dvpSetFlipHorizontalState(config->camera_handle,config->flip_horizontal_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set flip_horizontal_state failed\n",__func__);
        }
    }

    if(config->flip_vertical_state != user_config->flip_vertical_state)
    {
        config->flip_vertical_state = user_config->flip_vertical_state;

        dvp_status = dvpSetFlipVerticalState(config->camera_handle,config->flip_vertical_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set flip_vertical_state failed\n",__func__);
        }
    }

    if(config->rotate_state != user_config->rotate_state)
    {
        config->rotate_state = user_config->rotate_state;

        dvp_status = dvpSetRotateState(config->camera_handle,config->rotate_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set rotate_state failed\n",__func__);
        }
    }

    if(config->rotate_opposite != user_config->rotate_opposite)
    {
        config->rotate_opposite = user_config->rotate_opposite;

        dvp_status = dvpSetRotateOpposite(config->camera_handle,config->rotate_opposite);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set rotate_opposite failed\n",__func__);
        }
    }

    // 图像尺寸
    if(config->roi.X != user_config->roi.X ||
       config->roi.Y != user_config->roi.Y ||
       config->roi.W != user_config->roi.W ||
       config->roi.H != user_config->roi.H)
    {
        dvpRegionDescr pRoiDescr;

        dvp_status = dvpGetRoiDescr(config->camera_handle,&pRoiDescr);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetBlackLevelDescr failed\n",__func__);
        }
        else
        {
            if(user_config->roi.X % 2 != 0 ||
               user_config->roi.Y % 2 != 0 ||
               user_config->roi.W % pRoiDescr.iStepW != 0 ||
               user_config->roi.H % pRoiDescr.iStepH != 0 ||
               user_config->roi.X + user_config->roi.W > M3ST130H_MAX_ROI_W ||
               user_config->roi.Y + user_config->roi.H > M3ST130H_MAX_ROI_H ||
               user_config->roi.W < pRoiDescr.iMinW ||
               user_config->roi.W > pRoiDescr.iMaxW ||
               user_config->roi.H < pRoiDescr.iMinH ||
               user_config->roi.H > pRoiDescr.iMaxH)
            {
                fprintf(stderr,"%s: roi is Illegal, "
                        "roi.X and roi.Y should be an integer multiple of 2, "
                        "roi.W should be an integer multiple of %d, "
                        "roi.H should be an integer multiple of %d, "
                        "(X + W) should be less than 1280 and (Y + H) should be less than 1024\n",
                        __func__,
                        pRoiDescr.iStepW,
                        pRoiDescr.iStepH);
            }
            else
            {
                config->roi.X = user_config->roi.X;
                config->roi.Y = user_config->roi.Y;
                config->roi.W = user_config->roi.W;
                config->roi.H = user_config->roi.H;

                dvp_status = dvpSetRoi(config->camera_handle,config->roi);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set roi failed\n",__func__);
                }
            }
        }
    }

    if(config->roi_state != user_config->roi_state)
    {
        config->roi_state = user_config->roi_state;

        dvp_status = dvpSetRoiState(config->camera_handle,config->roi_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set roi_state failed\n",__func__);
        }
    }

    // 图像格式
    if(config->source_format != user_config->source_format)
    {
        if(user_config->target_format != S_RAW8)
        {
            fprintf(stderr,"%s: source_format is Illegal,should be S_RAW8(0)\n",__func__);
        }
        else
        {
            config->source_format = user_config->source_format;

            dvp_status = dvpSetSourceFormat(config->camera_handle,config->source_format);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set source_format failed\n",__func__);
            }
        }
    }

    if(config->target_format != user_config->target_format)
    {
        if(user_config->target_format != S_RAW8 &&
           user_config->target_format != S_RAW16 &&
           user_config->target_format != S_MONO8 &&
           user_config->target_format != S_MONO16 &&
           user_config->target_format != S_BGR24)
        {
            fprintf(stderr,"%s: target_format is Illegal,should be S_RAW8(0)/S_RAW16(4)/S_MONO8(30)/S_MONO16(34)/S_BGR24(14)\n",__func__);
        }
        else
        {
            config->target_format = user_config->target_format;

            dvp_status = dvpSetTargetFormat(config->camera_handle,config->target_format);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set target_format failed\n",__func__);
            }
        }
    }

    // 触发功能
    if(config->soft_trigger_loop_time != user_config->soft_trigger_loop_time)
    {
        dvp_status = dvpGetSoftTriggerLoopDescr(config->camera_handle,&d_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSoftTriggerLoopDescr failed\n",__func__);
        }
        else
        {
            if(user_config->soft_trigger_loop_time < d_describe.fMin ||
               user_config->soft_trigger_loop_time > d_describe.fMax)
            {
                fprintf(stderr,"%s: soft_trigger_loop_time is Illegal,should be (%.3f~%.3f) in step:%.3f\n",
                        __func__,
                        d_describe.fMin,
                        d_describe.fMax,
                        d_describe.fStep);
            }
            else
            {
                config->soft_trigger_loop_time = user_config->soft_trigger_loop_time;

                dvp_status = dvpSetSoftTriggerLoop(config->camera_handle,config->soft_trigger_loop_time);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set soft_trigger_loop_time failed\n",__func__);
                }
            }
        }
    }

    if(config->soft_trigger_loop_state != user_config->soft_trigger_loop_state)
    {
        config->soft_trigger_loop_state = user_config->soft_trigger_loop_state;

        dvp_status = dvpSetSoftTriggerLoopState(config->camera_handle,config->soft_trigger_loop_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set soft_trigger_loop_state failed\n",__func__);
        }
    }

    if(config->ext_trigger_jitter_filter != user_config->ext_trigger_jitter_filter)
    {
        dvp_status = dvpGetTriggerJitterFilterDescr(config->camera_handle,&d_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSoftTriggerLoopDescr failed\n",__func__);
        }
        else
        {
            if(user_config->ext_trigger_jitter_filter < d_describe.fMin ||
               user_config->ext_trigger_jitter_filter > d_describe.fMax)
            {
                fprintf(stderr,"%s: ext_trigger_jitter_filter is Illegal,should be (%.3f~%.3f) in step:%.3f\n",
                        __func__,
                        d_describe.fMin,
                        d_describe.fMax,
                        d_describe.fStep);
            }
            else
            {
                config->ext_trigger_jitter_filter = user_config->ext_trigger_jitter_filter;

                dvp_status = dvpSetTriggerJitterFilter(config->camera_handle,config->ext_trigger_jitter_filter);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set ext_trigger_jitter_filter failed\n",__func__);
                }
            }
        }
    }

    if(config->ext_trigger_delay != user_config->ext_trigger_delay)
    {
        dvp_status = dvpGetTriggerDelayDescr(config->camera_handle,&d_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSoftTriggerLoopDescr failed\n",__func__);
        }
        else
        {
            if(user_config->ext_trigger_delay < d_describe.fMin ||
               user_config->ext_trigger_delay > d_describe.fMax)
            {
                fprintf(stderr,"%s: ext_trigger_delay is Illegal,should be (%.3f~%.3f) in step:%.3f\n",
                        __func__,
                        d_describe.fMin,
                        d_describe.fMax,
                        d_describe.fStep);
            }
            else
            {
                config->ext_trigger_delay = user_config->ext_trigger_delay;

                dvp_status = dvpSetTriggerDelay(config->camera_handle,config->ext_trigger_delay);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set ext_trigger_delay failed\n",__func__);
                }
            }
        }
    }

    if(config->strobe_delay != user_config->strobe_delay)
    {
        dvp_status = dvpGetStrobeDelayDescr(config->camera_handle,&d_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSoftTriggerLoopDescr failed\n",__func__);
        }
        else
        {
            if(user_config->strobe_delay < d_describe.fMin ||
               user_config->strobe_delay > d_describe.fMax)
            {
                fprintf(stderr,"%s: strobe_delay is Illegal,should be (%.3f~%.3f) in step:%.3f\n",
                        __func__,
                        d_describe.fMin,
                        d_describe.fMax,
                        d_describe.fStep);
            }
            else
            {
                config->strobe_delay = user_config->strobe_delay;

                dvp_status = dvpSetStrobeDelay(config->camera_handle,config->strobe_delay);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set strobe_delay failed\n",__func__);
                }
            }
        }
    }

    if(config->strobe_duration != user_config->strobe_duration)
    {
        dvp_status = dvpGetStrobeDurationDescr(config->camera_handle,&d_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSoftTriggerLoopDescr failed\n",__func__);
        }
        else
        {
            if(user_config->strobe_duration < d_describe.fMin ||
               user_config->strobe_duration > d_describe.fMax)
            {
                fprintf(stderr,"%s: strobe_duration is Illegal,should be (%.3f~%.3f) in step:%.3f\n",
                        __func__,
                        d_describe.fMin,
                        d_describe.fMax,
                        d_describe.fStep);
            }
            else
            {
                config->strobe_duration = user_config->strobe_duration;

                dvp_status = dvpSetStrobeDuration(config->camera_handle,config->strobe_duration);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set strobe_duration failed\n",__func__);
                }
            }
        }
    }

    if(config->frames_per_trigger != user_config->frames_per_trigger)
    {
        dvp_status = dvpGetFramesPerTriggerDescr(config->camera_handle,&i_describe);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: dvpGetSoftTriggerLoopDescr failed\n",__func__);
        }
        else
        {
            if(user_config->frames_per_trigger < i_describe.iMin ||
               user_config->frames_per_trigger > i_describe.iMax)
            {
                fprintf(stderr,"%s: frames_per_trigger is Illegal,should be (%d~%d) in step:%d\n",
                        __func__,
                        i_describe.iMin,
                        i_describe.iMax,
                        i_describe.iStep);
            }
            else
            {
                config->frames_per_trigger = user_config->frames_per_trigger;

                dvp_status = dvpSetFramesPerTrigger(config->camera_handle,config->frames_per_trigger);
                if(dvp_status != DVP_STATUS_OK)
                {
                    fprintf(stderr,"%s: set frames_per_trigger failed\n",__func__);
                }
            }
        }
    }

    if(config->ext_trigger_source != user_config->ext_trigger_source)
    {
        if(user_config->ext_trigger_source < TRIGGER_SOURCE_SOFTWARE ||
           user_config->ext_trigger_source > TRIGGER_SOURCE_LINE8)
        {
            fprintf(stderr,"%s: ext_trigger_source is Illegal, "
                    "should be in range TRIGGER_SOURCE_SOFTWARE(0) to TRIGGER_SOURCE_LINE8(8)\n",__func__);
        }
        else
        {
            config->ext_trigger_source = user_config->ext_trigger_source;

            dvp_status = dvpSetTriggerSource(config->camera_handle,config->ext_trigger_source);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set ext_trigger_source failed\n",__func__);
            }
        }
    }

    if(config->trigger_state != user_config->trigger_state)
    {
        config->trigger_state = user_config->trigger_state;

        dvp_status = dvpSetTriggerState(config->camera_handle,config->trigger_state);
        if(dvp_status != DVP_STATUS_OK)
        {
            fprintf(stderr,"%s: set trigger_state failed\n",__func__);
        }
    }

    if(config->strobe_driver != user_config->strobe_driver)
    {
        if(user_config->strobe_driver < FRAME_DURATION ||
           user_config->strobe_driver > SENSOR_STROBE)
        {
            fprintf(stderr,"%s: strobe_driver is Illegal, "
                    "should be in range FRAME_DURATION(0) to SENSOR_STROBE(2)\n",__func__);
        }
        else
        {
            config->strobe_driver = user_config->strobe_driver;

            dvp_status = dvpSetStrobeDriver(config->camera_handle,config->strobe_driver);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set strobe_driver failed\n",__func__);
            }
        }
    }

    if(config->strobe_output_type != user_config->strobe_output_type)
    {
        if(user_config->strobe_output_type < STROBE_OUT_OFF ||
           user_config->strobe_output_type > STROBE_OUT_HIGH)
        {
            fprintf(stderr,"%s: strobe_output_type is Illegal, "
                    "should be in range STROBE_OUT_OFF(0) to STROBE_OUT_HIGH(2)\n",__func__);
        }
        else
        {
            config->strobe_output_type = user_config->strobe_output_type;

            dvp_status = dvpSetStrobeOutputType(config->camera_handle,config->strobe_output_type);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set strobe_output_type failed\n",__func__);
            }
        }
    }

    if(config->ext_trigger_input_type != user_config->ext_trigger_input_type)
    {
        if(user_config->ext_trigger_input_type < TRIGGER_IN_OFF ||
           user_config->ext_trigger_input_type > TRIGGER_HIGH_LEVEL)
        {
            fprintf(stderr,"%s: ext_trigger_input_type is Illegal, "
                    "should be in range TRIGGER_IN_OFF(0) to TRIGGER_HIGH_LEVEL(4)\n",__func__);
        }
        else
        {
            config->ext_trigger_input_type = user_config->ext_trigger_input_type;

            dvp_status = dvpSetTriggerInputType(config->camera_handle,config->ext_trigger_input_type);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set ext_trigger_input_type failed\n",__func__);
            }
        }
    }

    if(config->trigger_state == true)
    {
        if(config->ext_trigger_source != TRIGGER_SOURCE_SOFTWARE)
        {
            dvp_status = dvpSetInputIoFunction(config->camera_handle, INPUT_IO_1, INPUT_FUNCTION_TRIGGER);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set INPUT_IO_1 to INPUT_FUNCTION_TRIGGER failed\n",__func__);
                return -1;
            }

            dvp_status = dvpSetOutputIoFunction(config->camera_handle, INPUT_IO_2, OUTPUT_FUNCTION_STROBE);
            if(dvp_status != DVP_STATUS_OK)
            {
                fprintf(stderr,"%s: set INPUT_IO_2 to OUTPUT_FUNCTION_STROBE failed\n",__func__);
                return -1;
            }

            fprintf(stderr,"%s: set external trigger mode success\n",__func__);
        }
        else
        {
            fprintf(stderr,"%s: now it's software trigger mode\n",__func__);
        }
    }
    else
    {
        fprintf(stderr,"%s: now it's free run mode\n",__func__);
    }

    config->trigger_frame_rate = user_config->trigger_frame_rate;

    if(config->roi_state == false)
    {
        config->image_width = M3ST130H_MAX_ROI_W;
        config->image_height = M3ST130H_MAX_ROI_H;
    }
    else
    {
        config->image_width = config->roi.W;
        config->image_height = config->roi.H;
    }

    config->image_size = config->image_width * config->image_height;

    switch((unsigned char)config->target_format)
    {
        case (unsigned char)S_RAW8:
            config->bytes_per_pix = 1;
        break;

        case (unsigned char)S_RAW16:
            config->bytes_per_pix = 2;
        break;

        case (unsigned char)S_MONO8:
            config->bytes_per_pix = 1;
        break;

        case (unsigned char)S_MONO16:
            config->bytes_per_pix = 2;
        break;

        case (unsigned char)S_BGR24:
            config->bytes_per_pix = 3;
        break;

        default:
            config->bytes_per_pix = 6;
        break;
    }

    config->frame_buf_size = config->image_size * config->bytes_per_pix;

    return 0;
}

static int reallocateCameraBuffer(struct M3st130hConfig config)
{
    int ret = 0;
    int i = 0;
    dvpStatus dvp_status = DVP_STATUS_OK;

    dvp_status = dvpSetBufferQueueSize(config.camera_handle,config.frame_num);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr, "%s: set camera buffer queue size failed\n",__func__);
        return -1;
    }

    if(imageBuffer.image != NULL)
    {
        free(imageBuffer.image);
        imageBuffer.image = NULL;
    }

    if(imageBuffer.image == NULL)
    {
        imageBuffer.image = (char *)malloc(sizeof(char) * config.frame_buf_size);
        if(imageBuffer.image == NULL)
        {
            fprintf(stderr, "%s: malloc imageBuffer.image failed\n",__func__);
            return -EAGAIN;
        }
    }

    freeImageHeap();

    ret = allocateImageHeap(config.image_heap_depth,config.frame_buf_size);
    if(ret != 0)
    {
        fprintf(stderr, "%s: allocate image heap failed\n",__func__);
    }

    freeImageUnitHeap();

    ret = allocateImageUnitHeap(config.image_heap_depth,config.frame_buf_size);
    if(ret != 0)
    {
        fprintf(stderr, "%s: allocate image unit heap failed\n",__func__);
    }

    return ret;
}

static int startCaptureImage(struct M3st130hConfig config)
{
    dvpStatus dvp_status = DVP_STATUS_OK;

    dvp_status = dvpStart(config.camera_handle);
    if(dvp_status != DVP_STATUS_OK)
    {
        fprintf(stderr,"%s: start capture image failed\n",__func__);
        return -1;
    }

    return 0;
}

static int captureImage(struct M3st130hConfig config,unsigned int *image_num)
{
    dvpStatus dvp_status = DVP_STATUS_OK;
    dvpFrame frame;
    void *image = NULL;
    struct ImageBuffer image_buf;
    double exposure_time = 0.0f;

    dvp_status = dvpGetFrame(config.camera_handle, &frame, &image, 3000);
    if(dvp_status == DVP_STATUS_OK)
    {
        image_buf.image     = (char *)image;
        image_buf.size      = frame.uBytes;
        image_buf.width     = frame.iWidth;
        image_buf.height    = frame.iHeight;
        image_buf.number    = (*image_num) ++;

        /* fprintf(stdout,"%s, frame:%lu, timestamp:%lu, %d*%d, %dbytes, format:%d\r\n",
                __func__,
                frame.uFrameID,
                frame.uTimestamp,
                frame.iWidth,
                frame.iHeight,
                frame.uBytes,
                frame.format); */

        imageHeapPut(&image_buf);
    }
    else if(dvp_status == DVP_STATUS_TIME_OUT)
    {
        fprintf(stderr,"%s: capture image timeout\n",__func__);
        return -1;
    }
    else
    {
        fprintf(stderr,"%s: capture image failed\n",__func__);
        return -1;
    }

    return 0;
}


void *thread_m3st130h(void *arg)
{
    int ret = 0;
    static unsigned char capture_failed_cnt = 0;
    struct CmdArgs *args = (struct CmdArgs *)arg;
    enum CameraState camera_state = INIT_CONFIG;
    unsigned int image_num =  0;

    while(1)
    {
        switch((unsigned char)camera_state)
        {
            case (unsigned char)INIT_CONFIG:            //初始化配置
                initCameraConfig(&cameraConfig,args);
                camera_state = CONNECT_CAMERA;
            break;

            case (unsigned char)CONNECT_CAMERA:         //链接相机
                ret = connectCamrea(&cameraConfig);
                if(ret != 0)
                {
                    fprintf(stderr,"%s: connect camera failed\n",__func__);
                    camera_state = DISCONNECT_CAMERA;
                }
                else
                {
                    camera_state = LOAD_DEFAULT_CONFIG;
                }
            break;

            case (unsigned char)LOAD_DEFAULT_CONFIG:    //加载默认配置
                ret = loadCameraDefaultConfig(cameraConfig,(const char *)args->m3s_cam_def_conf_file);
                if(ret != 0)
                {
                    fprintf(stderr,"%s: load camera default config failed\n",__func__);
                    camera_state = DISCONNECT_CAMERA;
                }
                else
                {
                    camera_state = QUERY_CAMERA_CONFIG;
                }
            break;

            case (unsigned char)QUERY_CAMERA_CONFIG:    //获取相机配置
                ret = queryCameraConfig(&cameraConfig);
                printCameraConfig(cameraConfig);
                if(ret != 0)
                {
                    fprintf(stderr,"%s: query camera config failed\n",__func__);
                }
                camera_state = LOAD_USER_CONFIG;
            break;

            case (unsigned char)LOAD_USER_CONFIG:       //加载用户配置
                ret = loadCameraUserConfig(&cameraConfig,&userM3st130hConfig,args->m3s_cam_user_conf_file);
                if(ret != 0)
                {
                    fprintf(stderr,"%s: load camera user config failed\n",__func__);
                    camera_state = ALLOC_FRAME_BUFFER;
                }
                else
                {
                    camera_state = SET_USER_CONFIG;
                }
                printCameraConfig(userM3st130hConfig);
            break;

            case (unsigned char)SET_USER_CONFIG:        //设置用户配置
                setCameraUserConfig(&cameraConfig,&userM3st130hConfig);
                printCameraConfig(cameraConfig);
                camera_state = ALLOC_FRAME_BUFFER;
            break;

            case (unsigned char)ALLOC_FRAME_BUFFER:     //申请帧缓存
                ret = reallocateCameraBuffer(cameraConfig);
                if(ret != 0)
                {
                    fprintf(stderr,"%s: alloc camera buffer failed\n",__func__);
                    camera_state = DISCONNECT_CAMERA;
                }
                else
                {
                    camera_state = START_CAPTURE;
                }
            break;

            case (unsigned char)START_CAPTURE:          //开始采集
                ret = startCaptureImage(cameraConfig);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: start capture image failed\n",__func__);
                    camera_state =  DISCONNECT_CAMERA;
                }
                else
                {
                    camera_state = CAPTURE_IMAGE;
                }
            break;

            case (unsigned char)CAPTURE_IMAGE:          //采集图像
                ret = captureImage(cameraConfig,&image_num);
                if(ret == 0)
                {
                    capture_failed_cnt = 0;

                    sem_post(&sem_t_ImageHeap);
                }
                else
                {
                    capture_failed_cnt ++;

                    if(capture_failed_cnt >= M3ST130H_MAX_CAPTURE_FAILED_CNT)
                    {
                        camera_state = DISCONNECT_CAMERA;
                    }
                }
            break;

            case (unsigned char)DISCONNECT_CAMERA:      //断开相机链接
                disconnectCamera(cameraConfig);
                usleep(1000 * 10);
                camera_state = INIT_CONFIG;
            break;

            default:
                camera_state = DISCONNECT_CAMERA;
            break;
        }

        ret = recvCameraResetMsg();
        if(ret != -1)
        {
            image_num = 0;
//            camera_state = DISCONNECT_CAMERA;
        }

        ret = recvSync1HzSuccessMsg();
        if(ret != -1)
        {
           sendFrameRateMsgToThreadSync(cameraConfig.trigger_frame_rate);
        }
    }
}