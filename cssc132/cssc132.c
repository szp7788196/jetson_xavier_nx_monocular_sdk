#include "cssc132.h"

struct Cssc132Config cssc132Config;
struct Cssc132Config usercssc132config;


static void initCssc132Config(struct Cssc132Config *config,struct CmdArgs *args)
{
    if(config == NULL || args == NULL)
    {
        fprintf(stderr, "%s: input paras is NULL\n",__func__);
        return;
    }

    memset(config,0,sizeof(struct Cssc132Config));

    if(args->camera1 != NULL)
    {
        config->camera = (char *)malloc(strlen(args->camera1) + 1);
        memset(config->camera,0,strlen(args->camera1) + 1);
        memcpy(config->camera,args->camera1,strlen(args->camera1));
    }
    else
    {
        config->camera = (char *)malloc(strlen("/dev/video0") + 1);
        memset(config->camera,0,strlen("/dev/video0") + 1);
        memcpy(config->camera,"/dev/video0",strlen("/dev/video0"));
    }

    if(args->camera1_ctrl != NULL)
    {
        config->ctrl = (char *)malloc(strlen(args->camera1_ctrl) + 1);
        memset(config->ctrl,0,strlen(args->camera1_ctrl) + 1);
        memcpy(config->ctrl,args->camera1_ctrl,strlen(args->camera1_ctrl));
    }
    else
    {
        config->ctrl = (char *)malloc(strlen("/dev/cssc132_ctrl_0") + 1);
        memset(config->ctrl,0,strlen("/dev/cssc132_ctrl_0") + 1);
        memcpy(config->ctrl,"/dev/cssc132_ctrl_0",strlen("/dev/cssc132_ctrl_0"));
    }

    config->frame_num                                   = args->frame_num;
    config->image_heap_depth                            = args->image_heap_depth;
    config->trigger_frame_rate                          = 30.0f;
    config->capture_timeout                             = args->capture_timeout;
}

static void printCssc132Config(struct Cssc132Config *config)
{
    unsigned char i = 0;

    if(config == NULL)
    {
        fprintf(stderr, "%s: input paras is NULL\n",__func__);
        return;
    }

    printf("|====================== Cssc132Config begin =======================\n");
    printf("| camera                                      = %s\n",config->camera);
    printf("| ctrl                                        = %s\n",config->ctrl);
    printf("| i2c_addr                                    = 0x%02X\n",config->i2c_addr);
    printf("| device_id                                   = 0x%02X\n",config->device_id);
    printf("| hardware_ver                                = 0x%02X\n",config->hardware_ver);
    printf("| firmware_ver                                = 0x%02X\n",config->firmware_ver);
    printf("| camera_capability                           = 0x%04X\n",config->camera_capability);
    printf("| product_module                              = 0x%04X\n",config->product_module);
    printf("| support_format.number                       = %d\n",config->support_format.number);
    for(i = 0; i < config->support_format.number; i ++)
    {
        printf("| support_format.format[%d].widht              = %d\n",i,config->support_format.format[i].widht);
        printf("| support_format.format[%d].height             = %d\n",i,config->support_format.format[i].height);
        printf("| support_format.format[%d].frame_rate         = %d\n",i,config->support_format.format[i].frame_rate);
    }
    printf("| current_format.widht                        = %d\n",config->current_format.widht);
    printf("| current_format.height                       = %d\n",config->current_format.height);
    printf("| current_format.frame_rate                   = %d\n",config->current_format.frame_rate);
    printf("| isp_capability                              = 0x%08X\n",config->isp_capability);
    printf("| power_hz                                    = %d\n",config->power_hz);
    printf("| stream_mode                                 = %d\n",config->stream_mode);
    printf("| day_night_mode                              = %d\n",config->day_night_mode);
    printf("| hue                                         = %d\n",config->hue);
    printf("| contrast                                    = %d\n",config->contrast);
    printf("| saturation                                  = %d\n",config->saturation);
    printf("| exposure_state.expo_time                    = %d\n",config->exposure_state.expo_time);
    printf("| exposure_state.again                        = %f\n",(float)config->exposure_state.again.inter + (float)config->exposure_state.again.dec / 10.0f);
    printf("| exposure_state.dgain                        = %f\n",(float)config->exposure_state.dgain.inter + (float)config->exposure_state.dgain.dec / 10.0f);
    printf("| wb_sate.rgain                               = %d\n",config->wb_sate.rgain);
    printf("| wb_sate.ggain                               = %d\n",config->wb_sate.ggain);
    printf("| config->wb_sate.bgain                       = %d\n",config->wb_sate.bgain);
    printf("| wb_sate.color_temp                          = %d\n",config->wb_sate.color_temp);
    printf("| exposure_frame_mode                         = %d\n",config->exposure_frame_mode);
    printf("| slow_shutter_gain                           = %f\n",(float)config->slow_shutter_gain.inter + (float)config->slow_shutter_gain.dec / 10.0f);
    printf("| exposure_mode                               = %d\n",config->exposure_mode);
    printf("| manual_exposure_time                        = %d\n",config->manual_exposure_time);
    printf("| manual_exposure_again                       = %f\n",(float)config->manual_exposure_again.inter + (float)config->manual_exposure_again.dec / 10.0f);
    printf("| manual_exposure_dgain                       = %f\n",(float)config->manual_exposure_dgain.inter + (float)config->manual_exposure_dgain.dec / 10.0f);
    printf("| direct_manual_exposure_time                 = %d\n",config->direct_manual_exposure_time);
    printf("| direct_manual_exposure_again                = %f\n",(float)config->direct_manual_exposure_again.inter + (float)config->direct_manual_exposure_again.dec / 10.0f);
    printf("| direct_manual_exposure_dgain                = %f\n",(float)config->direct_manual_exposure_dgain.inter + (float)config->direct_manual_exposure_dgain.dec / 10.0f);
    printf("| awb_mode                                    = %d\n",config->awb_mode);
    printf("| mwb_color_temperature                       = %d\n",config->mwb_color_temperature);
    printf("| mwb_gain.rgain                              = %f\n",(float)((config->mwb_gain.inter >> 4) % 0x0F) + (float)(config->mwb_gain.inter % 0x0F) / 16.0f);
    printf("| mwb_gain.bgain                              = %f\n",(float)((config->mwb_gain.dec >> 4) % 0x0F) + (float)(config->mwb_gain.dec % 0x0F) / 16.0f);
    printf("| image_direction                             = %d\n",config->image_direction);
    printf("| auto_exposure_target_brightness             = %d\n",config->auto_exposure_target_brightness);
    printf("| auto_exposure_max_time                      = %ld\n",(unsigned long int)config->auto_exposure_max_time);
    printf("| auto_exposure_max_gain                      = %f\n",(float)config->auto_exposure_max_gain.inter + (float)config->auto_exposure_max_gain.dec / 10.0f);
    printf("| hardware_trigger_edge                       = %d\n",config->hardware_trigger_edge);
    printf("| hardware_trigger_delete_bouncer_time.enable = %d\n",config->hardware_trigger_delete_bouncer_time.enable);
    printf("| hardware_trigger_delete_bouncer_time.time   = %d\n",config->hardware_trigger_delete_bouncer_time.time);
    printf("| hardware_trigger_delay                      = %d\n",config->hardware_trigger_delay);
    printf("| pick_one_mode                               = %d\n",config->pick_one_mode);
    printf("| mipi_status.start                           = %d\n",config->mipi_status.start);
    printf("| mipi_status.count                           = %d\n",config->mipi_status.count);
    printf("| led_strobe_enable                           = %d\n",config->led_strobe_enable);
    printf("| frame_num                                   = %d\n",config->frame_num);
    printf("| image_heap_depth                            = %d\n",config->image_heap_depth);
    printf("| trigger_frame_rate                          = %f\n",config->trigger_frame_rate);
    printf("| capture_timeout                             = %d\n",config->capture_timeout);
    printf("|======================= Cssc132Config end ========================\n");
}

