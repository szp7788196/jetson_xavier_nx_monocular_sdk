#include "ui3240.h"

static struct Ui3240Config cameraConfig;
static struct Ui3240Config userUi3240Config;
static struct QueueMsgNormal FrameRateMsg;
static struct QueueMsgNormal ui3240ImageCounterMsg;
static int FrameRateMsgId = -1;
static int ui3240ImageCounterMsgId = -1;
static unsigned int ImageCounter = 0;

static bool isConnected(void)
{
    return (cameraConfig.camera_handle != (HIDS)0); 
}

static bool freeRunModeActive(void)
{
    return ((cameraConfig.camera_handle != (HIDS) 0) &&
           (is_SetExternalTrigger(cameraConfig.camera_handle, IS_GET_EXTERNALTRIGGER) == IS_SET_TRIGGER_OFF));
}

static bool extTriggerModeActive(void)
{
    return ((cameraConfig.camera_handle != (HIDS) 0) &&
           (is_SetExternalTrigger(cameraConfig.camera_handle, IS_GET_EXTERNALTRIGGER) == IS_SET_TRIGGER_HI_LO || 
            is_SetExternalTrigger(cameraConfig.camera_handle, IS_GET_EXTERNALTRIGGER) == IS_SET_TRIGGER_LO_HI));
}

static bool softTriggerModeActive(void)
{
    return ((cameraConfig.camera_handle != (HIDS) 0) &&
           (is_SetExternalTrigger(cameraConfig.camera_handle, IS_GET_EXTERNALTRIGGER) == IS_SET_TRIGGER_SOFTWARE));
}

static bool isCapturing(void)
{
    return ((cameraConfig.camera_handle != (HIDS) 0) &&
           (is_CaptureVideo(cameraConfig.camera_handle, IS_GET_LIVE) == TRUE));
}

static int gpioPwmConfig(HIDS hCam, double frame_rate, bool active)
{
    // Set GPIO2 as PWM output
    unsigned int nMode = IO_GPIO_2;
    int is_err = is_IO(hCam, IS_IO_CMD_PWM_SET_MODE,(void*)&nMode, sizeof(nMode));
    IO_PWM_PARAMS m_pwmParams;

    // Set the values of the PWM parameters
    m_pwmParams.dblFrequency_Hz = frame_rate;
    m_pwmParams.dblDutyCycle = (active ? 0.1:0.0); //TODO: What does the duty change?(active ? 0.1:0)

    is_err = is_IO(hCam, IS_IO_CMD_PWM_SET_PARAMS,(void*)&m_pwmParams, sizeof(m_pwmParams));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "pwm not set,error code is %d\n",is_err);
    }

    return is_err;
}

static int gpioInputConfig(HIDS hCam)
{
    // FOR THE GPIO 1 : INPUT
    IO_GPIO_CONFIGURATION gpioConfiguration;
    int is_err = IS_SUCCESS;

    // Set configuration of GPIO1
    gpioConfiguration.u32Gpio = IO_GPIO_1;
    gpioConfiguration.u32Configuration = IS_GPIO_TRIGGER;
    gpioConfiguration.u32State = 0;

    is_err = is_IO(hCam, IS_IO_CMD_GPIOS_SET_CONFIGURATION, (void*)&gpioConfiguration,sizeof(gpioConfiguration));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "GPIO1 config not done,error code is %d\n",is_err);
    }

    // // set configuration of GPIO2
    gpioConfiguration.u32Gpio = IO_GPIO_2;
    gpioConfiguration.u32Configuration = IS_GPIO_FLASH;
    gpioConfiguration.u32State = 0;
    is_err = is_IO(hCam, IS_IO_CMD_GPIOS_SET_CONFIGURATION, (void*)&gpioConfiguration,sizeof(gpioConfiguration));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "GPIO2 config not done,error code is %d\n",is_err);
    }

    return is_err;
}

static int setStandbyMode(void)
{
    int is_err = IS_SUCCESS;
    unsigned int nMode = IO_FLASH_MODE_OFF;
    unsigned int event = IS_SET_EVENT_FRAME;
    IS_INIT_EVENT init_event = {IS_SET_EVENT_FRAME, TRUE, FALSE};

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    if(extTriggerModeActive())
    {
        /* set the GPIO2 to generate PWM */
        is_err = gpioPwmConfig(cameraConfig.camera_handle, cameraConfig.frame_rate, false);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set GPIO2 as output (PWM),error code is %d\n",is_err);
            return is_err;
        }

        is_err = is_SetExternalTrigger(cameraConfig.camera_handle, IS_SET_TRIGGER_OFF);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not disable external trigger mode,error code is %d\n",is_err);
            return is_err;
        }

        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_INIT, &init_event, sizeof(IS_INIT_EVENT));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not init frame event 1,error code is %d\n",is_err);
        }

        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_DISABLE, &event, sizeof(unsigned int));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not disable frame event 1,error code is %d\n",is_err);
            return is_err;
        }

        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_EXIT, &event, sizeof(unsigned int));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not exit frame event,error code is %d\n",is_err);
        }

        // documentation seems to suggest that this is needed to disable external trigger mode (to go into free-run mode)
        is_SetExternalTrigger(cameraConfig.camera_handle, IS_GET_TRIGGER_STATUS);
        
        is_err = is_StopLiveVideo(cameraConfig.camera_handle, IS_WAIT);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not stop live video mode,error code is %d\n",is_err);
            return is_err;
        }

        fprintf(stderr, "Stopped external trigger mode\n");
    }
    else if(freeRunModeActive())
    {
        is_err = is_IO(cameraConfig.camera_handle, IS_IO_CMD_FLASH_SET_MODE,(void*)&nMode, sizeof(nMode));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not disable flash output,error code is %d\n",is_err);
            return is_err;
        }

        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_INIT, &init_event, sizeof(IS_INIT_EVENT));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not init frame event 2,error code is %d\n",is_err);
        }

        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_DISABLE, &event, sizeof(unsigned int));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not disable frame event 2,error code is %d\n",is_err);
            return is_err;
        }

        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_EXIT, &event, sizeof(unsigned int));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not exit frame event,error code is %d\n",is_err);
        }

        is_err = is_StopLiveVideo(cameraConfig.camera_handle, IS_WAIT);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not stop live video mode,error code is %d\n",is_err);
            return is_err;
        }

        fprintf(stderr, "Stopped free-run live video mode\n");
    }
    else if(softTriggerModeActive())
    {
        
    }

    is_err = is_CameraStatus(cameraConfig.camera_handle, IS_STANDBY, IS_GET_STATUS);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not set standby mode,error code is %d\n",is_err);
        return is_err;
    }

    return is_err;
}

static int connectCamrea(int camera_id)
{
    int is_err = IS_SUCCESS;
    int num_cameras = 0;

    // Terminate any existing opened cameras
    setStandbyMode();

    is_err = is_GetNumberOfCameras(&num_cameras);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed query for number of connected UEye cameras,error code is %d\n",is_err);
        return is_err;
    }
    else
    {
        if(num_cameras < 1)
        {
            fprintf(stderr, "No UEye cameras are connected\n");
            return IS_NO_SUCCESS;
        }
    }

    cameraConfig.camera_handle = (HIDS)cameraConfig.camera_id;

    is_err = is_InitCamera(&cameraConfig.camera_handle, NULL);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not open UEye camera ID %d\n",cameraConfig.camera_id);
        return is_err;
    }

    is_err = is_SetDisplayMode(cameraConfig.camera_handle, IS_SET_DM_DIB);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "does not support Device Independent Bitmap mode,error code is %d\n",is_err);
        return is_err;
    }

    is_err = is_GetSensorInfo(cameraConfig.camera_handle, &cameraConfig.camera_sensor_info);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not poll sensor information,error code is %d\n",is_err);
        return is_err;
    }
    
    return is_err;
}

static int disconnectCamera(void)
{
  int is_err = IS_SUCCESS;
  int i = 0;

  if(isConnected())
  {
    setStandbyMode();

    // Exit the image queue and clear sequence
    is_err = is_ImageQueue(cameraConfig.camera_handle, IS_IMAGE_QUEUE_CMD_EXIT, NULL, 0);
    is_err = is_ClearSequence(cameraConfig.camera_handle);

    // Release existing camera buffers
    if(cameraConfig.frame_buf != NULL)
    {
        for(i = 0; i < cameraConfig.frame_num; i ++)
        {
            if(cameraConfig.frame_buf[i] != NULL)
            {
                is_err = is_FreeImageMem(cameraConfig.camera_handle, cameraConfig.frame_buf[i], cameraConfig.frame_buf_id[i]);
                if(is_err != IS_SUCCESS)
                {
                    fprintf(stderr, "Failed to free frame_buf[%d]\n",i);
                }
            }

            cameraConfig.frame_buf[i] = NULL;
        }

        free(cameraConfig.frame_buf);
        cameraConfig.frame_buf = NULL;
    }

    if(cameraConfig.image_buf.image != NULL)
    {
        free(cameraConfig.image_buf.image);
        cameraConfig.image_buf.image = NULL;
    }

    if(cameraConfig.frame_buf_id != NULL)
    {
        free(cameraConfig.frame_buf_id);
        cameraConfig.frame_buf_id = NULL;
    }

    // Release camera handle
    is_err = is_ExitCamera(cameraConfig.camera_handle);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to release camera handle,error code is %d\n",is_err);
    }

    cameraConfig.camera_handle = (HIDS)0;

    fprintf(stderr, "camera disconnected success\n");
  }

  return is_err;
}

static int loadCameraDefaultConfig(wchar_t *filename, bool ignore_load_failure)
{
    int is_err = IS_SUCCESS;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    is_err = is_ParameterSet(cameraConfig.camera_handle, IS_PARAMETERSET_CMD_LOAD_FILE,filename, 0);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not load camera default parameters file,error code is %d\n",is_err);

        if(ignore_load_failure)
        {
            is_err = IS_SUCCESS;
        }

        return is_err;
    }
    else
    {
        fprintf(stderr, "========================= Congratulations!!! =========================\n");
        fprintf(stderr, "Successfully loaded camera default parameter file\n");
    }

    return is_err;
}

