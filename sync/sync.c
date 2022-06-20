#include "sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <semaphore.h>
#include "monocular.h"
#include "cmd_parse.h"

static int syncImageAndCameraTimeStamp(void)
{
    int ret = 0;
    int res = 0;
    int i = 0;
    static struct SyncCamTimeStamp time_stamp;
    static char sync_1hz_success = 0;
    static char got_time_stamp = 0;
    static char got_image = 0;
    struct timespec start_tm;
	struct timespec end_tm;
    int timeout_ms = 5;

    if(got_time_stamp == 0)
    {
        clock_gettime(CLOCK_REALTIME, &start_tm);
        end_tm = ns_to_tm(tm_to_ns(start_tm) + timeout_ms * 1000000);

        if(sem_timedwait(&sem_t_SyncCamTimeStampHeap, &end_tm) == 0)
        {
            res = syncCamTimeStampHeapGet(&time_stamp);
            if(res == 0)
            {
                got_time_stamp = 1;
            }
        }
        else
        {
            // fprintf(stderr, "%s: wait time_stamp cond timeout or error\n",__func__);
        }
    }

    if(got_image == 0)
    {
        clock_gettime(CLOCK_REALTIME, &start_tm);
        end_tm = ns_to_tm(tm_to_ns(start_tm) + timeout_ms * 1000000);

        if(sem_timedwait(&sem_t_ImageHeap, &end_tm) == -1)
        {
            res = imageHeapGet(&imageBuffer);
            if(res == 0)
            {
                got_image = 1;
            }
        }
        else
        {
            // fprintf(stderr, "%s: wait imageBuffer cond timeout or error\n",__func__);
        }
    }

    if(got_time_stamp == 1 && got_image == 1)
    {
        got_time_stamp = 0;
        got_image = 0;

        if(time_stamp.number != imageBuffer.number)
        {
            fprintf(stderr, "%s:sync failed  time_stamp.number = %d, imageBuffer.number = %d\n",__func__,time_stamp.number,imageBuffer.number);

            imageHeap.put_ptr = 0;
            imageHeap.get_ptr = 0;

            for(i = 0; i < imageHeap.depth; i ++)
            {
                imageHeap.heap[i]->number = 0;
            }

            syncCamTimeStampHeap.put_ptr = 0;
            syncCamTimeStampHeap.get_ptr = 0;

            for(i = 0; i < syncCamTimeStampHeap.depth; i ++)
            {
                syncCamTimeStampHeap.heap[i]->number = 0;
            }

            sync_1hz_success = 0;
            ret = -1;

            goto GET_OUT;
        }
        else
        {
            if(sync_1hz_success < 10)
            {
                sync_1hz_success ++;

                if(sync_1hz_success == 10)
                {
                    ret = xQueueSend((key_t)KEY_SYNC_1HZ_SUCCESS_MSG,&sync_1hz_success,MAX_QUEUE_MSG_NUM);
                    if(ret == -1)
                    {
                        fprintf(stderr, "%s: send sync_1hz_success queue msg failed\n",__func__);
                    }
                }
            }

            imageUnitHeapPut(&imageBuffer, &time_stamp);

            sem_post(&sem_t_ImageUnitHeap);
        }
    }

GET_OUT:
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

        ret = xQueueSend((key_t)KEY_CAMERA_RESET_MSG,camera_reset,MAX_QUEUE_MSG_NUM);
        if(ret == -1)
        {
            fprintf(stderr, "%s: send camera reset queue msg failed\n",__func__);
        }
    }

    sync_module_reset = (unsigned char *)malloc(sizeof(unsigned char));
    if(sync_module_reset != NULL)
    {
        *sync_module_reset = 1;

        ret = xQueueSend((key_t)KEY_SYNC_MODULE_RESET_MSG,sync_module_reset,MAX_QUEUE_MSG_NUM);
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