static int connectCamrea(struct Cssc132Config *config)
{
    config->camera_fd = open (config->camera, O_RDWR | O_NONBLOCK, 0);
    if(0 > config->camera_fd) 
    {
        fprintf(stderr, "%s: open %s failed\n",__func__,config->camera);
		return -ENODEV;
	}

    config->ctrl_fd = open (config->ctrl, O_RDWR | O_NONBLOCK, 0);
    if(0 > config->ctrl_fd) 
    {
        fprintf(stderr, "%s: open %s failed\n",__func__,config->ctrl);
		return -ENODEV;
	}

    return 0;
}

static int disconnectCamera(struct Cssc132Config *config)
{
    int ret = 0;
    int i = 0;

    if(config->frame_buf != NULL)
    {
        for(i = 0; i < config->frame_num; i ++)
        {
            ret = munmap(config->frame_buf[i].start, config->frame_buf[i].length);
            if(ret == -1)
            {
                fprintf(stderr, "%s: munmap config->frame_buf[%d] failed\n",__func__,i);
            }
        }

        free(config->frame_buf);
    }

    if(config->image_buf.image != NULL)
    {
        free(config->image_buf.image);
        config->image_buf.image = NULL;
    }

    close(config->camera_fd);
    close(config->ctrl_fd);
    
    return ret;
}

static int v4l2QueryCameraInfo(struct Cssc132Config config)
{
    struct v4l2_capability cap;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_format fmt;

    fmtdesc.index = 0;  
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    printf("|===================== v4l2 camera info start =====================\n");

    /*获取驱动信息*/
    ioctl (config.camera_fd, VIDIOC_QUERYCAP, &cap);
    
    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        fprintf(stderr, "%s is no video capture device\n",config.camera);
        return -EPERM;
    }

    fprintf(stdout, "| Driver info:\n| \tDriver Name:%s\n| \tCard Name:%s\n| \tBus info:%s\n",cap.driver,cap.card,cap.bus_info);

    /*获取支持的格式*/
    fprintf(stdout, "| Support format:\n");  
    while(ioctl(config.camera_fd,VIDIOC_ENUM_FMT,&fmtdesc) != -1)  
    {  
        fprintf(stdout, "| \t%d.%s\n",fmtdesc.index + 1,fmtdesc.description);  
        fmtdesc.index ++;  
    }

    /*获取当前帧格式*/
	ioctl(config.camera_fd,VIDIOC_G_FMT,&fmt);  
	fprintf(stdout, "| Current format info:\n| \twidth:%d\n| \theight:%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);  
 
    fmtdesc.index = 0;  
	while(ioctl(config.camera_fd,VIDIOC_ENUM_FMT,&fmtdesc) != -1)
	{  
		if(fmtdesc.pixelformat)
		{  
			fprintf(stdout, "| \tformat:%s\n",fmtdesc.description);

			break;  
		}

		fmtdesc.index ++;  
	}

    printf("|====================== v4l2 camera info end ======================\n");

    return 0;
}