static int loadCameraUserConfig(char *filename)
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
    double temp_f = 0.0;
    char *endptr = NULL;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    fp = fopen(filename, "rt");
    if(fp == NULL)
    {
        fprintf(stderr, "Could not open camera user parameters file\n");
		return IS_NO_SUCCESS;
    }

    fseek(fp,0,SEEK_END);

    file_len = ftell(fp);
    if(file_len < 1)
    {
        fprintf(stderr, "query camera user parameters file length failed\n");
		return IS_NO_SUCCESS;
    }

    if(file_len > UI3240_MAX_USER_CONFIG_FILE_LEN || file_len < UI3240_MIN_USER_CONFIG_FILE_LEN)
    {
        fprintf(stderr, "camera user parameters file length error\n");
		return IS_NO_SUCCESS;
    }

    file_buf = (char *)malloc(sizeof(char) * (file_len + 1));
    if(file_buf == NULL)
    {
        fprintf(stderr, "alloc user parameters file buf failed\n");
		return IS_NO_SUCCESS;
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
        fprintf(stderr, "camera user parameters file missing head or tail\n");
		return IS_NO_SUCCESS;
    }

    memcpy(&userUi3240Config,&cameraConfig,sizeof(struct Ui3240Config));

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"master", file_len, strlen("master"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 1)
            {
                userUi3240Config.master = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"stereo", file_len, strlen("stereo"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 1)
            {
                userUi3240Config.stereo = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"capture_mode", file_len, strlen("capture_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 1)
            {
                userUi3240Config.capture_mode = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"capture_timeout", file_len, strlen("capture_timeout"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 65535)
            {
                userUi3240Config.capture_timeout = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"image_width", file_len, strlen("image_width"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 65535)
            {
                userUi3240Config.image_width = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"image_height", file_len, strlen("image_height"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 65535)
            {
                userUi3240Config.image_height = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"image_left", file_len, strlen("image_left"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 65535)
            {
                userUi3240Config.image_left = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"image_top", file_len, strlen("image_top"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 65535)
            {
                userUi3240Config.image_top = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"color_mode", file_len, strlen("color_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            if(strstr(msg, "mono8") != NULL)
            {
                userUi3240Config.color_mode = IS_CM_MONO8;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "bayer_rggb8") != NULL)
            {
                userUi3240Config.color_mode = IS_CM_SENSOR_RAW8;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "rgb8") != NULL)
            {
                userUi3240Config.color_mode = IS_CM_RGB8_PACKED;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "bgr8") != NULL)
            {
                userUi3240Config.color_mode = IS_CM_BGR8_PACKED;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"subsampling", file_len, strlen("subsampling"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 65535)
            {
                userUi3240Config.subsampling = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"binning", file_len, strlen("binning"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 65535)
            {
                userUi3240Config.binning = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"binning", file_len, strlen("binning"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 65535)
            {
                userUi3240Config.binning = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"sensor_scaling", file_len, strlen("sensor_scaling"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            if(temp_f <= 65535)
            {
                userUi3240Config.sensor_scaling = temp_f;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"auto_gain", file_len, strlen("auto_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            if(strstr(msg, "true") != NULL)
            {
                userUi3240Config.auto_gain = true;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "false") != NULL)
            {
                userUi3240Config.auto_gain = false;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"master_gain", file_len, strlen("master_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 255)
            {
                userUi3240Config.master_gain = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"red_gain", file_len, strlen("red_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 255)
            {
                userUi3240Config.red_gain = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"green_gain", file_len, strlen("green_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 255)
            {
                userUi3240Config.green_gain = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"blue_gain", file_len, strlen("blue_gain"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 255)
            {
                userUi3240Config.blue_gain = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"gain_boost", file_len, strlen("gain_boost"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            if(strstr(msg, "true") != NULL)
            {
                userUi3240Config.gain_boost = true;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "false") != NULL)
            {
                userUi3240Config.gain_boost = false;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"auto_exposure", file_len, strlen("auto_exposure"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            if(strstr(msg, "true") != NULL)
            {
                userUi3240Config.auto_exposure = true;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "false") != NULL)
            {
                userUi3240Config.auto_exposure = false;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"exposure_ms", file_len, strlen("exposure_ms"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            if(temp_f <= 65535)
            {
                userUi3240Config.exposure = temp_f;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"auto_white_balance", file_len, strlen("auto_white_balance"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            if(strstr(msg, "true") != NULL)
            {
                userUi3240Config.auto_white_balance = true;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "false") != NULL)
            {
                userUi3240Config.auto_white_balance = false;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"white_balance_red_offset", file_len, strlen("white_balance_red_offset"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 127 && temp > -127)
            {
                userUi3240Config.white_balance_red_offset = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"white_balance_blue_offset", file_len, strlen("white_balance_blue_offset"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 127 && temp > -127)
            {
                userUi3240Config.white_balance_blue_offset = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"auto_frame_rate", file_len, strlen("auto_frame_rate"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            if(strstr(msg, "true") != NULL)
            {
                userUi3240Config.auto_frame_rate = true;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "false") != NULL)
            {
                userUi3240Config.auto_frame_rate = false;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"frame_rate_hz", file_len, strlen("frame_rate_hz"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp_f = atof(temp_buf);
            if(temp_f <= 65535)
            {
                userUi3240Config.frame_rate = temp_f;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"pixel_clock", file_len, strlen("pixel_clock"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 65535)
            {
                userUi3240Config.pixel_clock = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"ext_trigger_mode", file_len, strlen("ext_trigger_mode"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            if(strstr(msg, "true") != NULL)
            {
                userUi3240Config.ext_trigger_mode = true;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "false") != NULL)
            {
                userUi3240Config.ext_trigger_mode = false;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"ext_trigger_delay", file_len, strlen("ext_trigger_delay"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp <= 4000000 && temp >= 15)
            {
                userUi3240Config.ext_trigger_delay = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"flash_delay", file_len, strlen("flash_delay"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp > 0)
            {
                userUi3240Config.flash_delay = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"flash_duration", file_len, strlen("flash_duration"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            temp = strtol(temp_buf, &endptr, 10);
            if(temp > 0)
            {
                userUi3240Config.flash_duration = temp;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"flip_upd", file_len, strlen("flip_upd"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            if(strstr(msg, "true") != NULL)
            {
                userUi3240Config.flip_upd = true;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "false") != NULL)
            {
                userUi3240Config.flip_upd = false;
                have_diff = IS_SUCCESS;
            }
        }
    }

    msg = file_buf;
    pos = mystrstr((unsigned char *)file_buf, (unsigned char *)"flip_lr", file_len, strlen("flip_lr"));
    if(pos != 0xFFFF && pos < file_len)
    {
        msg += pos;

        memset(temp_buf,0,32);

        get_str1((unsigned char *)msg, (unsigned char *)" = ", 1, (unsigned char *)";", 1, (unsigned char *)temp_buf);
        if(strlen(temp_buf) != 0 && strlen(temp_buf) < 32)
        {
            if(strstr(msg, "true") != NULL)
            {
                userUi3240Config.flip_lr = true;
                have_diff = IS_SUCCESS;
            }
            else if(strstr(msg, "false") != NULL)
            {
                userUi3240Config.flip_lr = false;
            }
        }
    }

    return have_diff;
}

static int reallocateCameraBuffer(void)
{
    int is_err = IS_SUCCESS;
    int i = 0;
    int frame_width = 0;
    int frame_height = 0;
    int cam_buffer_pitch = 0;

    // Stop capture to prevent access to memory buffer
    setStandbyMode();

    // Free existing memory from previous calls to reallocateCamBuffer()
    if(cameraConfig.frame_buf != NULL)
    {
        for(i = 0; i < cameraConfig.frame_num; i ++)
        {
            if(cameraConfig.frame_buf[i] != NULL)
            {
                is_err = is_FreeImageMem(cameraConfig.camera_handle, cameraConfig.frame_buf[i], cameraConfig.frame_buf_id[i]);
                if(is_err != IS_SUCCESS)
                {
                    fprintf(stderr, "Failed to free frame_buf[%d]\n",i);
                }
            }

            cameraConfig.frame_buf[i] = NULL;
        }

        free(cameraConfig.frame_buf);
        cameraConfig.frame_buf = NULL;
    }

    if(cameraConfig.image_buf.image != NULL)
    {
        free(cameraConfig.image_buf.image);
        cameraConfig.image_buf.image = NULL;
    }

    if(cameraConfig.frame_buf_id != NULL)
    {
        free(cameraConfig.frame_buf_id);
        cameraConfig.frame_buf_id = NULL;
    }

    cameraConfig.frame_buf = malloc(cameraConfig.frame_num * sizeof(char *));
    if(cameraConfig.frame_buf == NULL)
    {
        fprintf(stderr, "Failed to malloc cameraConfig.frame_buf\n");
        return is_err;
    }

    cameraConfig.frame_buf_id = (int *)malloc(cameraConfig.frame_num * sizeof(int));
    if(cameraConfig.frame_buf_id == NULL)
    {
        fprintf(stderr, "Failed to malloc cameraConfig.frame_buf_id\n");
        return is_err;
    }

    for(i = 0; i < cameraConfig.frame_num; i ++)
    {
        cameraConfig.frame_buf[i] = NULL;
        cameraConfig.frame_buf_id[i] = 0;
    }

    // Allocate new memory section for IDS driver to use as frame buffer
    frame_width = cameraConfig.image_width / (cameraConfig.sensor_scaling * cameraConfig.subsampling);
    frame_height = cameraConfig.image_height / (cameraConfig.sensor_scaling * cameraConfig.subsampling);

    is_err = is_ClearSequence(cameraConfig.camera_handle);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to clear sequence,error code is %d\n",is_err);
    }

    for(i = 0; i < cameraConfig.frame_num; i ++)
    {
        is_err = is_AllocImageMem(cameraConfig.camera_handle, frame_width, frame_height,
                                  cameraConfig.bits_per_pixel,
                                  &cameraConfig.frame_buf[i],
                                  &cameraConfig.frame_buf_id[i]);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to allocate image buffer[%d]\n",i);
            return is_err;
        }

        is_err = is_SetImageMem(cameraConfig.camera_handle, cameraConfig.frame_buf[i], cameraConfig.frame_buf_id[i]);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to associate image buffer to IDS driver,error code is %d\n",is_err);
            return is_err;
        }

        is_err = is_AddToSequence(cameraConfig.camera_handle, cameraConfig.frame_buf[i], cameraConfig.frame_buf_id[i]);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to add sequence image buffer,error code is %d\n",is_err);
            return is_err;
        }
    }

    // Initialize the image queue
    is_err = is_ImageQueue(cameraConfig.camera_handle, IS_IMAGE_QUEUE_CMD_INIT, NULL, 0);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to initialize image queue,error code is %d\n",is_err);
        return is_err;
    }

    // Flush image queue
    is_err = is_ImageQueue(cameraConfig.camera_handle, IS_IMAGE_QUEUE_CMD_FLUSH, NULL, 0);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to flush image queue,error code is %d\n",is_err);
        return is_err;
    }

    // Synchronize internal settings for buffer step size and overall buffer size
    // NOTE: assume that sensor_scaling_rate, subsampling_rate, and cam_binning_rate_
    //       have all been previously validated and synchronized by syncCamConfig()
    is_err = is_GetImageMemPitch(cameraConfig.camera_handle, &cam_buffer_pitch);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query buffer step size / pitch / stride,error code is %d\n",is_err);
        return is_err;
    }

    if(cam_buffer_pitch < frame_width)
    {
        fprintf(stderr, "Frame buffer's queried step size is smaller than buffer's expected width\n");
        fprintf(stderr, "(THIS IS A CODING ERROR, PLEASE CONTACT PACKAGE AUTHOR)\n");
    }

    cameraConfig.frame_buf_size = cam_buffer_pitch * frame_height;

    if(cameraConfig.image_buf.image == NULL)
    {
        cameraConfig.image_buf.image = (char *)malloc(cameraConfig.frame_buf_size);
        if(cameraConfig.image_buf.image == NULL)
        {
            fprintf(stderr, "alloc image buffer failed 1\n");
            return IS_NO_SUCCESS;
        }

        cameraConfig.image_buf.size = cameraConfig.frame_buf_size;
    }
    else
    {
        fprintf(stderr, "alloc image buffer failed 2\n");
        return IS_NO_SUCCESS;
    }

    return is_err;
}

static int setColorMode(char *mode) 
{
    int is_err = IS_SUCCESS;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    // Stop capture to prevent access to memory buffer
    setStandbyMode();

    // Set to specified color mode
    if(strstr(mode,"rgb8") != NULL)
    {
        is_err = is_SetColorMode(cameraConfig.camera_handle, IS_CM_RGB8_PACKED);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set color mode to RGB8,error code is %d\n",is_err);
            return is_err;
        }
    }
    else if(strstr(mode,"bgr8") != NULL)
    {
        is_err = is_SetColorMode(cameraConfig.camera_handle, IS_CM_BGR8_PACKED);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set color mode to BGR8,error code is %d\n",is_err);
            return is_err;
        }
    }
    else if(strstr(mode,"bayer_rggb8") != NULL)
    {
        is_err = is_SetColorMode(cameraConfig.camera_handle, IS_CM_SENSOR_RAW8);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set color mode to BAYER_RGGB8,error code is %d\n",is_err);
            return is_err;
        }
    }
    else
    {   // Default to MONO8
        is_err = is_SetColorMode(cameraConfig.camera_handle, IS_CM_MONO8);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set color mode to MONO8,error code is %d\n",is_err);
            return is_err;
        }
    }

    fprintf(stderr, "Updated color mode to %s\n",mode);

    return is_err;
}

static int setResolution(unsigned short image_width, unsigned short image_height,short image_left, short image_top)
{
    int is_err = IS_SUCCESS;
    IS_RECT camera_aio;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    // Validate arguments
    CAP(image_width, UI3240_MIN_IMAGE_WIDTH, (int)cameraConfig.camera_sensor_info.nMaxWidth);
    CAP(image_height, UI3240_MIN_IMAGE_HEIGHT, (int)cameraConfig.camera_sensor_info.nMaxHeight);

    if(image_left >= 0 && (int)cameraConfig.camera_sensor_info.nMaxWidth - image_width - image_left < 0)
    {
        fprintf(stderr, "Cannot set AOI left index to %d with a frame width of %d and sensor max width of %d\n",
                image_left,image_width,cameraConfig.camera_sensor_info.nMaxWidth);

        image_left = -1;
    }

    if(image_top >= 0 && (int)cameraConfig.camera_sensor_info.nMaxHeight - image_height - image_top < 0)
    {
        fprintf(stderr, "Cannot set AOI top index to %d with a frame height of %d and sensor max height of %d\n",
                image_top,image_height,cameraConfig.camera_sensor_info.nMaxHeight);

        image_top = -1;
    }

    camera_aio.s32X = (image_left < 0) ? (cameraConfig.camera_sensor_info.nMaxWidth - image_width) / 2 : image_left;
    camera_aio.s32Y = (image_top < 0) ? (cameraConfig.camera_sensor_info.nMaxHeight - image_height) / 2 : image_top;
    camera_aio.s32Width = image_width;
    camera_aio.s32Height = image_height;

    cameraConfig.image_width = camera_aio.s32Width;
    cameraConfig.image_height = camera_aio.s32Height;
    cameraConfig.image_left = camera_aio.s32X;
    cameraConfig.image_top = camera_aio.s32Y;

    is_err = is_AOI(cameraConfig.camera_handle, IS_AOI_IMAGE_SET_AOI, &camera_aio, sizeof(camera_aio));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to set Area Of Interest (AOI),error code is %d\n",is_err);
        return is_err;
    }

    fprintf(stderr, "Updated Area Of Interest (AOI)\n");

    return is_err;
}

static int setSubsampling(int rate)
{
    int is_err = IS_SUCCESS;
    int rate_flag;
    int supportedRates;
    int currRate;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    // Stop capture to prevent access to memory buffer
    setStandbyMode();

    supportedRates = is_SetSubSampling(cameraConfig.camera_handle, IS_GET_SUPPORTED_SUBSAMPLING);
    switch(rate)
    {
        case 1:
            rate_flag = IS_SUBSAMPLING_DISABLE;
        break;

        case 2:
            rate_flag = IS_SUBSAMPLING_2X_VERTICAL;
        break;

        case 4:
            rate_flag = IS_SUBSAMPLING_4X_VERTICAL;
        break;

        case 8:
            rate_flag = IS_SUBSAMPLING_8X_VERTICAL;
        break;

        case 16:
            rate_flag = IS_SUBSAMPLING_16X_VERTICAL;
        break;

        default:
            rate = 1;
            rate_flag = IS_SUBSAMPLING_DISABLE;

            fprintf(stderr, "currently has unsupported this subsampling rate,resetting to 1X\n");
        break;
    }

    if((supportedRates & rate_flag) == rate_flag)
    {
        is_err = is_SetSubSampling(cameraConfig.camera_handle, rate_flag);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to set subsampling rate,error code is %d\n",is_err);
            return is_err;
        }
    }
    else
    {
        fprintf(stderr, "does not support requested sampling rate of %dX\n",rate);

        // Query current rate
        currRate = is_SetSubSampling(cameraConfig.camera_handle, IS_GET_SUBSAMPLING);
        if(currRate == IS_SUBSAMPLING_DISABLE)
        {
            rate = 1;
        }
        else if(currRate == IS_SUBSAMPLING_2X_VERTICAL)
        {
            rate = 2;
        }
        else if(currRate == IS_SUBSAMPLING_4X_VERTICAL)
        {
            rate = 4;
        }
        else if(currRate == IS_SUBSAMPLING_8X_VERTICAL)
        {
            rate = 8;
        }
        else if(currRate == IS_SUBSAMPLING_16X_VERTICAL)
        {
            rate = 16;
        }
        else
        {
            fprintf(stderr, "currently has an unsupported sampling rate %dX,resetting to 1X\n",currRate);

            is_err = is_SetBinning(cameraConfig.camera_handle, IS_SUBSAMPLING_DISABLE);
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "Failed to set subsampling rate to 1X,error code is %d\n",is_err);
                return is_err;
            }
        }

        return IS_SUCCESS;
    }

    fprintf(stderr, "Updated subsampling rate to %dX\n",rate);

    cameraConfig.subsampling = rate;

    return is_err;
}

static int setBinning(int rate)
{
    int is_err = IS_SUCCESS;
    int rate_flag;
    int supportedRates;
    int currRate;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    // Stop capture to prevent access to memory buffer
    setStandbyMode();

    supportedRates = is_SetBinning(cameraConfig.camera_handle, IS_GET_SUPPORTED_BINNING);
    switch(rate) 
    {
        case 1:
            rate_flag = IS_BINNING_DISABLE;
        break;

        case 2:
            rate_flag = IS_BINNING_2X_VERTICAL;
        break;

        case 4:
            rate_flag = IS_BINNING_4X_VERTICAL;
        break;

        case 8:
            rate_flag = IS_BINNING_8X_VERTICAL;
        break;

        case 16:
            rate_flag = IS_BINNING_16X_VERTICAL;
        break;

        default:
            rate = 1;
            rate_flag = IS_BINNING_DISABLE;

            fprintf(stderr, "currently has unsupported binning rate: %dX,resetting to 1X\n",rate);
        break;
    }

    if((supportedRates & rate_flag) == rate_flag)
    {
        is_err = is_SetBinning(cameraConfig.camera_handle, rate_flag);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set binning rate to %dX\n",rate);
            return is_err;
        }
    }
    else
    {
        fprintf(stderr, "does not support requested binning rate of %dX\n",rate);

        // Query current rate
        currRate = is_SetBinning(cameraConfig.camera_handle, IS_GET_BINNING);
        if(currRate == IS_BINNING_DISABLE)
        {
            rate = 1;
        }
        else if(currRate == IS_BINNING_2X_VERTICAL)
        {
            rate = 2;
        }
        else if(currRate == IS_BINNING_4X_VERTICAL)
        {
            rate = 4;
        }
        else if(currRate == IS_BINNING_8X_VERTICAL)
        {
            rate = 8;
        }
        else if(currRate == IS_BINNING_16X_VERTICAL)
        {
            rate = 16;
        }
        else
        {
            fprintf(stderr, "currently has an unsupported binning rate %dX,resetting to 1X\n",currRate);

            is_err = is_SetBinning(cameraConfig.camera_handle, IS_BINNING_DISABLE);
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "Failed to set binning rate to 1X,error code is %d\n",is_err);
                return is_err;
            }
        }

        return IS_SUCCESS;
    }

    fprintf(stderr, "Updated binning rate to %dX\n",rate);

    cameraConfig.binning = rate;

    return is_err;
}

static int setSensorScaling(double rate)
{
    int is_err = IS_SUCCESS;
    SENSORSCALERINFO sensorScalerInfo;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    // Stop capture to prevent access to memory buffer
    setStandbyMode();

    is_err = is_GetSensorScalerInfo(cameraConfig.camera_handle, &sensorScalerInfo, sizeof(sensorScalerInfo));
    if(is_err == IS_NOT_SUPPORTED)
    {
        rate = 1.0;
        cameraConfig.sensor_scaling = 1.0;

        fprintf(stderr, "does not support internal image scaling,error code is %d\n",is_err);

        return IS_SUCCESS;
    }
    else if(is_err != IS_SUCCESS)
    {
        rate = 1.0;
        cameraConfig.sensor_scaling = 1.0;

        fprintf(stderr, "Failed to obtain supported internal image scaling information,error code is %d\n",is_err);

        return is_err;
    }
    else
    {
        if(rate < sensorScalerInfo.dblMinFactor || rate > sensorScalerInfo.dblMaxFactor)
        {
            rate = sensorScalerInfo.dblCurrFactor;

            fprintf(stderr, "Requested internal image scaling rate of %f is not within supported range %f to %f\n",
                    rate,sensorScalerInfo.dblMinFactor,sensorScalerInfo.dblMaxFactor);

            return IS_SUCCESS;
        }
    }

    is_err = is_SetSensorScaler(cameraConfig.camera_handle, IS_ENABLE_SENSOR_SCALER, rate);
    if(is_err != IS_SUCCESS)
    {
        rate = 1.0;

        fprintf(stderr, "Failed to set internal image scaling rate to %f,resetting to 1X\n",rate);

        is_err = is_SetSensorScaler(cameraConfig.camera_handle, IS_ENABLE_SENSOR_SCALER, rate);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to set internal image scaling rate to 1X,error code is %d\n",is_err);
            return is_err;
        }
    }

    fprintf(stderr, "Updated internal image scaling rate to %fX\n",rate);

    cameraConfig.sensor_scaling = rate;

    return is_err;
}

static int setGain(bool auto_gain, int master_gain, int red_gain,int green_gain, int blue_gain, bool gain_boost)
{
    int is_err = IS_SUCCESS;
    double pval1 = 0;
    double pval2 = 0;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    // Validate arguments
    CAP(master_gain, 0, 100);
    CAP(red_gain, 0, 100);
    CAP(green_gain, 0, 100);
    CAP(blue_gain, 0, 100);

    if(auto_gain)
    {
        // Set auto gain
        pval1 = 1;

        is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_ENABLE_AUTO_SENSOR_GAIN,&pval1, &pval2);
        if(is_err != IS_SUCCESS)
        {
            if ((is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_ENABLE_AUTO_GAIN,&pval1, &pval2)) != IS_SUCCESS)
            {
                fprintf(stderr, "does not support auto gain mode 1,error code is %d\n",is_err);
                auto_gain = false;
            }
        }
    }
    else
    {
        // Disable auto gain
        is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_ENABLE_AUTO_SENSOR_GAIN,&pval1, &pval2);
        if(is_err != IS_SUCCESS)
        {
            is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_ENABLE_AUTO_GAIN,&pval1, &pval2);
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "does not support auto gain mode 2,error code is %d\n",is_err);
            }
        }

        // Set gain boost
        is_err = is_SetGainBoost(cameraConfig.camera_handle, IS_GET_SUPPORTED_GAINBOOST);
        if(is_err != IS_SET_GAINBOOST_ON)
        {
            gain_boost = false;
        }
        else
        {
            is_err = is_SetGainBoost(cameraConfig.camera_handle,(gain_boost) ? IS_SET_GAINBOOST_ON : IS_SET_GAINBOOST_OFF);
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "Failed to set gain boost,error code is %d\n",is_err);
            }
        }

        // Set manual gain parameters
        is_err = is_SetHardwareGain(cameraConfig.camera_handle,master_gain,red_gain,green_gain,blue_gain);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to set manual gains,error code is %d\n",is_err);
        }
    }

    cameraConfig.master_gain = master_gain;
    cameraConfig.red_gain = red_gain;
    cameraConfig.green_gain = green_gain;
    cameraConfig.blue_gain = blue_gain;
    cameraConfig.auto_gain = auto_gain;
    cameraConfig.gain_boost = gain_boost;

    if(auto_gain)
    {
        fprintf(stderr, "Updated gain to auto\n");
    }
    else
    {
        fprintf(stderr, "Updated gain to manual\n");
    }

    return is_err;
}

static int setExposure(bool auto_exposure, double exposure_ms)
{
    int is_err = IS_SUCCESS;
    int is_err1 = IS_SUCCESS;
    double minExposure, maxExposure;
    double pval1 = auto_exposure;
    double pval2 = 0;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER,&pval1, &pval2);
    if(is_err != IS_SUCCESS)
    {
        is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_ENABLE_AUTO_SHUTTER,&pval1, &pval2);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Auto exposure mode is not supported,error code is %d\n",is_err);
            auto_exposure = false;
        }
    }

    // Set manual exposure timing
    if(!auto_exposure)
    {
        // Make sure that user-requested exposure rate is achievable
        is_err  = is_Exposure(cameraConfig.camera_handle, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN,(void*)&minExposure, sizeof(minExposure));
        is_err1 = is_Exposure(cameraConfig.camera_handle, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX,(void*)&maxExposure, sizeof(maxExposure));
        if(is_err != IS_SUCCESS || is_err1 != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to query valid exposure range,error code is %d\n",is_err);
            return is_err;
        }

        CAP(exposure_ms, minExposure, maxExposure);

        // Update exposure
        is_err = is_Exposure(cameraConfig.camera_handle, IS_EXPOSURE_CMD_SET_EXPOSURE,(void*)&(exposure_ms), sizeof(exposure_ms));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to set exposure to %f ms\n",exposure_ms);
            return is_err;
        }
    }

    cameraConfig.auto_exposure = auto_exposure;
    cameraConfig.exposure = exposure_ms;

    fprintf(stderr, "Updated exposure success\n");

    return is_err;
}

static int setWhiteBalance(bool auto_white_balance, int red_offset,int blue_offset)
{
    int is_err = IS_SUCCESS;
    double pval1 = auto_white_balance;
    double pval2 = 0;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    CAP(red_offset, -50, 50);
    CAP(blue_offset, -50, 50);

    // TODO: 9 bug: enabling auto white balance does not seem to have an effect; in ueyedemo it seems to change R/G/B gains automatically
    is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_ENABLE_AUTO_SENSOR_WHITEBALANCE, &pval1, &pval2);
    if(is_err != IS_SUCCESS)
    {
        is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_AUTO_WB_ONCE,&pval1, &pval2);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Auto white balance mode is not supported,error code is %d\n",is_err);
            auto_white_balance = false;
        }
    }
    if(auto_white_balance)
    {
        pval1 = red_offset;
        pval2 = blue_offset;

        is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_AUTO_WB_OFFSET,&pval1, &pval2);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to set white balance red/blue offsets to %d/%d\n",red_offset,blue_offset);
        }
    }

    cameraConfig.auto_white_balance = auto_white_balance;
    cameraConfig.white_balance_red_offset = red_offset;
    cameraConfig.white_balance_blue_offset = blue_offset;

    fprintf(stderr, "Updated white balance success\n");

    return is_err;
}

