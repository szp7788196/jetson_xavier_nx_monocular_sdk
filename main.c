#include <stdio.h>
#include "common.h"
#include "cmd_parse.h"
#include "ub482.h"
#include "net.h"
#include "mpu9250.h"
#include "ui3240.h"
#include "cssc132.h"
#include "sync_module.h"
#include "led.h"

#define NOT_SYNC_THRESHOLD      30

pthread_t tid_ub482 = 0;
pthread_t tid_net = 0;
pthread_t tid_mpu9250 = 0;
pthread_t tid_ui3240 = 0;
pthread_t tid_cssc132 = 0;
pthread_t tid_sync = 0;
pthread_t tid_led = 0;

static struct QueueMsgNormal syncCamCounterMsg;
static struct QueueMsgNormal ui3240ImageCounterMsg;
static int syncCamCounterMsgId = -1;
static int ui3240ImageCounterMsgId = -1;
static unsigned int syncCamCounter = 0;
static unsigned int ui3240ImageCounter = 0;
static int sync_D_value = 0;
static int last_sync_D_value = 0;
static unsigned char first_sync = 0;

static void mainCreateMsgQueue(void)
{
    syncCamCounterMsgId = msgget((key_t)KEY_FRAME_RATE_MSG, IPC_CREAT | 0777);
    if(syncCamCounterMsgId == -1)
    {
        fprintf(stderr, "thread_mian: create syncCamCounterMsg failed\n");
    }

    ui3240ImageCounterMsgId = msgget((key_t)KEY_VIDEO_IMAGE_COUNTER_MSG, IPC_CREAT | 0777);
    if(ui3240ImageCounterMsgId == -1)
    {
        fprintf(stderr, "thread_main: create ui3240ImageCounterMsg failed\n");
    }

    memset(&syncCamCounterMsg,0,sizeof(struct QueueMsgNormal));
    memset(&ui3240ImageCounterMsg,0,sizeof(struct QueueMsgNormal));
}


int main(int argc, char **argv)
{
    int ret = 0;

    mainCreateMsgQueue();

    ret = cmdParse(argc, argv, &cmdArgs);           //解析命令
    if(ret != 1)
    {
        fprintf(stderr, "thread_main: parse shell cmd failed\n");
    }
/*
    ret = pthread_create(&tid_ub482,NULL,thread_ub482,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "thread_main: create thread_ub482 failed\n");
        exit(1);
    }

    ret = pthread_create(&tid_net,NULL,thread_net,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "thread_main: create thread_net failed\n");
        exit(1);
    }

    ret = pthread_create(&tid_mpu9250,NULL,thread_mpu9250,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "thread_main: create thread_mpu9250 failed\n");
        exit(1);
    }

    ret = pthread_create(&tid_ui3240,NULL,thread_ui3240,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "thread_main: create thread_ui3240 failed\n");
        exit(1);
    }
*/
    ret = pthread_create(&tid_cssc132,NULL,thread_cssc132,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "thread_main: create thread_cssc132 failed\n");
        exit(1);
    }
/*
    ret = pthread_create(&tid_sync,NULL,thread_sync,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "thread_main: create thread_sync failed\n");
        exit(1);
    }

    ret = pthread_create(&tid_led,NULL,thread_led,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "thread_main: create thread_led failed\n");
        exit(1);
    }
*/
    while(1)
    {
        ret = msgrcv(ui3240ImageCounterMsgId,&ui3240ImageCounterMsg,sizeof(ui3240ImageCounterMsg.mtext),1,IPC_NOWAIT);
        if(ret != -1)
        {
            ui3240ImageCounter = ((((unsigned int)ui3240ImageCounterMsg.mtext[0]) << 24) & 0xFF000000) + 
                                 ((((unsigned int)ui3240ImageCounterMsg.mtext[1]) << 16) & 0x00FF0000) + 
                                 ((((unsigned int)ui3240ImageCounterMsg.mtext[2]) <<  8) & 0x0000FF00) + 
                                 ((((unsigned int)ui3240ImageCounterMsg.mtext[3]) <<  0) & 0x000000FF);
        }

        ret = msgrcv(syncCamCounterMsgId,&syncCamCounterMsg,sizeof(syncCamCounterMsg.mtext),1,IPC_NOWAIT);
        if(ret != -1)
        {
            syncCamCounter = ((((unsigned int)syncCamCounterMsg.mtext[0]) << 24) & 0xFF000000) + 
                             ((((unsigned int)syncCamCounterMsg.mtext[1]) << 16) & 0x00FF0000) + 
                             ((((unsigned int)syncCamCounterMsg.mtext[2]) <<  8) & 0x0000FF00) + 
                             ((((unsigned int)syncCamCounterMsg.mtext[3]) <<  0) & 0x000000FF);
        }

        if(ui3240ImageCounter != 0 && syncCamCounter != 0)
        {
            sync_D_value = syncCamCounter - ui3240ImageCounter;

            if(first_sync == 0)
            {
                first_sync = 1;

                last_sync_D_value = sync_D_value;
            }
            else
            {
                if(last_sync_D_value != sync_D_value)
                {
                    if(abs(sync_D_value - last_sync_D_value) >= NOT_SYNC_THRESHOLD)
                    {
                        last_sync_D_value = sync_D_value;
                        fprintf(stderr, "thread_main: sync failed\n");
                    }
                    else
                    {
                        fprintf(stdout, "thread_main: sync success\n");
                    }
                }
            }
        }

        usleep(1000 * 10);
    }

	return ret;
}





