static void queryCssc132Config(struct Cssc132Config *config)
{
    int ret = 0;
    struct Cssc132SupportFormat support_format;
    struct Cssc132Format format;
    struct WhiteBalanceState wb_state;
    struct HardwareTriggerDeleteBouncerTime hd_trig_del_bou_time;
    struct MipiStatus mipi_status;
    struct ExposureState exposure_state;
    struct GainDisassemble gain;
    unsigned char temp_u8 = 0;
    unsigned short temp_u16 = 0;
    unsigned int temp_u32 = 0;

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_DEVICE_ID,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read device_id failed\n",__func__);
    }
    else
    {
        config->device_id = temp_u8;
    }
    
    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_HARDWARE_VER,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read hardware_ver failed\n",__func__);
    }
    else
    {
        config->hardware_ver = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_FIRMWARE_VER,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read firmware_ver failed\n",__func__);
    }
    else
    {
        config->firmware_ver = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_CAM_CAP,&temp_u16);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read firmware_ver failed\n",__func__);
    }
    else
    {
        config->firmware_ver = temp_u16;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_PRODUCT_MODULE,&temp_u16);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read product_module failed\n",__func__);
    }
    else
    {
        config->product_module = temp_u16;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_SUPPORT_FORMAT,&support_format);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read support_format failed\n",__func__);
    }
    else
    {
        config->support_format = support_format;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_CURRENT_FORMAT,&format);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read current_format failed\n",__func__);
    }
    else
    {
        config->current_format = format;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_CURRENT_FORMAT,&temp_u32);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read isp_capability failed\n",__func__);
    }
    else
    {
        config->isp_capability = temp_u32;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_POWER_HZ,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read power_hz failed\n",__func__);
    }
    else
    {
        config->power_hz = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_I2C_ADDR,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read i2c_addr failed\n",__func__);
    }
    else
    {
        config->i2c_addr = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_STREAM_MODE,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read stream_mode failed\n",__func__);
    }
    else
    {
        config->stream_mode = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_DAY_NIGHT_MODE,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read day_night_mode failed\n",__func__);
    }
    else
    {
        config->day_night_mode = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_HUE,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read hue failed\n",__func__);
    }
    else
    {
        config->hue = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_CONTRAST,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read contrast failed\n",__func__);
    }
    else
    {
        config->contrast = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_STATURATION,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read saturation failed\n",__func__);
    }
    else
    {
        config->saturation = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_EXPOSURE_STATE,&exposure_state);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read exposure_state failed\n",__func__);
    }
    else
    {
        config->exposure_state = exposure_state;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_WB_STATE,&wb_state);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read wb_sate failed\n",__func__);
    }
    else
    {
        config->wb_sate = wb_state;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_EXPOSURE_FRAME_MODE,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read exposure_frame_mode failed\n",__func__);
    }
    else
    {
        config->exposure_frame_mode = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_SLOW_SHUTTER_GAIN,&gain);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read slow_shutter_gain failed\n",__func__);
    }
    else
    {
        config->slow_shutter_gain = gain;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_EXPOSURE_MODE,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read exposure_mode failed\n",__func__);
    }
    else
    {
        config->exposure_mode = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_MANUAL_EXPOSURE_TIME,&temp_u32);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read manual_exposure_time failed\n",__func__);
    }
    else
    {
        config->manual_exposure_time = temp_u32;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_MANUAL_EXPOSURE_AGAIN,&gain);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read manual_exposure_again failed\n",__func__);
    }
    else
    {
        config->manual_exposure_again = gain;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_MANUAL_EXPOSURE_DGAIN,&gain);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read manual_exposure_dgain failed\n",__func__);
    }
    else
    {
        config->manual_exposure_dgain = gain;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_DIRECT_MANUAL_EXPOSURE_TIME,&temp_u32);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read direct_manual_exposure_time failed\n",__func__);
    }
    else
    {
        config->direct_manual_exposure_time = temp_u32;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_DIRECT_MANUAL_EXPOSURE_AGAIN,&gain);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read direct_manual_exposure_again failed\n",__func__);
    }
    else
    {
        config->direct_manual_exposure_again = gain;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_DIRECT_MANUAL_EXPOSURE_DGAIN,&gain);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read direct_manual_exposure_dgain failed\n",__func__);
    }
    else
    {
        config->direct_manual_exposure_dgain = gain;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_AWB_MODE,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read awb_mode failed\n",__func__);
    }
    else
    {
        config->awb_mode = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_MWB_COLOR_TEMPERATURE,&temp_u32);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read mwb_color_temperature failed\n",__func__);
    }
    else
    {
        config->mwb_color_temperature = temp_u32;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_MWB_GAIN,&gain);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read mwb_gain failed\n",__func__);
    }
    else
    {
        config->mwb_gain = gain;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_IMAGE_DIRECTION,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read image_direction failed\n",__func__);
    }
    else
    {
        config->image_direction = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_EXPOSURE_TARGET_BRIGHTNESS,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read auto_exposure_target_brightness failed\n",__func__);
    }
    else
    {
        config->auto_exposure_target_brightness = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_AUTO_EXPOSURE_MAX_TIME,&temp_u32);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read auto_exposure_max_time failed\n",__func__);
    }
    else
    {
        config->auto_exposure_max_time = temp_u32;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_AUTO_EXPOSURE_MAX_GAIN,&gain);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read auto_exposure_max_gain failed\n",__func__);
    }
    else
    {
        config->auto_exposure_max_gain = gain;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_HARDWARE_TRIGGER_EDGE,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read hardware_trigger_edge failed\n",__func__);
    }
    else
    {
        config->hardware_trigger_edge = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_HARDWARE_TRIGGER_DELETE_BOUNCER_TIME,&hd_trig_del_bou_time);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read hardware_trigger_delete_bouncer_time failed\n",__func__);
    }
    else
    {
        config->hardware_trigger_delete_bouncer_time = hd_trig_del_bou_time;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_HARDWARE_TRIGGER_DELAY,&temp_u32);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read hardware_trigger_delay failed\n",__func__);
    }
    else
    {
        config->hardware_trigger_delay = temp_u32;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_PICK_MODE,&temp_u8);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read pick_one_mode failed\n",__func__);
    }
    else
    {
        config->pick_one_mode = temp_u8;
    }

    ret = ioctl(config->ctrl_fd,IOC_CSSC132_CTRL_R_MIPI_STATUS,&mipi_status);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl read mipi_status failed\n",__func__);
    }
    else
    {
        config->mipi_status = mipi_status;
    }
}