static int setFrameRate(bool auto_frame_rate, double frame_rate_hz)
{
    int is_err = IS_SUCCESS;
    double pval1 = 0;
    double pval2 = 0;
    double minFrameTime;
    double maxFrameTime;
    double intervalFrameTime;
    double newFrameRate;
    bool autoShutterOn = false;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    // Make sure that auto shutter is enabled before enabling auto frame rate
    is_SetAutoParameter(cameraConfig.camera_handle, IS_GET_ENABLE_AUTO_SENSOR_SHUTTER, &pval1, &pval2);
    autoShutterOn |= (pval1 != 0);

    is_SetAutoParameter(cameraConfig.camera_handle, IS_GET_ENABLE_AUTO_SHUTTER, &pval1, &pval2);
    autoShutterOn |= (pval1 != 0);

    if(!autoShutterOn)
    {
        auto_frame_rate = false;
    }

    // Set frame rate / auto
    pval1 = auto_frame_rate;

    is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_ENABLE_AUTO_SENSOR_FRAMERATE,&pval1, &pval2);
    if(is_err != IS_SUCCESS)
    {
        is_err = is_SetAutoParameter(cameraConfig.camera_handle, IS_SET_ENABLE_AUTO_FRAMERATE,&pval1, &pval2);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Auto frame rate mode is not supported,error code is %d\n",is_err);
            auto_frame_rate = false;
        }
    }
    if(!auto_frame_rate)
    {
        // Make sure that user-requested frame rate is achievable
        is_err = is_GetFrameTimeRange(cameraConfig.camera_handle, &minFrameTime,&maxFrameTime, &intervalFrameTime);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to query valid frame rate range,error code is %d\n",is_err);
            return is_err;
        }

        CAP(frame_rate_hz, 1.0/maxFrameTime, 1.0/minFrameTime);

        // Update frame rate
        is_err = is_SetFrameRate(cameraConfig.camera_handle, frame_rate_hz, &newFrameRate);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to set frame rate to %lfHz\n",frame_rate_hz);
            return is_err;
        }
        else if(frame_rate_hz != newFrameRate)
        {
            frame_rate_hz = newFrameRate;
        }
    }

    cameraConfig.auto_frame_rate = auto_frame_rate;
    cameraConfig.frame_rate = frame_rate_hz;

    fprintf(stderr, "Updated frame rate success\n");

    return is_err;
}

