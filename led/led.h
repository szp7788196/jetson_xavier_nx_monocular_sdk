#ifndef __LED_H
#define __LED_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>


#define IOC_LED_MAGIC                   'L'
#define IOC_LED_INIT                    _IO(IOC_LED_MAGIC, 0)
#define IOC_W_LED1_ON                   _IOW(IOC_LED_MAGIC, 1, int)
#define IOC_W_LED1_OFF                  _IOW(IOC_LED_MAGIC, 2, int)
#define IOC_W_LED2_ON                   _IOW(IOC_LED_MAGIC, 3, int)
#define IOC_W_LED2_OFF                  _IOW(IOC_LED_MAGIC, 4, int)
#define IOC_W_LED3_ON                   _IOW(IOC_LED_MAGIC, 5, int)
#define IOC_W_LED3_OFF                  _IOW(IOC_LED_MAGIC, 6, int)
#define IOC_W_LED4_ON                   _IOW(IOC_LED_MAGIC, 7, int)
#define IOC_W_LED4_OFF                  _IOW(IOC_LED_MAGIC, 8, int)
#define IOC_W_LED_ALL_ON                _IOW(IOC_LED_MAGIC, 9, int)
#define IOC_W_LED_ALL_OFF               _IOW(IOC_LED_MAGIC, 10, int)
#define IOC_W_LED_ALL_STATE             _IOW(IOC_LED_MAGIC, 11, int)
#define IOC_LED_MAX_NUM                 11


void *thread_led(void *config);

#endif