static int loadCssc132UserConfig(char *filename)
{
    int have_diff = -1;
    FILE *fp;
    char str[128] = {0};
    unsigned int file_len = 0l;
    char *file_buf = NULL;
    char *msg = NULL;
    unsigned short pos = 0;
    char temp_buf[32] = {0};
    long int temp = 0;
    double temp_f = 0.0f;
    char *endptr = NULL;

    fp = fopen(filename, "rt");
    if(fp == NULL)
    {
        fprintf(stderr, "%s: Could not open camera user parameters file\n",__func__);
		return -ENOENT;
    }

    fseek(fp,0,SEEK_END);

    file_len = ftell(fp);
    if(file_len < 1)
    {
        fprintf(stderr, "%s: query camera user parameters file length failed\n",__func__);
		return -EAGAIN;
    }

    if(file_len > CSSC132_MAX_USER_CONFIG_FILE_LEN || file_len < CSSC132_MIN_USER_CONFIG_FILE_LEN)
    {
        fprintf(stderr, "%s: camera user parameters file length error\n",__func__);
		return -EFBIG;
    }

    file_buf = (char *)malloc(sizeof(char) * (file_len + 1));
    if(file_buf == NULL)
    {
        fprintf(stderr, "%s: alloc user parameters file buf failed\n",__func__);
		return -EPERM;
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
		return -EINVAL;
    }

    memcpy(&usercssc132config,&cssc132Config,sizeof(struct Cssc132Config));

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"format_widht", file_len, strlen("format_widht"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 480 && temp <= 1280)
            {
                usercssc132config.current_format.widht = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"format_height", file_len, strlen("format_height"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 480 && temp <= 1280)
            {
                usercssc132config.current_format.height = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"format_frame_rate", file_len, strlen("format_frame_rate"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 2 && temp <= 120)
            {
                usercssc132config.current_format.frame_rate = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"trigger_frame_rate", file_len, strlen("trigger_frame_rate"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            if(temp_f >= 45.0f && temp_f <= 120.0f)
            {
                usercssc132config.trigger_frame_rate = temp_f;
                have_diff = 1;
            }
        }
    }

    if(usercssc132config.current_format.widht == 1280 && 
        usercssc132config.current_format.height == 1080 && 
        usercssc132config.current_format.frame_rate <=  45 && 
        usercssc132config.current_format.frame_rate >= 2)
    {
        if(usercssc132config.trigger_frame_rate > 45.0f)
        {
            fprintf(stderr, "%s: trigger_frame_rate = %f is too large,fixed to 45fps\n",
                    __func__,usercssc132config.trigger_frame_rate);
            usercssc132config.trigger_frame_rate = 45.0f;
        }
    }
    else if(usercssc132config.current_format.widht == 1280 && 
            usercssc132config.current_format.height == 720  && 
            usercssc132config.current_format.frame_rate <=  60 && 
            usercssc132config.current_format.frame_rate >= 2)
    {
        if(usercssc132config.trigger_frame_rate > 60.0f)
        {
            fprintf(stderr, "%s: trigger_frame_rate = %f is too large,fixed to 60fps\n",
                    __func__,usercssc132config.trigger_frame_rate);
            usercssc132config.trigger_frame_rate = 60.0f;
        }
    }
    else if(usercssc132config.current_format.widht == 640  && 
            usercssc132config.current_format.height == 480  && 
            usercssc132config.current_format.frame_rate <= 120 && 
            usercssc132config.current_format.frame_rate >= 2)
    {
        if(usercssc132config.trigger_frame_rate > 120.0f)
        {
            fprintf(stderr, "%s: trigger_frame_rate = %f is too large,fixed to 120fps\n",
                    __func__,usercssc132config.trigger_frame_rate);
            usercssc132config.trigger_frame_rate = 120.0f;
        }
    }
    else if(usercssc132config.current_format.widht == 1080 && 
            usercssc132config.current_format.height == 1208 && 
            usercssc132config.current_format.frame_rate <=  45 && 
            usercssc132config.current_format.frame_rate >= 2)
    {
        if(usercssc132config.trigger_frame_rate > 45.0f)
        {
            fprintf(stderr, "%s: trigger_frame_rate = %f is too large,fixed to 45fps\n",
                    __func__,usercssc132config.trigger_frame_rate);
            usercssc132config.trigger_frame_rate = 45.0f;
        }
    }
    else if(usercssc132config.current_format.widht == 720  && 
            usercssc132config.current_format.height == 1280 && 
            usercssc132config.current_format.frame_rate <=  60 && 
            usercssc132config.current_format.frame_rate >= 2)
    {
        if(usercssc132config.trigger_frame_rate > 60.0f)
        {
            fprintf(stderr, "%s: trigger_frame_rate = %f is too large,fixed to 60fps\n",
                    __func__,usercssc132config.trigger_frame_rate);
            usercssc132config.trigger_frame_rate = 60.0f;
        }
    }
    else if(usercssc132config.current_format.widht == 480  && 
            usercssc132config.current_format.height == 640  && 
            usercssc132config.current_format.frame_rate <= 120 && 
            usercssc132config.current_format.frame_rate >= 2)
    {
        if(usercssc132config.trigger_frame_rate > 120.0f)
        {
            fprintf(stderr, "%s: trigger_frame_rate = %f is too large,fixed to 120fps\n",
                    __func__,usercssc132config.trigger_frame_rate);
            usercssc132config.trigger_frame_rate = 120.0f;
        }
    }
    else
    {
        fprintf(stderr, "%s: user conf format_width = %d,format_height = %d,frame_rate = %d,trigger_frame_rate = %f,"
                "do not match cssc132 sensor,"
                "fixed to 1280 720 60 60\n",
                __func__,usercssc132config.current_format.widht,usercssc132config.current_format.height,
                usercssc132config.current_format.frame_rate,usercssc132config.trigger_frame_rate);

        usercssc132config.current_format.widht = 1280;
        usercssc132config.current_format.height = 720;
        usercssc132config.current_format.frame_rate =  60;
        usercssc132config.trigger_frame_rate = 60;
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"power_hz", file_len, strlen("power_hz"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp == 50 || temp == 60)
            {
                usercssc132config.power_hz = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"stream_mode", file_len, strlen("stream_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp == 0 || temp == 2 || temp == 3)
            {
                usercssc132config.stream_mode = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"day_night_mode", file_len, strlen("day_night_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 4)
            {
                usercssc132config.day_night_mode = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"hue", file_len, strlen("hue"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= 100)
            {
                usercssc132config.hue = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"contrast", file_len, strlen("contrast"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= 100)
            {
                usercssc132config.contrast = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"saturation", file_len, strlen("saturation"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= 100)
            {
                usercssc132config.saturation = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"exposure_frame_mode", 
                   file_len, strlen("exposure_frame_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= 1)
            {
                usercssc132config.exposure_frame_mode = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"slow_shutter_gain", file_len, strlen("slow_shutter_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);

            usercssc132config.slow_shutter_gain.inter = (unsigned char)temp_f;
            usercssc132config.slow_shutter_gain.dec = (unsigned char)((unsigned int )(temp_f * 10.0f) % 10);
            have_diff = 1;
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"exposure_mode", file_len, strlen("exposure_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= 2)
            {
                usercssc132config.exposure_mode = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"manual_exposure_time", 
                   file_len, strlen("manual_exposure_time"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= (unsigned int)(1000000 / usercssc132config.current_format.frame_rate))
            {
                usercssc132config.manual_exposure_time = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"manual_exposure_again", 
                   file_len, strlen("manual_exposure_again"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);

            usercssc132config.manual_exposure_again.inter = (unsigned char)temp_f;
            usercssc132config.manual_exposure_again.dec = (unsigned char)((unsigned int )(temp_f * 10.0f) % 10);
            have_diff = 1;
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"manual_exposure_dgain", 
                   file_len, strlen("manual_exposure_dgain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);

            usercssc132config.manual_exposure_dgain.inter = (unsigned char)temp_f;
            usercssc132config.manual_exposure_dgain.dec = (unsigned char)((unsigned int )(temp_f * 10.0f) % 10);
            have_diff = 1;
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"direct_manual_exposure_time", 
                   file_len, strlen("direct_manual_exposure_time"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= (unsigned int)(1000000 / usercssc132config.current_format.frame_rate))
            {
                usercssc132config.direct_manual_exposure_time = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"direct_manual_exposure_again", 
                   file_len, strlen("direct_manual_exposure_again"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);

            usercssc132config.direct_manual_exposure_again.inter = (unsigned char)temp_f;
            usercssc132config.direct_manual_exposure_again.dec = (unsigned char)((unsigned int )(temp_f * 10.0f) % 10);
            have_diff = 1;
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"direct_manual_exposure_dgain", 
                   file_len, strlen("direct_manual_exposure_dgain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);

            usercssc132config.direct_manual_exposure_dgain.inter = (unsigned char)temp_f;
            usercssc132config.direct_manual_exposure_dgain.dec = (unsigned char)((unsigned int )(temp_f * 10.0f) % 10);
            have_diff = 1;
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"awb_mode", file_len, strlen("awb_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= 2)
            {
                usercssc132config.awb_mode = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"mwb_color_temperature", 
                   file_len, strlen("mwb_color_temperature"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 1500 && temp <= 15000)
            {
                usercssc132config.mwb_color_temperature = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"mwb_gain_rgain", file_len, strlen("mwb_gain_rgain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            if(temp_f >= 0.0f && temp_f <= 15.9375f)
            {
                usercssc132config.mwb_gain.inter = ((((unsigned char)temp_f) << 4) & 0xF0) + 
                                                   (unsigned char)((temp_f - (unsigned char)temp_f) * 16.0f);
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"mwb_gain_bgain", file_len, strlen("mwb_gain_bgain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            if(temp_f >= 0.0f && temp_f <= 15.9375f)
            {
                usercssc132config.mwb_gain.dec = ((((unsigned char)temp_f) << 4) & 0xF0) + 
                                                 (unsigned char)((temp_f - (unsigned char)temp_f) * 16.0f);
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"image_direction", file_len, strlen("image_direction"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= 3)
            {
                usercssc132config.image_direction = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"auto_exposure_target_brightness", 
                   file_len, strlen("auto_exposure_target_brightness"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= 255)
            {
                usercssc132config.auto_exposure_target_brightness = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"auto_exposure_max_time", file_len, 
                   strlen("auto_exposure_max_time"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);

            if(usercssc132config.exposure_frame_mode == 1)
            {
                if(temp >= 100 && temp <= (unsigned int)(1000000 / usercssc132config.current_format.frame_rate))
                {
                    usercssc132config.auto_exposure_max_time = temp;
                    have_diff = 1;
                }
            }
            else if(usercssc132config.exposure_frame_mode == 0)
            {
                if(temp >= 100 && temp <= 0xFFFFFFFF)
                {
                    usercssc132config.auto_exposure_max_time = temp;
                    have_diff = 1;
                }
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"auto_exposure_max_gain", 
                   file_len, strlen("auto_exposure_max_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);

            usercssc132config.auto_exposure_max_gain.inter = (unsigned char)temp_f;
            usercssc132config.auto_exposure_max_gain.dec = (unsigned char)((unsigned int )(temp_f * 10.0f) % 10);
            have_diff = 1;
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"hardware_trigger_edge", 
                   file_len, strlen("hardware_trigger_edge"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0 && temp <= 1)
            {
                usercssc132config.hardware_trigger_edge = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"hardware_trigger_delete_bouncer_time_enable", 
                   file_len, strlen("hardware_trigger_delete_bouncer_time_enable"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp == 0 || temp == 1)
            {
                usercssc132config.hardware_trigger_delete_bouncer_time.enable = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"hardware_trigger_delete_bouncer_time", 
                   file_len, strlen("hardware_trigger_delete_bouncer_time"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0)
            {
                usercssc132config.hardware_trigger_delete_bouncer_time.time = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"hardware_trigger_delay", 
                   file_len, strlen("hardware_trigger_delay"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp >= 0)
            {
                usercssc132config.hardware_trigger_delay = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"pick_one_mode", file_len, strlen("pick_one_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp == 0 || temp == 1)
            {
                usercssc132config.pick_one_mode = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"led_strobe_enable", file_len, strlen("led_strobe_enable"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp == 0 || temp == 1)
            {
                usercssc132config.led_strobe_enable = temp;
                have_diff = 1;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"yuv_sequence", file_len, strlen("yuv_sequence"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp == 0 || temp == 1)
            {
                usercssc132config.yuv_sequence = temp;
                have_diff = 1;
            }
        }
    }

    return have_diff;
}

static int setCssc132UserConfig(struct Cssc132Config config,struct Cssc132Config user_config)
{
    int ret = 0;

    if(user_config.current_format.widht != config.current_format.widht || 
       user_config.current_format.height != config.current_format.height || 
       user_config.current_format.frame_rate != config.current_format.frame_rate)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_CURRENT_FORMAT,&user_config.current_format);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write current_format failed\n",__func__);
        }
    }

    if(user_config.power_hz != config.power_hz)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_POWER_HZ,&user_config.power_hz);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write power_hz failed\n",__func__);
        }
    }

    if(user_config.stream_mode != config.stream_mode)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_STREAM_MODE,&user_config.stream_mode);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write stream_mode failed\n",__func__);
        }
    }

    if(user_config.day_night_mode != config.day_night_mode)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_DAY_NIGHT_MODE,&user_config.day_night_mode);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write day_night_mode failed\n",__func__);
        }
    }

    if(user_config.hue != config.hue)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_HUE,&user_config.hue);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write hue failed\n",__func__);
        }
    }

    if(user_config.contrast != config.contrast)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_CONTRAST,&user_config.contrast);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write contrast failed\n",__func__);
        }
    }

    if(user_config.saturation != config.saturation)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_STATURATION,&user_config.saturation);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write saturation failed\n",__func__);
        }
    }

    if(user_config.exposure_frame_mode != config.exposure_frame_mode)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_EXPOSURE_FRAME_MODE,&user_config.exposure_frame_mode);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write exposure_frame_mode failed\n",__func__);
        }
    }

    if(user_config.slow_shutter_gain.inter != config.slow_shutter_gain.inter ||
       user_config.slow_shutter_gain.dec != config.slow_shutter_gain.dec)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_SLOW_SHUTTER_GAIN,&user_config.slow_shutter_gain);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write slow_shutter_gain failed\n",__func__);
        }
    }

    if(user_config.exposure_mode != config.exposure_mode)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_EXPOSURE_MODE,&user_config.exposure_mode);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write exposure_mode failed\n",__func__);
        }
    }

    if(user_config.manual_exposure_time != config.manual_exposure_time)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_MANUAL_EXPOSURE_TIME,&user_config.manual_exposure_time);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write manual_exposure_time failed\n",__func__);
        }
    }

    if(user_config.manual_exposure_again.inter != config.manual_exposure_again.inter ||
       user_config.manual_exposure_again.dec != config.manual_exposure_again.dec)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_MANUAL_EXPOSURE_AGAIN,&user_config.manual_exposure_again);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write manual_exposure_again failed\n",__func__);
        }
    }

    if(user_config.manual_exposure_dgain.inter != config.manual_exposure_dgain.inter ||
       user_config.manual_exposure_dgain.dec != config.manual_exposure_dgain.dec)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_MANUAL_EXPOSURE_DGAIN,&user_config.manual_exposure_dgain);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write manual_exposure_dgain failed\n",__func__);
        }
    }

    if(user_config.direct_manual_exposure_time != config.direct_manual_exposure_time)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_DIRECT_MANUAL_EXPOSURE_TIME,
                    &user_config.direct_manual_exposure_time);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write direct_manual_exposure_time failed\n",__func__);
        }
    }

    if(user_config.direct_manual_exposure_again.inter != config.direct_manual_exposure_again.inter ||
       user_config.direct_manual_exposure_again.dec != config.direct_manual_exposure_again.dec)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_DIRECT_MANUAL_EXPOSURE_AGAIN,
                    &user_config.direct_manual_exposure_again);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write direct_manual_exposure_again failed\n",__func__);
        }
    }

    if(user_config.direct_manual_exposure_dgain.inter != config.direct_manual_exposure_dgain.inter ||
       user_config.direct_manual_exposure_dgain.dec != config.direct_manual_exposure_dgain.dec)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_DIRECT_MANUAL_EXPOSURE_DGAIN,
                    &user_config.direct_manual_exposure_dgain);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write direct_manual_exposure_dgain failed\n",__func__);
        }
    }

    if(user_config.awb_mode != config.awb_mode)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_AWB_MODE,&user_config.awb_mode);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write awb_mode failed\n",__func__);
        }
    }

    if(user_config.mwb_color_temperature != config.mwb_color_temperature)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_MWB_COLOR_TEMPERATURE,&user_config.mwb_color_temperature);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write mwb_color_temperature failed\n",__func__);
        }
    }

    if(user_config.mwb_gain.inter != config.mwb_gain.inter ||
       user_config.mwb_gain.dec != config.mwb_gain.dec)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_MWB_GAIN,&user_config.mwb_gain);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write mwb_gain failed\n",__func__);
        }
    }

    if(user_config.image_direction != config.image_direction)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_IMAGE_DIRECTION,&user_config.image_direction);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write image_direction failed\n",__func__);
        }
    }

    if(user_config.auto_exposure_target_brightness != config.auto_exposure_target_brightness)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_EXPOSURE_TARGET_BRIGHTNESS,
                    &user_config.auto_exposure_target_brightness);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write auto_exposure_target_brightness failed\n",__func__);
        }
    }

    if(user_config.auto_exposure_max_time != config.auto_exposure_max_time)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_AUTO_EXPOSURE_MAX_TIME,&user_config.auto_exposure_max_time);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write auto_exposure_max_time failed\n",__func__);
        }
    }

    if(user_config.auto_exposure_max_gain.inter != config.auto_exposure_max_gain.inter ||
       user_config.auto_exposure_max_gain.dec != config.auto_exposure_max_gain.dec)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_AUTO_EXPOSURE_MAX_GAIN,&user_config.auto_exposure_max_gain);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write auto_exposure_max_gain failed\n",__func__);
        }
    }

    if(user_config.hardware_trigger_edge != config.hardware_trigger_edge)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_HARDWARE_TRIGGER_EDGE,&user_config.hardware_trigger_edge);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write hardware_trigger_edge failed\n",__func__);
        }
    }

    if(user_config.hardware_trigger_delete_bouncer_time.enable != config.hardware_trigger_delete_bouncer_time.enable ||
       user_config.hardware_trigger_delete_bouncer_time.time != config.hardware_trigger_delete_bouncer_time.time)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_HARDWARE_TRIGGER_DELETE_BOUNCER_TIME,
                    &user_config.hardware_trigger_delete_bouncer_time);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write hardware_trigger_delete_bouncer_time failed\n",__func__);
        }
    }

    if(user_config.hardware_trigger_delay != config.hardware_trigger_delay)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_HARDWARE_TRIGGER_DELAY,&user_config.hardware_trigger_delay);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write hardware_trigger_delay failed\n",__func__);
        }
    }

    if(user_config.pick_one_mode != config.pick_one_mode)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_PICK_MODE,&user_config.pick_one_mode);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write pick_one_mode failed\n",__func__);
        }
    }

    if(user_config.led_strobe_enable != config.led_strobe_enable)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_LED_STROBE_ENABLE,&user_config.led_strobe_enable);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write led_strobe_enable failed\n",__func__);
        }
    }

    if(user_config.yuv_sequence != config.yuv_sequence)
    {
        ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_YUV_SEQUENCE,&user_config.yuv_sequence);
        if(ret < 0)
        {
            fprintf(stderr, "%s: ioctl write yuv_sequence failed\n",__func__);
        }
    }

    ret = ioctl(config.ctrl_fd,IOC_CSSC132_CTRL_W_PARAMS_SAVE,NULL);
    if(ret < 0)
    {
        fprintf(stderr, "%s: ioctl write save all parameters failed\n",__func__);
    }

    return ret;
}