static int setPixelClockRate(int clock_rate_mhz)
{
    int is_err = IS_SUCCESS;
    unsigned int pixelClockList[150];  // No camera has more than 150 different pixel clocks (uEye manual)
    unsigned int numberOfSupportedPixelClocks = 0;
    int minPixelClock = 0;
    int maxPixelClock = 0;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    is_err = is_PixelClock(cameraConfig.camera_handle, IS_PIXELCLOCK_CMD_GET_NUMBER,
                          (void*)&numberOfSupportedPixelClocks,sizeof(numberOfSupportedPixelClocks));
    if(is_err!= IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query number of supported pixel clocks,error code is %d\n",is_err);
        return is_err;
    }

    if(numberOfSupportedPixelClocks > 0)
    {
        ZeroMemory(pixelClockList, sizeof(pixelClockList));

        is_err = is_PixelClock(cameraConfig.camera_handle, IS_PIXELCLOCK_CMD_GET_LIST,
                              (void*)pixelClockList, numberOfSupportedPixelClocks * sizeof(int));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to query list of supported pixel clocks,error code is %d\n",is_err);
            return is_err;
        }
    }
    
    minPixelClock = (int)pixelClockList[0];
    maxPixelClock = (int)pixelClockList[numberOfSupportedPixelClocks - 1];

    CAP(clock_rate_mhz, minPixelClock, maxPixelClock);

    // As list is sorted smallest to largest...
    for(UINT i = 0; i < numberOfSupportedPixelClocks; i ++)
    {
        if(clock_rate_mhz <= (int) pixelClockList[i])
        {
            clock_rate_mhz = pixelClockList[i];  // ...get the closest-larger-or-equal from the list
            break;
        }
    }

    is_err = is_PixelClock(cameraConfig.camera_handle, IS_PIXELCLOCK_CMD_SET,(void*)&(clock_rate_mhz), sizeof(clock_rate_mhz));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to set pixel clock to %dMHz\n",clock_rate_mhz);
        return is_err;
    }

    cameraConfig.pixel_clock = clock_rate_mhz;

    fprintf(stderr, "Updated pixel clock success\n");

    return IS_SUCCESS;
}

