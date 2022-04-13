#include "sync.h"

static int syncImageAndCameraTimeStamp(void)
{
    int ret = 0;
    struct SyncCamTimeStamp *cam_time_stamp = NULL;
    struct SyncCamTimeStamp time_stamp;
    static unsigned int image_counter = 0;
    static char first_sync = 1;

    ret = xQueueReceive((key_t)KEY_SYNC_CAM_TIME_STAMP_MSG,(void **)&cam_time_stamp,1);
    if(ret == -1)
    {
        return 0;
    }

    memcpy(&time_stamp,cam_time_stamp,sizeof(struct SyncCamTimeStamp));
    free(cam_time_stamp);
    cam_time_stamp = NULL;

    pthread_mutex_lock(&mutexImageHeap);

    pthread_cond_wait(&condImageHeap, &mutexImageHeap);

    memcpy(imageHeap.heap[imageHeap.put_ptr]->time_stamp,&time_stamp,sizeof(struct SyncCamTimeStamp));

    if(first_sync == 1)
    {
        first_sync = 0;

        do
        {
            ret = xQueueReceive((key_t)KEY_SYNC_CAM_TIME_STAMP_MSG,(void **)&cam_time_stamp,0);
            if(ret != -1)
            {
                image_counter = cam_time_stamp->counter;

                free(cam_time_stamp);
                cam_time_stamp = NULL;

                fprintf(stderr, "%s: +\n",__func__);
            }
        }
        while(ret != -1);

        ret = 0;

        image_counter = imageHeap.heap[imageHeap.put_ptr]->time_stamp->counter;
    }
    else
    {
        image_counter ++;
        imageHeap.heap[imageHeap.put_ptr]->image->counter = image_counter;

        if(imageHeap.heap[imageHeap.put_ptr]->image->counter != 
           imageHeap.heap[imageHeap.put_ptr]->time_stamp->counter)
        {
            if(abs(imageHeap.heap[imageHeap.put_ptr]->time_stamp->counter - 
                   imageHeap.heap[imageHeap.put_ptr]->image->counter) >= NOT_SYNC_THRESHOLD)
            {
                fprintf(stderr, "%s: sync image and camera trigger time stamp failed\n",__func__);
                first_sync = 1;
                ret = -1;
            }
        }
        else
        {
//            fprintf(stderr, "%s: sync image and camera trigger time stamp success\n",__func__);
        }
    }

    imageHeap.put_ptr = (imageHeap.put_ptr + 1) % imageHeap.depth;

	imageHeap.cnt += 1;
	if(imageHeap.cnt >= imageHeap.depth)
	{
		imageHeap.cnt = imageHeap.depth;

		imageHeap.get_ptr = imageHeap.put_ptr;
	}

    pthread_mutex_unlock(&mutexImageHeap);

    return ret;
}

static int sendQueueMsgToResetCameraAndSyncModule(void)
{
    int ret = 0;
    unsigned char *camera_reset = NULL;
    unsigned char *sync_module_reset = NULL;

    camera_reset = (unsigned char *)malloc(sizeof(unsigned char));
    if(camera_reset != NULL)
    {
        *camera_reset = 1;

        ret = xQueueSend((key_t)KEY_CAMERA_RESET_MSG,camera_reset);
        if(ret == -1)
        {
            fprintf(stderr, "%s: send camera reset queue msg failed\n",__func__);
        }
    }

    sync_module_reset = (unsigned char *)malloc(sizeof(unsigned char));
    if(sync_module_reset != NULL)
    {
        *sync_module_reset = 1;

        ret = xQueueSend((key_t)KEY_SYNC_MODULE_RESET_MSG,sync_module_reset);
        if(ret == -1)
        {
            fprintf(stderr, "%s: send sync module reset queue msg failed\n",__func__);
        }
    }

    return ret;
}

void *thread_sync(void *arg)
{
    int ret = 0;

    while(1)
    {
        ret = syncImageAndCameraTimeStamp();
        if(ret == -1)
        {
            sendQueueMsgToResetCameraAndSyncModule();

            fprintf(stderr, "%s: send queue msg to reset camera and sync module\n",__func__);
        }
    }
}