static int reallocateCameraBuffer(struct Cssc132Config *config)
{
    int ret = 0;
    int i = 0;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;

    memset(&fmt,0,sizeof(struct v4l2_format));
    memset(&req,0,sizeof(struct v4l2_requestbuffers));

    fmt.type                    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width           = config->current_format.widht; 
	fmt.fmt.pix.height          = config->current_format.height;
	fmt.fmt.pix.pixelformat     = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field           = V4L2_FIELD_INTERLACED;
    fmt.fmt.pix.bytesperline    = fmt.fmt.pix.width * 1;
    fmt.fmt.pix.sizeimage       = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;

    ret = ioctl(config->camera_fd, VIDIOC_S_FMT, &fmt);
    if(ret < 0)
    {
        fprintf(stderr, "%s: set format failed\n",__func__);
        return ret;
    }

    req.count	= config->frame_num;
	req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory	= V4L2_MEMORY_MMAP;

    ret = ioctl(config->camera_fd, VIDIOC_REQBUFS, &req);
    if(ret < 0)
    {
        fprintf(stderr, "%s: does not support memory map\n",__func__);
        return ret;
    }

    if(req.count < 2)
    {
        fprintf(stderr, "%s: insufficient buffer memory\n",__func__);
        return -EPERM;
    }

    config->frame_buf = calloc(req.count, sizeof(struct FrameBuffer));
    if(config->frame_buf == NULL)
    {
        fprintf(stderr, "%s: calloc config->frame_buf failed\n",__func__);
        return -EAGAIN;
    }

    for(i = 0; i < req.count; i ++)
    {
        struct v4l2_buffer buf;

        memset(&buf,0,sizeof(struct v4l2_buffer));

        buf.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory	= V4L2_MEMORY_MMAP;
		buf.index	= i;

        ret = ioctl(config->camera_fd, VIDIOC_QUERYBUF, &buf);
        if(ret < 0)
        {
            fprintf(stderr, "%s: calloc config->frame_buf.buf failed\n",__func__);
            return -EAGAIN;
        }

        config->frame_buf[i].length = buf.length;
		config->frame_buf[i].start = mmap(NULL,                               //start anywhere
								         buf.length,
								         PROT_READ | PROT_WRITE,              //required
								         MAP_SHARED,                          //recommended
								         config->camera_fd, buf.m.offset);

        if(config->frame_buf[i].start == MAP_FAILED)
		{
			fprintf(stderr, "%s: memory map frame buf failed\n",__func__);
            return -EAGAIN;
		}
    }

/*
    config->image_buf.size = fmt.fmt.pix.sizeimage;

    if(config->image_buf.image != NULL)
    {
        fprintf(stderr, "%s: config->image_buf.image dose not null\n",__func__);
        
        free(config->image_buf.image);
        config->image_buf.image = NULL;
    }

    config->image_buf.image = (char *)malloc(config->image_buf.size * sizeof(char));

    if(config->image_buf.image == NULL)
    {
        fprintf(stderr, "%s: malloc config->image_buf.image failed\n",__func__);
        return -EAGAIN;
    }
*/
    freeImageHeap(config->image_heap_depth);

    ret = allocateImageHeap(config->image_heap_depth,fmt.fmt.pix.sizeimage);
    if(ret != 0)
    {
        fprintf(stderr, "%s: allocate image heap failed\n",__func__);
    }

    return ret;
}