static int setFlashParams(int delay_us, unsigned int duration_us)
{
    int is_err = IS_SUCCESS;
    // Make sure parameters are within range supported by camera
    IO_FLASH_PARAMS minFlashParams;
    IO_FLASH_PARAMS maxFlashParams;
    IO_FLASH_PARAMS newFlashParams;

    is_err = is_IO(cameraConfig.camera_handle, IS_IO_CMD_FLASH_GET_PARAMS_MIN,(void*)&minFlashParams, sizeof(IO_FLASH_PARAMS));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not retrieve flash parameter info (min),error code is %d\n",is_err);
        return is_err;
    }

    is_err = is_IO(cameraConfig.camera_handle, IS_IO_CMD_FLASH_GET_PARAMS_MAX,(void*)&maxFlashParams, sizeof(IO_FLASH_PARAMS));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not retrieve flash parameter info (max),error code is %d\n",is_err);
        return is_err;
    }

    delay_us = (delay_us < minFlashParams.s32Delay) ? minFlashParams.s32Delay : 
               ((delay_us > maxFlashParams.s32Delay) ? maxFlashParams.s32Delay : delay_us);
    duration_us = (duration_us < minFlashParams.u32Duration && duration_us != 0) ? 
                  minFlashParams.u32Duration : ((duration_us > maxFlashParams.u32Duration) ? 
                  maxFlashParams.u32Duration : duration_us);

    newFlashParams.s32Delay = delay_us;
    newFlashParams.u32Duration = duration_us;

    // WARNING: Setting s32Duration to 0, according to documentation, means
    //          setting duration to total exposure time. If non-ext-triggered
    //          camera is operating at fastest grab rate, then the resulting
    //          flash signal will APPEAR as active LO when set to active HIGH,
    //          and vice versa. This is why the duration is set manually.
    is_err = is_IO(cameraConfig.camera_handle, IS_IO_CMD_FLASH_SET_PARAMS,(void*)&newFlashParams, sizeof(IO_FLASH_PARAMS));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not set flash parameter info,error code is %d\n",is_err);
        return is_err;
    }

    cameraConfig.flash_delay = delay_us;
    cameraConfig.flash_duration = duration_us;

    return is_err;
}

static int setMirrorUpsideDown(bool flip_horizontal)
{
    int is_err = IS_SUCCESS;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    if(flip_horizontal)
    {
        is_err = is_SetRopEffect(cameraConfig.camera_handle,IS_SET_ROP_MIRROR_UPDOWN,1,0);
    }
    else
    {
        is_err = is_SetRopEffect(cameraConfig.camera_handle,IS_SET_ROP_MIRROR_UPDOWN,0,0);
    }

    cameraConfig.flip_upd = flip_horizontal;

    return is_err;
}

static int setMirrorLeftRight(bool flip_vertical)
{
    int is_err = IS_SUCCESS;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    if(flip_vertical)
    {
        is_err = is_SetRopEffect(cameraConfig.camera_handle,IS_SET_ROP_MIRROR_LEFTRIGHT,1,0);
    }
    else
    {
        is_err = is_SetRopEffect(cameraConfig.camera_handle,IS_SET_ROP_MIRROR_LEFTRIGHT,0,0);
    }

    cameraConfig.flip_lr = flip_vertical;

    return is_err;
}

static int setCameraUserConfig(void)
{
    int is_err = IS_SUCCESS;

    if(cameraConfig.master != userUi3240Config.master)
    {
        cameraConfig.master = userUi3240Config.master;
    }

    if(cameraConfig.stereo != userUi3240Config.stereo)
    {
        cameraConfig.stereo = userUi3240Config.stereo;
    }

    if(cameraConfig.capture_mode != userUi3240Config.capture_mode)
    {
        cameraConfig.capture_mode = userUi3240Config.capture_mode;
    }

    if(cameraConfig.capture_timeout != userUi3240Config.capture_timeout)
    {
        cameraConfig.capture_timeout = userUi3240Config.capture_timeout;
    }

    if(cameraConfig.image_width != userUi3240Config.image_width || 
       cameraConfig.image_height != userUi3240Config.image_height || 
       cameraConfig.image_left != userUi3240Config.image_left || 
       cameraConfig.image_top != userUi3240Config.image_top)
    {
        is_err = setResolution(userUi3240Config.image_width,userUi3240Config.image_height,
                               userUi3240Config.image_left,userUi3240Config.image_top);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Failed to set user resolution,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.color_mode != userUi3240Config.color_mode)
    {
        cameraConfig.color_mode = userUi3240Config.color_mode;

        is_err = is_SetColorMode(userUi3240Config.camera_handle, userUi3240Config.color_mode);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user color mode,error code is %d\n",is_err);
        }

        if(userUi3240Config.color_mode == IS_CM_BGR8_PACKED || userUi3240Config.color_mode == IS_CM_RGB8_PACKED)
        {
            cameraConfig.bits_per_pixel = 24;
        }
        else if(userUi3240Config.color_mode == IS_CM_MONO8 || userUi3240Config.color_mode == IS_CM_SENSOR_RAW8)
        {
            cameraConfig.bits_per_pixel = 8;
        }
    }

    if(cameraConfig.subsampling != userUi3240Config.subsampling)
    {
        is_err = setSubsampling(userUi3240Config.subsampling);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user subsampling,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.binning != userUi3240Config.binning)
    {
        is_err = setBinning(userUi3240Config.binning);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user binning,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.sensor_scaling != userUi3240Config.sensor_scaling)
    {
        is_err = setSensorScaling(userUi3240Config.sensor_scaling);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user scaling,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.auto_gain != userUi3240Config.auto_gain || 
       cameraConfig.master_gain != userUi3240Config.master_gain || 
       cameraConfig.red_gain != userUi3240Config.red_gain || 
       cameraConfig.green_gain != userUi3240Config.green_gain || 
       cameraConfig.blue_gain != userUi3240Config.blue_gain || 
       cameraConfig.gain_boost != userUi3240Config.gain_boost)
    {
        is_err = setGain(userUi3240Config.auto_gain,userUi3240Config.master_gain,userUi3240Config.red_gain,
                         userUi3240Config.green_gain,userUi3240Config.blue_gain,userUi3240Config.gain_boost);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user gain,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.auto_exposure != userUi3240Config.auto_exposure ||  
       cameraConfig.exposure != userUi3240Config.exposure)
    {
        is_err = setExposure(userUi3240Config.auto_exposure,userUi3240Config.exposure);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user exposure,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.auto_white_balance != userUi3240Config.auto_white_balance || 
       cameraConfig.white_balance_red_offset != userUi3240Config.white_balance_red_offset ||  
       cameraConfig.white_balance_blue_offset != userUi3240Config.white_balance_blue_offset)
    {
        is_err = setWhiteBalance(userUi3240Config.auto_white_balance,
                                 userUi3240Config.white_balance_red_offset,
                                userUi3240Config.white_balance_blue_offset);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user white_balance,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.auto_frame_rate != userUi3240Config.auto_frame_rate ||  
       cameraConfig.frame_rate != userUi3240Config.frame_rate)
    {
        is_err = setFrameRate(userUi3240Config.auto_frame_rate,userUi3240Config.frame_rate);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user frame_rate,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.pixel_clock != userUi3240Config.pixel_clock)
    {
        is_err = setPixelClockRate(userUi3240Config.pixel_clock);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user pixel_clock,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.ext_trigger_mode != userUi3240Config.ext_trigger_mode)
    {
        cameraConfig.ext_trigger_mode = userUi3240Config.ext_trigger_mode;
    }

    if(cameraConfig.ext_trigger_delay != userUi3240Config.ext_trigger_delay)
    {
        cameraConfig.ext_trigger_delay = userUi3240Config.ext_trigger_delay;
    }

    if(cameraConfig.flash_delay != userUi3240Config.flash_delay ||  
       cameraConfig.flash_duration != userUi3240Config.flash_duration)
    {
        is_err = setFlashParams(userUi3240Config.flash_delay,userUi3240Config.flash_duration);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user flash params,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.flip_upd != userUi3240Config.flip_upd)
    {
        is_err = setMirrorUpsideDown(userUi3240Config.flip_upd);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user image up down,error code is %d\n",is_err);
        }
    }

    if(cameraConfig.flip_lr != userUi3240Config.flip_lr)
    {
        is_err = setMirrorLeftRight(userUi3240Config.flip_lr);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set user image left right,error code is %d\n",is_err);
        }
    }

    return IS_SUCCESS;
}

static int setFreeRunMode(void)
{
    int is_err = IS_SUCCESS;
    int flash_delay = 0;
    unsigned int flash_duration = 1000;
    unsigned int nMode = IO_FLASH_MODE_FREERUN_HI_ACTIVE;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

//    if(!freeRunModeActive())
//    {
        setStandbyMode(); // No need to check for success

        // Set the flash to a high active pulse for each image in the trigger mode
        setFlashParams(flash_delay, flash_duration);

        is_err = is_IO(cameraConfig.camera_handle, IS_IO_CMD_FLASH_SET_MODE,(void*)&nMode, sizeof(nMode));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set free-run active-low flash output,error code is %d\n",is_err);
            return is_err;
        }
/*
        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_ENABLE, IS_SET_EVENT_FRAME, sizeof(unsigned int));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not enable frame event,error code is %d\n",is_err);
            return is_err;
        }
*/
        is_err = is_CaptureVideo(cameraConfig.camera_handle, IS_WAIT);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not start free-run live video mode,error code is %d\n",is_err);
            return is_err;
        }

        fprintf(stderr, "Started live video mode\n");
//    }

    return is_err;
}

