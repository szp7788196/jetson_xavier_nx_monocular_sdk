#include <stdio.h>
#include "common.h"
#include "cmd_parse.h"
#include "ub482.h"
#include "net.h"
#include "mpu9250.h"
#include "ui3240.h"
#include "cssc132.h"
#include "sync_module.h"
#include "sync.h"
#include "led.h"


static void syncAndMutexCreate(void)
{
    pthread_mutex_init(&mutexImageHeap, NULL);
    pthread_mutex_init(&mutexImuAdis16505Heap, NULL);
    pthread_cond_init(&condImageHeap, NULL);
}

static int pthreadCreate(void *args)
{
    int ret = 0;
    static pthread_t tid_ub482 = 0;
    static pthread_t tid_net = 0;
    static pthread_t tid_mpu9250 = 0;
    static pthread_t tid_ui3240 = 0;
    static pthread_t tid_cssc132 = 0;
    static pthread_t tid_sync_module = 0;
    static pthread_t tid_sync = 0;
    static pthread_t tid_led = 0;

    ret = pthread_create(&tid_ub482,NULL,thread_ub482,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_ub482 failed\n",__func__);
    }

    ret = pthread_create(&tid_net,NULL,thread_net,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_net failed\n",__func__);
    }

    ret = pthread_create(&tid_mpu9250,NULL,thread_mpu9250,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_mpu9250 failed\n",__func__);
    }

    if(cmdArgs.camera_module == 0)
    {
        ret = pthread_create(&tid_ui3240,NULL,thread_ui3240,&cmdArgs);
        if(0 != ret)
        {
            fprintf(stderr, "%s: create thread_ui3240 failed\n",__func__);
        }
    }
    else
    {
        ret = pthread_create(&tid_cssc132,NULL,thread_cssc132,&cmdArgs);
        if(0 != ret)
        {
            fprintf(stderr, "%s: create thread_cssc132 failed\n",__func__);
        }
    }

    ret = pthread_create(&tid_sync_module,NULL,thread_sync_module,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_sync_module failed\n",__func__);
    }

    ret = pthread_create(&tid_sync,NULL,thread_sync,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_sync failed\n",__func__);
    }

    ret = pthread_create(&tid_led,NULL,thread_led,&cmdArgs);
    if(0 != ret)
    {
        fprintf(stderr, "%s: create thread_led failed\n",__func__);
    }

    return ret;
}

int main(int argc, char **argv)
{
    int ret = 0;

    ret = cmdParse(argc, argv, &cmdArgs);           //解析命令
    if(ret != 1)
    {
        fprintf(stderr, "%s: parse shell cmd failed\n",__func__);
    }

    clearSystemQueueMsg();

    syncAndMutexCreate();
    pthreadCreate(&cmdArgs);

    while(1)
    {
        sleep(5);
    }

	return ret;
}





