static int startCaptureImage(struct Cssc132Config config)
{
    int ret = 0;
    int i = 0;
	enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    for (i = 0; i < config.frame_num; i ++)
    {
        struct v4l2_buffer buf;

        memset(&buf,0,sizeof(struct v4l2_buffer));

        buf.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory	= V4L2_MEMORY_MMAP;
        buf.index	= i;

        ret = ioctl(config.camera_fd,VIDIOC_QBUF,&buf);
        if(ret < 0)
        {
            fprintf(stderr, "%s: VIDIOC_QBUF failed\n",__func__);
            return -EPERM;
        }
    }

    ret = ioctl(config.camera_fd,VIDIOC_STREAMON,&type);
    if(ret < 0)
    {
        fprintf(stderr, "%s: VIDIOC_STREAMON failed\n",__func__);
        return -EPERM;
    }

    return ret;
}

static void sendFrameRateMsgToThreadSync(struct Cssc132Config config)
{
    int ret = 0;
    double *frame_rate = NULL;

    if(config.stream_mode != 2)
    {
        return;
    }

    frame_rate = (double *)malloc(sizeof(double));
    if(frame_rate != NULL)
    {
        *frame_rate = config.trigger_frame_rate;

        ret = xQueueSend((key_t)KEY_FRAME_RATE_MSG,frame_rate);
        if(ret == -1)
        {
            fprintf(stderr, "%s: send cssc132 frame rate queue msg failed\n",__func__);
        }
    }
}