static int setExtTriggerMode(double frame_rate, int trigger_delay, bool master)
{
    int is_err = IS_SUCCESS;
    int min_delay;
    int max_delay;
    int current_delay;
    unsigned int nMode = IO_FLASH_MODE_TRIGGER_HI_ACTIVE;

    if(!isConnected())
    {
        return IS_INVALID_CAMERA_HANDLE;
    }

    if(!extTriggerModeActive())
    {
        setStandbyMode(); // No need to check for success
/*
        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_ENABLE, IS_SET_EVENT_FRAME, sizeof(unsigned int));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not enable frame event,error code is %d\n",is_err);
            return is_err;
        }
*/
        /* If "master" set the GPIO2 to generate PWM */
        if(master)
        {
            fprintf(stderr, "GPIO2 configured as output (PWM) at %lf Hz\n",frame_rate);

            is_err = gpioPwmConfig(cameraConfig.camera_handle, frame_rate, true);
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "Could not set GPIO 2 as output (PWM),error code is %d\n",is_err);
                return is_err;
            }
        }

        /* set global shutter mode */
        /*
        int nShutterMode = IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL;
        is_err = is_DeviceFeature(cameraConfig.camera_handle, IS_DEVICE_FEATURE_CMD_SET_SHUTTER_MODE,
                                 (void *)&nShutterMode, sizeof(nShutterMode));
        if(is_err == IS_SUCCESS)
        {
            fprintf(stderr, "Global shutter ok,error code is %d\n",is_err);
        }
        */

        /* Set GPIO1 as input for trigger */
        is_err = gpioInputConfig(cameraConfig.camera_handle);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set GPIO1 as input,error code is %d\n",is_err);
            return is_err;
        }

        fprintf(stderr, "GPIO1 configured as input for triggering\n");
        fprintf(stderr, "GPIO2 configured as output for flash\n");

        /* Set to trigger on falling edge */
        is_err = is_SetExternalTrigger(cameraConfig.camera_handle, IS_SET_TRIGGER_HI_LO);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not enable falling-edge external trigger mode,error code is %d\n",is_err);
            return is_err;
        }

        /* Set trigger delay */
        min_delay = is_SetTriggerDelay(cameraConfig.camera_handle, IS_GET_MIN_TRIGGER_DELAY);
        max_delay = is_SetTriggerDelay(cameraConfig.camera_handle, IS_GET_MAX_TRIGGER_DELAY);

        is_err = is_SetTriggerDelay(cameraConfig.camera_handle, (int)trigger_delay);
        if(is_err != IS_SUCCESS && (trigger_delay >= min_delay && trigger_delay <= max_delay))
        {
            fprintf(stderr, "Min delay: %dus; Max delay: %dus\n",min_delay,max_delay);
            fprintf(stderr, "Could not set trigger-delay,error code is %d\n",is_err);
            return is_err;
        }

        current_delay = is_SetTriggerDelay(cameraConfig.camera_handle, IS_GET_TRIGGER_DELAY);
        fprintf(stderr, "external trigger delay: %dus\n",current_delay);

        // high level in trigger mode
        is_err = is_IO(cameraConfig.camera_handle, IS_IO_CMD_FLASH_SET_MODE, (void *)&nMode, sizeof(nMode));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "set trigger flash mode failed,error code is %d\n",is_err);
        } 

        // // get the flash Mode to confirm
        is_err = is_IO(cameraConfig.camera_handle, IS_IO_CMD_FLASH_GET_MODE, (void *)&nMode, sizeof(nMode));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "set flash mode failed,error code is %d\n",is_err);
        }

        // start video capture
        is_err = is_CaptureVideo(cameraConfig.camera_handle, IS_DONT_WAIT);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not start external trigger live video mode,error code is %d\n",is_err);
            return is_err;
        }

        fprintf(stderr, "Started falling-edge external trigger live video mode\n");
    }

    return is_err;
}

static int setTriggerMode(void)
{
    int is_err = IS_SUCCESS;
    int is_err1 = IS_SUCCESS;

    if(cameraConfig.ext_trigger_mode) 
    {
        fprintf(stderr, "Setup external trigger mode...\n");

        is_err = setExtTriggerMode(cameraConfig.frame_rate, cameraConfig.ext_trigger_delay, cameraConfig.master);
        if(is_err != IS_SUCCESS) 
        {
            fprintf(stderr, "Setup external trigger mode failed,error code is %d\n",is_err);
            return is_err;
        }

        fprintf(stderr, "Setup external trigger mode success\n");
    } 
    else 
    {
        fprintf(stderr, "Setup freerun mode...\n");
        // NOTE: need to copy flash parameters to local copies since
        //       cameraConfig.flash_duration is type int, and also sizeof(int)
        //       may not equal to sizeof(INT) / sizeof(UINT)
        
        is_err  = setFreeRunMode();
        is_err1 = setFlashParams(cameraConfig.flash_delay, cameraConfig.flash_duration);
        if(is_err != IS_SUCCESS || is_err1 != IS_SUCCESS)
        {
            fprintf(stderr, "Setup freerun mode failed,error code is %d,%d\n",is_err,is_err1);
            return is_err;
        }
        
        fprintf(stderr, "Setup freerun mode success\n");
    }

    return is_err;
}

static int queryCameraConfig(void)
{
    int is_err = IS_SUCCESS;
    int is_err1 = IS_SUCCESS;
    IS_RECT camera_aio;
    SENSORSCALERINFO sensor_scaler_info;
    int curr_subsampling_rate;
    int curr_binning_rate;
    int query;
    double pval1;
    double pval2;
    IO_FLASH_PARAMS currFlashParams;
    unsigned int currPixelClock;
    int currROP;

    // Synchronize resolution, color mode, bits per pixel settings
    is_err = is_AOI(cameraConfig.camera_handle, IS_AOI_IMAGE_GET_AOI,(void*)&camera_aio, sizeof(camera_aio));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not retrieve Area Of Interest (AOI) information,error code is %d\n",is_err);
        return is_err;
    }
    cameraConfig.image_width = camera_aio.s32Width;
    cameraConfig.image_height = camera_aio.s32Height;
    cameraConfig.image_left = camera_aio.s32X;
    cameraConfig.image_top = camera_aio.s32Y;

    cameraConfig.color_mode = is_SetColorMode(cameraConfig.camera_handle, IS_GET_COLOR_MODE);
    if(cameraConfig.color_mode == IS_CM_BGR8_PACKED || cameraConfig.color_mode == IS_CM_RGB8_PACKED)
    {
        cameraConfig.bits_per_pixel = 24;
    }
    else if(cameraConfig.color_mode == IS_CM_MONO8 || cameraConfig.color_mode == IS_CM_SENSOR_RAW8)
    {
        cameraConfig.bits_per_pixel = 8;
    }
    else
    {
        fprintf(stderr, "Current color mode is not supported by this wrapper{mono8 | bayer_rggb8 | rgb8 | bgr8}\n");

        is_err = setColorMode(UI3240_DEFAULT_CLOLOR_MODE_STRING); 
        if(is_err != IS_SUCCESS)
        {
            return is_err;
        }
   
        cameraConfig.color_mode = is_SetColorMode(cameraConfig.camera_handle, IS_GET_COLOR_MODE);
        cameraConfig.bits_per_pixel = 8;
    }

    // Synchronize sensor scaling rate setting
    is_err = is_GetSensorScalerInfo(cameraConfig.camera_handle, &sensor_scaler_info, sizeof(sensor_scaler_info));
    if(is_err == IS_NOT_SUPPORTED)
    {
        cameraConfig.sensor_scaling = 1.0;
    }
    else if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not obtain supported internal image scaling information,error code is %d\n",is_err);
        return is_err;
    }
    else
    {
        cameraConfig.sensor_scaling = sensor_scaler_info.dblCurrFactor;
    }

    // Synchronize subsampling rate setting
    curr_subsampling_rate = is_SetSubSampling(cameraConfig.camera_handle, IS_GET_SUBSAMPLING);
    if(curr_subsampling_rate == IS_SUBSAMPLING_DISABLE)
    {
        cameraConfig.subsampling = 1;
    }
    else if(curr_subsampling_rate == IS_SUBSAMPLING_2X_VERTICAL)
    {
        cameraConfig.subsampling = 2;
    }
    else if(curr_subsampling_rate == IS_SUBSAMPLING_4X_VERTICAL)
    {
        cameraConfig.subsampling = 4;
    }
    else if(curr_subsampling_rate == IS_SUBSAMPLING_8X_VERTICAL)
    {
        cameraConfig.subsampling = 8;
    }
    else if(curr_subsampling_rate == IS_SUBSAMPLING_16X_VERTICAL)
    {
        cameraConfig.subsampling = 16;
    }
    else
    {
        fprintf(stderr, "Current sampling rate is not supported by this wrapper; resetting to 1X\n");

        is_err = is_SetSubSampling(cameraConfig.camera_handle, IS_SUBSAMPLING_DISABLE);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set subsampling rate to 1X,error code is %d\n",is_err);
            return is_err;
        }

        cameraConfig.subsampling = 1;
    }

    // Synchronize binning rate setting
    curr_binning_rate = is_SetBinning(cameraConfig.camera_handle, IS_GET_BINNING);
    if(curr_binning_rate == IS_BINNING_DISABLE)
    {
        cameraConfig.binning = 1;
    }
    else if(curr_binning_rate == IS_BINNING_2X_VERTICAL)
    {
        cameraConfig.binning = 2;
    }
    else if(curr_binning_rate == IS_BINNING_4X_VERTICAL)
    {
        cameraConfig.binning = 4;
    }
    else if(curr_binning_rate == IS_BINNING_8X_VERTICAL)
    {
        cameraConfig.binning = 8;
    }
    else if(curr_binning_rate == IS_BINNING_16X_VERTICAL)
    {
        cameraConfig.binning = 16;
    }
    else
    {
        fprintf(stderr, "Current binning rate is not supported by this wrapper; resetting to 1X\n");

        is_err = is_SetBinning(cameraConfig.camera_handle, IS_BINNING_DISABLE);

        if (is_err != IS_SUCCESS)
        {
            fprintf(stderr, "Could not set binning rate to 1X,error code is %d\n",is_err);
            
        }

        cameraConfig.binning = 1;
    }   

    is_err  = is_SetAutoParameter(cameraConfig.camera_handle,IS_GET_ENABLE_AUTO_SENSOR_GAIN, &pval1, &pval2);
    is_err1 = is_SetAutoParameter(cameraConfig.camera_handle,IS_GET_ENABLE_AUTO_GAIN, &pval1, &pval2);
    if(is_err != IS_SUCCESS && is_err1 != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query auto gain mode,error code is %d\n",is_err);
        return is_err;
    }
    
    cameraConfig.auto_gain = (pval1 != 0);

    cameraConfig.master_gain = is_SetHardwareGain(cameraConfig.camera_handle, IS_GET_MASTER_GAIN,IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
    cameraConfig.red_gain = is_SetHardwareGain(cameraConfig.camera_handle, IS_GET_RED_GAIN,IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
    cameraConfig.green_gain = is_SetHardwareGain(cameraConfig.camera_handle, IS_GET_GREEN_GAIN,IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
    cameraConfig.blue_gain = is_SetHardwareGain(cameraConfig.camera_handle, IS_GET_BLUE_GAIN,IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);

    query = is_SetGainBoost(cameraConfig.camera_handle, IS_GET_SUPPORTED_GAINBOOST);
    if(query == IS_SET_GAINBOOST_ON)
    {
        query = is_SetGainBoost(cameraConfig.camera_handle, IS_GET_GAINBOOST);
        if(query == IS_SET_GAINBOOST_ON)
        {
            cameraConfig.gain_boost = true;
        }
        else if(query == IS_SET_GAINBOOST_OFF)
        {
            cameraConfig.gain_boost = false;
        }
        else
        {
            fprintf(stderr, "Failed to query gain boost,error code is %d\n",is_err);
            return query;
        }
    }
    else
    {
        cameraConfig.gain_boost = false;
    }

    is_err  = is_SetAutoParameter(cameraConfig.camera_handle,IS_GET_ENABLE_AUTO_SENSOR_SHUTTER, &pval1, &pval2);
    is_err1 = is_SetAutoParameter(cameraConfig.camera_handle,IS_GET_ENABLE_AUTO_SHUTTER, &pval1, &pval2);
    if(is_err != IS_SUCCESS && is_err1 != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query auto shutter mode,error code is %d\n",is_err);
        return is_err;
    }

    cameraConfig.auto_exposure = (pval1 != 0);

    is_err = is_Exposure(cameraConfig.camera_handle, IS_EXPOSURE_CMD_GET_EXPOSURE,&cameraConfig.exposure,sizeof(cameraConfig.exposure));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query exposure timing,error code is %d\n",is_err);
        return is_err;
    }

    is_err = is_SetAutoParameter(cameraConfig.camera_handle,IS_GET_ENABLE_AUTO_SENSOR_WHITEBALANCE, &pval1, &pval2);
    is_err = is_SetAutoParameter(cameraConfig.camera_handle,IS_GET_ENABLE_AUTO_WHITEBALANCE, &pval1, &pval2);
    if(is_err != IS_SUCCESS && is_err1 != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query auto white balance mode,error code is %d,%d\n",is_err,is_err1);
        return is_err;
    }

    cameraConfig.auto_white_balance = (pval1 != 0);

    is_err = is_SetAutoParameter(cameraConfig.camera_handle,IS_GET_AUTO_WB_OFFSET, &pval1, &pval2);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query auto white balance red/blue channel offsets,error code is %d\n",is_err);
        return is_err;
    }

    cameraConfig.white_balance_red_offset = pval1;
    cameraConfig.white_balance_blue_offset = pval2;

    is_err = is_IO(cameraConfig.camera_handle, IS_IO_CMD_FLASH_GET_PARAMS,(void*)&currFlashParams, sizeof(IO_FLASH_PARAMS));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Could not retrieve current flash parameter info,error code is %d\n",is_err);
        return is_err;
    }

    cameraConfig.flash_delay = currFlashParams.s32Delay;
    cameraConfig.flash_duration = currFlashParams.u32Duration;

    is_err  = is_SetAutoParameter(cameraConfig.camera_handle,IS_GET_ENABLE_AUTO_SENSOR_FRAMERATE, &pval1, &pval2);
    is_err1 = is_SetAutoParameter(cameraConfig.camera_handle,IS_GET_ENABLE_AUTO_FRAMERATE, &pval1, &pval2);
    if(is_err != IS_SUCCESS && is_err1 != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query auto frame rate mode,error code is %d\n",is_err);
        return is_err;
    }

    cameraConfig.auto_frame_rate = (pval1 != 0);

    is_err = is_SetFrameRate(cameraConfig.camera_handle, IS_GET_FRAMERATE, &cameraConfig.frame_rate);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query frame rate,error code is %d\n",is_err);
        return is_err;
    }

    is_err = is_PixelClock(cameraConfig.camera_handle, IS_PIXELCLOCK_CMD_GET,(void*)&currPixelClock, sizeof(currPixelClock));
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to query pixel clock rate,error code is %d\n",is_err);
        return is_err;
    }

    cameraConfig.pixel_clock = currPixelClock;

    currROP = is_SetRopEffect(cameraConfig.camera_handle, IS_GET_ROP_EFFECT, 0, 0);
    cameraConfig.flip_upd = ((currROP & IS_SET_ROP_MIRROR_UPDOWN) == IS_SET_ROP_MIRROR_UPDOWN);
    cameraConfig.flip_lr = ((currROP & IS_SET_ROP_MIRROR_LEFTRIGHT) == IS_SET_ROP_MIRROR_LEFTRIGHT);

    // NOTE: do not need to (re-)populate ROS image message, since assume that
    //       syncCamConfig() was previously called

    fprintf(stderr, "Successfully queries parameters from UEye camera\n");

    return is_err;
}