static int captureImage(struct Cssc132Config config,struct ImageBuffer *image_buf)
{
    int ret = 0;
    fd_set fds;
	struct timeval tv;
    struct v4l2_buffer buf;

    FD_ZERO(&fds);
	FD_SET(config.camera_fd,&fds);

    tv.tv_sec = config.capture_timeout / 1000;
	tv.tv_usec = (config.capture_timeout % 1000) * 1000;

    ret = select(config.camera_fd + 1,&fds,NULL,NULL,&tv);
    if(ret == -1)
    {
        fprintf(stderr, "%s: select error\n",__func__);
        usleep(1000 * 10);
        return -EPERM;
    }
    else if(ret == 0)
    {
        fprintf(stderr, "%s: select timeout\n",__func__);
        return -EAGAIN;
    }

    memset(&buf,0,sizeof(struct v4l2_buffer));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

    ret = ioctl(config.camera_fd, VIDIOC_DQBUF, &buf);
    if(ret == -1)
    {
        fprintf(stderr, "%s: ioctl VIDIOC_DQBUF failed\n",__func__);
        return -EPERM;
    }

    pthread_mutex_lock(&mutexImageHeap); 
    memcpy(image_buf->image,config.frame_buf[buf.index].start,config.frame_buf[buf.index].length);

    image_buf->size = config.frame_buf[buf.index].length;
    pthread_mutex_unlock(&mutexImageHeap);

    ret = ioctl(config.camera_fd, VIDIOC_QBUF, &buf);
    if(ret == -1)
    {
        fprintf(stderr, "%s: ioctl VIDIOC_QBUF failed\n",__func__);
    }

    return ret;
}

static int recvResetMsg(void)
{
    int ret = 0;
    unsigned char *reset = NULL;

    ret = xQueueReceive((key_t)KEY_CAMERA_RESET_MSG,(void **)&reset,0);
    if(ret == -1)
    {
        return -1;
    }

    free(reset);
    reset = NULL;

    return ret;
}

void *thread_cssc132(void *arg)
{
    int ret = 0;
    struct CmdArgs *args = (struct CmdArgs *)arg;
    enum CameraState camera_state = INIT_CONFIG;

    while(1)
    {
        switch((unsigned char)camera_state)
        {
            case (unsigned char)INIT_CONFIG:            //初始化配置
                initCssc132Config(&cssc132Config,(struct CmdArgs *)arg);
                camera_state = CONNECT_CAMERA;
            break;

            case (unsigned char)CONNECT_CAMERA:         //链接相机
                ret = connectCamrea(&cssc132Config);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: connect camera failed\n",__func__);
                    camera_state =  DISCONNECT_CAMERA;
                }
                camera_state = QUERY_CAMERA_CONFIG;
            break;

            case (unsigned char)QUERY_CAMERA_CONFIG:    //获取相机配置
                queryCssc132Config(&cssc132Config);
                printCssc132Config(&cssc132Config);
                camera_state = LOAD_USER_CONFIG;
            break;

            case (unsigned char)LOAD_USER_CONFIG:       //加载用户配置
                ret = loadCssc132UserConfig(args->mipi_cam_user_conf_file);
                if(ret != 1)
                {
                    fprintf(stderr, "%s: load camera user config failed\n",__func__);
                    camera_state = ALLOC_FRAME_BUFFER;
                }
                else
                {
                    camera_state = SET_USER_CONFIG;
                }
            break;

            case (unsigned char)SET_USER_CONFIG:        //设置用户配置
                setCssc132UserConfig(cssc132Config,usercssc132config);

                sleep(3);

                ret = v4l2QueryCameraInfo(cssc132Config);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: camera not support video capture\n",__func__);
                    camera_state =  DISCONNECT_CAMERA;
                }
                else
                {
                    camera_state = ALLOC_FRAME_BUFFER;
                }
                queryCssc132Config(&cssc132Config);
                printCssc132Config(&cssc132Config);
            break;

            case (unsigned char)ALLOC_FRAME_BUFFER:     //申请帧缓存
                ret = reallocateCameraBuffer(&cssc132Config);
                if(ret != 0)
                {
                    fprintf(stderr, "%s: alloc camera buffer failed\n",__func__);
                    camera_state =  DISCONNECT_CAMERA;
                }
                else
                {
                    sendFrameRateMsgToThreadSync(cssc132Config);

                    camera_state = START_CAPTURE;
                }
            break;

            case (unsigned char)START_CAPTURE:          //开始采集
                ret = startCaptureImage(cssc132Config);
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
                ret = captureImage(cssc132Config,imageHeap.heap[imageHeap.put_ptr]->image);
                if(ret == 0)
                {
                    pthread_cond_signal(&condImageHeap);

                    fprintf(stdout,"%s: capture iamge success,image_counter = %d, put_ptr = %d\n",
                            __func__,imageHeap.heap[imageHeap.put_ptr]->image->counter,imageHeap.put_ptr);
                }
            break;

            case (unsigned char)DISCONNECT_CAMERA:      //断开相机链接
                disconnectCamera(&cssc132Config);
                usleep(1000 * 1000);
                camera_state = INIT_CONFIG;
            break;

            default:
                camera_state = DISCONNECT_CAMERA;
            break;
        }

        ret = recvResetMsg();
        if(ret == 0)
        {
            camera_state = DISCONNECT_CAMERA;
        }
    }
}