static int getTimestamp(UEYETIME *timestamp,int cam_buffer_id)
{
    int is_err = IS_SUCCESS;
    UEYEIMAGEINFO ImageInfo;

    is_err = is_GetImageInfo (cameraConfig.camera_handle,cam_buffer_id, &ImageInfo, sizeof(ImageInfo));
    if(is_err == IS_SUCCESS)
    {
        *timestamp = ImageInfo.TimestampSystem;

        return is_err;
    }

    return IS_NO_SUCCESS;
}

char *captureImage(unsigned char capture_mode, unsigned short timeout_ms)
{
    int is_err = IS_SUCCESS;
    static unsigned char mode = 255;
    char *memory = NULL;
    int id;
    UEYETIME time_stamp;
    IMAGEQUEUEWAITBUFFER waitBuffer;
    waitBuffer.timeout = timeout_ms;
    waitBuffer.pnMemId= &id;
    waitBuffer.ppcMem = &memory;
    unsigned int event = IS_SET_EVENT_FRAME;
    IS_INIT_EVENT init_event = {IS_SET_EVENT_FRAME, TRUE, FALSE};
    IS_WAIT_EVENT wait_event = {IS_SET_EVENT_FRAME, 1000, 0, 0};

    if(!isCapturing())
    {
        fprintf(stderr, "!isCapturing()\n");
        return NULL;
    }

    if(!freeRunModeActive() && !extTriggerModeActive())
    {
        fprintf(stderr, "!freeRunModeActive() && !extTriggerModeActive()\n");
        return NULL;
    }

    if(mode != capture_mode)
    {
        mode = capture_mode;

        if(capture_mode == 0)
        {
            is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_INIT, &init_event, sizeof(IS_INIT_EVENT));
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "Could not init frame event 3,error code is %d\n",is_err);
            }

            is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_DISABLE, &event, sizeof(unsigned int));
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "Could not disable frame event 3,error code is %d\n",is_err);
            }

            is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_EXIT, &event, sizeof(unsigned int));
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "Could not exit frame event,error code is %d\n",is_err);
            }
        }
        else
        {
            wait_event.nTimeoutMilliseconds = timeout_ms;

            is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_INIT, &init_event, sizeof(IS_INIT_EVENT));
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "Could not init frame event 2,error code is %d\n",is_err);
            }

            is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_ENABLE, &event, sizeof(unsigned int));
            if(is_err != IS_SUCCESS)
            {
                fprintf(stderr, "Could not enable frame event,error code is %d\n",is_err);
            }
        }
    }

    if(capture_mode == 0)
    {
        is_err = is_ImageQueue(cameraConfig.camera_handle, IS_IMAGE_QUEUE_CMD_WAIT, (void*)&waitBuffer, sizeof(waitBuffer));
        if(is_err != IS_SUCCESS)
        {
            if(is_err == IS_TIMED_OUT)
            {
                fprintf(stderr, "capture image from queue timeout\n");
            }
            else
            {
                fprintf(stderr, "Failed to capture from queue image,error code is %d\n",is_err);
            }

            return NULL;
        }

        // copy the buffer
//        memcpy(cameraConfig.image_buf.image, memory, cameraConfig.frame_buf_size);

        is_err = is_CopyImageMem(cameraConfig.camera_handle,memory,id,cameraConfig.image_buf.image);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "copy image buffer failed,error code is %d\n",is_err);
            return NULL;
        }

        is_err = getTimestamp(&time_stamp,id);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "query image time stamp failed,error code is %d\n",is_err);
            return NULL;
        }

        printf("============================= time stamp start==============================\n");
        printf("| year        : %d\n",time_stamp.wYear);
        printf("| month       : %d\n",time_stamp.wMonth);
        printf("| day         : %d\n",time_stamp.wDay);
        printf("| hour        : %d\n",time_stamp.wHour);
        printf("| minute      : %d\n",time_stamp.wMinute);
        printf("| second      : %d\n",time_stamp.wSecond);
        printf("| willi second: %d\n",time_stamp.wMilliseconds);
        printf("============================= time stamp end ===============================\n");
    }
    else
    {
        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_WAIT, &wait_event, sizeof(wait_event));
        if(is_err != IS_SUCCESS)
        {
            if(is_err == IS_TIMED_OUT)
            {
                fprintf(stderr, "capture image from event timeout\n");
            }
            else
            {
                fprintf(stderr, "Failed to capture from event image,error code is %d\n",is_err);
            }

            return NULL;
        }

        is_err = is_Event(cameraConfig.camera_handle, IS_EVENT_CMD_RESET, &event, sizeof(unsigned int));
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "reset event image failed,error code is %d\n",is_err);
            return NULL;
        }

        is_err = is_GetImageMem(cameraConfig.camera_handle, (void **)&memory);
        if(is_err != IS_SUCCESS)
        {
            fprintf(stderr, "get image menory failed,error code is %d\n",is_err);
            return NULL;
        }

        memcpy(cameraConfig.image_buf.image, memory, cameraConfig.frame_buf_size);
    }

    // Unlock the buffer which has been automatically locked by is_WaitForNextImage()
    is_err = is_UnlockSeqBuf(cameraConfig.camera_handle, IS_IGNORE_PARAMETER, memory);
    if(is_err != IS_SUCCESS)
    {
        fprintf(stderr, "Failed to unlock image buffer,error code is %d\n",is_err);
    }

    fprintf(stderr, "Get frame from driver OK\n");

    return cameraConfig.image_buf.image;
}

static void initCameraConfig(struct Ui3240Config *config,struct CmdArgs *args)
{
    if(config == NULL || args == NULL)
    {
        fprintf(stderr, "input paras is NULL\n");
        return;
    }

    memset(config,0,sizeof(struct Ui3240Config));

    config->capture_mode                = args->capture_mode;
    config->capture_timeout             = args->capture_timeout;
    config->image_width                 = 1280;
    config->image_height                = 512;
    config->image_left                  = 0;
    config->image_top                   = 256;
    config->frame_num                   = args->frame_num;
    config->color_mode                  = IS_CM_MONO8;
    config->bits_per_pixel              = 8;
    config->subsampling                 = 1;
    config->binning                     = 1;
    config->sensor_scaling              = 1;
    config->auto_gain                   = false;
    config->master_gain                 = 0;
    config->red_gain                    = 0;
    config->green_gain                  = 0;
    config->blue_gain                   = 0;
    config->gain_boost                  = false;
    config->auto_exposure               = false;
    config->exposure                    = 33;
    config->auto_white_balance          = false;
    config->white_balance_red_offset    = 0;
    config->white_balance_blue_offset   = 0;
    config->auto_frame_rate             = false;
    config->frame_rate                  = 30;
    config->pixel_clock                 = 42;
    config->ext_trigger_mode            = false;
    config->ext_trigger_delay           = 0;
    config->flash_delay                 = 1000;
    config->flash_duration              = 1;
    config->flip_upd                    = false;
    config->flip_lr                     = false;
}

static void printCameraConfig(struct Ui3240Config *config)
{
    if(config == NULL)
    {
        fprintf(stderr, "input paras is NULL\n");
        return;
    }

    printf("|==================== Ui3240Config begin ====================\n");
    printf("| camera_handle             = %d\n",config->camera_handle);
    printf("| camera_id                 = %d\n",config->camera_id);
    printf("| master                    = %d\n",config->master);
    printf("| stereo                    = %d\n",config->stereo);
    printf("| capture_mode              = %d\n",config->capture_mode);
    printf("| capture_timeout           = %d\n",config->capture_timeout);
    printf("| image_width               = %d\n",config->image_width);
    printf("| image_height              = %d\n",config->image_height);
    printf("| image_left                = %d\n",config->image_left);
    printf("| image_top                 = %d\n",config->image_top);
    printf("| frame_num                 = %d\n",config->frame_num);
    printf("| frame_buf_size            = %d\n",config->frame_buf_size);
    printf("| color_mode                = %d\n",config->color_mode);
    printf("| bits_per_pixel            = %d\n",config->bits_per_pixel);
    printf("| subsampling               = %d\n",config->subsampling);
    printf("| binning                   = %d\n",config->binning);
    printf("| sensor_scaling            = %lf\n",config->sensor_scaling);
    printf("| auto_gain                 = %d\n",config->auto_gain);
    printf("| master_gain               = %d\n",config->master_gain);
    printf("| red_gain                  = %d\n",config->red_gain);
    printf("| green_gain                = %d\n",config->green_gain);
    printf("| blue_gain                 = %d\n",config->blue_gain);
    printf("| gain_boost                = %d\n",config->gain_boost);
    printf("| auto_exposure             = %d\n",config->auto_exposure);
    printf("| exposure                  = %lf\n",config->exposure);
    printf("| auto_white_balance        = %d\n",config->auto_white_balance);
    printf("| white_balance_red_offset  = %d\n",config->white_balance_red_offset);
    printf("| white_balance_blue_offset = %d\n",config->white_balance_blue_offset);
    printf("| auto_frame_rate           = %d\n",config->auto_frame_rate);
    printf("| frame_rate                = %lf\n",config->frame_rate);
    printf("| pixel_clock               = %d\n",config->pixel_clock);
    printf("| ext_trigger_mode          = %d\n",config->ext_trigger_mode);
    printf("| ext_trigger_delay         = %d\n",config->ext_trigger_delay);
    printf("| flash_delay               = %d\n",config->flash_delay);
    printf("| flash_duration            = %d\n",config->flash_duration);
    printf("| flip_upd                  = %d\n",config->flip_upd);
    printf("| flip_lr                   = %d\n",config->flip_lr);
    printf("|===================== Ui3240Config end =====================\n");
}

void sendFrameRateMsgToThreadSync(void)
{
    int ret = 0;
    unsigned short msg_len = 9;
    static unsigned char cnt = 0;

    memset(&FrameRateMsg,0,sizeof(struct QueueMsgNormal));

    FrameRateMsg.mtype = 1;
    FrameRateMsg.mtext[0] = (char)(((unsigned short)msg_len >> 8) & 0x00FF);
    FrameRateMsg.mtext[1] = (char)(((unsigned short)msg_len >> 0) & 0x00FF);

    memcpy(&FrameRateMsg.mtext[2],&cameraConfig.auto_frame_rate,1);
    memcpy(&FrameRateMsg.mtext[3],&cameraConfig.frame_rate,8);

    RE_SEND:
    ret = msgsnd(FrameRateMsgId,&FrameRateMsg,sizeof(FrameRateMsg.mtext),0);
    if(ret == -1)
    {
        fprintf(stderr, "thread_ui3240: send FrameRateMsg failed,failed times: %d\n",cnt + 1);

        usleep(100 * 1000);

        if((cnt ++) <= 10)
        {
            goto RE_SEND;
        }
    }

    cnt = 0;
}

static void ui3240CreateMsgQueue(void)
{
    FrameRateMsgId = msgget((key_t)KEY_FRAME_RATE_MSG, IPC_CREAT | 0777);
    if(FrameRateMsgId == -1)
    {
        fprintf(stderr, "thread_ui3240: create FrameRateMsg failed\n");
    }

    ui3240ImageCounterMsgId = msgget((key_t)KEY_VIDEO_IMAGE_COUNTER_MSG, IPC_CREAT | 0777);
    if(ui3240ImageCounterMsgId == -1)
    {
        fprintf(stderr, "thread_ui3240: create ui3240ImageCounterMsg failed\n");
    }

    memset(&FrameRateMsg,0,sizeof(struct QueueMsgNormal));
    memset(&ui3240ImageCounterMsg,0,sizeof(struct QueueMsgNormal));
}

static int ui3240SendImageCounterToMainThread(unsigned int counter)
{
    int ret = 0;

    ui3240ImageCounterMsg.mtype = 1;
    ui3240ImageCounterMsg.mtext[0] = (char)((counter >> 24) & 0x000000FF);
    ui3240ImageCounterMsg.mtext[1] = (char)((counter >> 16) & 0x000000FF);
    ui3240ImageCounterMsg.mtext[2] = (char)((counter >> 16) & 0x000000FF);
    ui3240ImageCounterMsg.mtext[3] = (char)((counter >> 16) & 0x000000FF);

    ret = msgsnd(ui3240ImageCounterMsgId,&ui3240ImageCounterMsg,sizeof(ui3240ImageCounterMsg.mtext),0);
    if(ret == -1)
    {
        fprintf(stderr, "thread_ui3240: send ui3240ImageCounterMsg failed\n");
    }

    return ret;
}

void *thread_ui3240(void *arg)
{
    int ret = IS_SUCCESS;
    struct CmdArgs *args = (struct CmdArgs *)arg;
    enum CameraState camera_state = INIT_CONFIG;
    char *frame_buf = NULL;

    ui3240CreateMsgQueue();                              //创建消息队列

    while(1)
    {
        switch((unsigned char)camera_state)
        {
            case (unsigned char)INIT_CONFIG:            //初始化配置
                initCameraConfig(&cameraConfig,args);
                camera_state = CONNECT_CAMERA;
            break;

            case (unsigned char)CONNECT_CAMERA:         //链接相机
                ret = connectCamrea(cameraConfig.camera_id);
                if(ret != IS_SUCCESS)
                {
                    printf("thread_ui3240: connect camera failed\n");
                    camera_state = DISCONNECT_CAMERA;
                }
                camera_state = LOAD_DEFAULT_CONFIG;
            break;

            case (unsigned char)LOAD_DEFAULT_CONFIG:    //加载默认配置
                ret = loadCameraDefaultConfig(L"./config/ids_default_config.ini", false);
                if(ret != IS_SUCCESS)
                {
                    printf("thread_ui3240: load camera default config failed\n");
                    camera_state = DISCONNECT_CAMERA;
                }
                else
                {
                    camera_state = QUERY_CAMERA_CONFIG;
                }
            break;

            case (unsigned char)QUERY_CAMERA_CONFIG:    //获取相机配置
                ret = queryCameraConfig();
                if(ret != IS_SUCCESS)
                {
                    printf("thread_ui3240: query camera config failed\n");
                }
                camera_state = LOAD_USER_CONFIG;
            break;

            case (unsigned char)LOAD_USER_CONFIG:       //加载用户配置
                ret = loadCameraUserConfig(args->usb_cam_user_conf_file);
                if(ret != IS_SUCCESS)
                {
                    printf("thread_ui3240: load camera user config failed\n");
                    camera_state = ALLOC_FRAME_BUFFER;
                }
                else
                {
                    camera_state = SET_USER_CONFIG;
                }
            break;

            case (unsigned char)SET_USER_CONFIG:        //设置用户配置
                setCameraUserConfig();
                camera_state = ALLOC_FRAME_BUFFER;
            break;

            case (unsigned char)ALLOC_FRAME_BUFFER:     //申请帧缓存
                ret = reallocateCameraBuffer();
                if(ret != IS_SUCCESS)
                {
                    printf("thread_ui3240: alloc camera buffer failed\n");
                    camera_state = DISCONNECT_CAMERA;
                }
                else
                {
                    sendFrameRateMsgToThreadSync();

                    camera_state = SET_TRIGGER_MODE;
                }
            break;

            case (unsigned char)SET_TRIGGER_MODE:       //设置触发模式 外部触发 自由模式
                ret = setTriggerMode();
                if(ret != IS_SUCCESS)
                {
                    printf("thread_ui3240: set trigger mode failed\n");
                    camera_state = DISCONNECT_CAMERA;
                }
                else
                {
                    camera_state = CAPTURE_IMAGE;
                }   
            break;

            case (unsigned char)CAPTURE_IMAGE:          //采集图像
                frame_buf = captureImage(cameraConfig.capture_mode,cameraConfig.capture_timeout);
                if(frame_buf != NULL)
                {
                    printf("thread_ui3240: capture iamge success\n");
                    frame_buf = NULL;
                    ImageCounter ++;

                    ret = ui3240SendImageCounterToMainThread(ImageCounter);
                    if(ret != 0)
                    {
                        printf("thread_ui3240: send ui3240ImageCounterMsg failed\n");
                    }
                }
            break;

            case (unsigned char)DISCONNECT_CAMERA:      //断开相机链接
                disconnectCamera();
                usleep(1000 * 1000);
                camera_state = INIT_CONFIG;
            break;

            default:
                camera_state = DISCONNECT_CAMERA;
            break;
        }
    }
}
