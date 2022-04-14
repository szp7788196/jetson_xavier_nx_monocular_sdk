#include "led.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "net.h"
#include "ub482.h"

int ledFd;


void allLedInit(void)
{
    int i = 0;

    for(i = 0; i < 5; i ++)
    {
        ioctl(ledFd,IOC_W_LED_ALL_ON);
        usleep(1000 * 250);
        ioctl(ledFd,IOC_W_LED_ALL_OFF);
        usleep(1000 * 250);
    }
}

void *thread_led(void *arg)
{
    static unsigned int cnt = 0;
    unsigned char led_state = 0;
    unsigned char mirror_led_state = 0;
    unsigned char led_run_state = 0;
	unsigned char led_net_state = 0;
    unsigned char led_gnss_state = 0;

    /* 打开设备 */
	ledFd = open("/dev/gpio-led", O_RDWR);
	if(0 > ledFd) 
    {
        fprintf(stderr, "%s: open /dev/gpio-led failed\n",__func__);
		goto THREAD_EXIT;
	}

    allLedInit();

    while(1)
    {
        if(cnt % 40 == 0)
		{
			led_run_state = ~led_run_state;
		}

        if(connectState == CONNECTED)
        {
            if(cnt % 300 == 0)
            {
                led_net_state = 1;
            }
            else
            {
                led_net_state = 0;
            }
        }
        else
        {
            if(cnt % 25 == 0)
            {
                led_net_state = ~led_net_state;
            }
        }

        if(gnssState == TYPE_NARROW_INT)
        {
            if(cnt % 300 == 0)
            {
                led_gnss_state = 1;
            }
            else
            {
                led_gnss_state = 0;
            }
        }
        else if(gnssState == TYPE_NONE)
        {
            if(cnt % 25 == 0)
            {
                led_gnss_state = ~led_gnss_state;
            }
        }
        else
        {
            if(cnt % 100 == 0)
            {
                led_gnss_state = 1;
            }
            else
            {
                led_gnss_state = 0;
            }
        }

        if(led_run_state)
        {
            led_state |= (1 << 0);
        }
        else
        {
            led_state &= ~(1 << 0);
        }

        if(led_net_state)
        {
            led_state |= (1 << 1);
        }
        else
        {
            led_state &= ~(1 << 1);
        }

        if(led_gnss_state)
        {
            led_state |= (1 << 2);
        }
        else
        {
            led_state &= ~(1 << 2);
        }

        if(mirror_led_state != led_state)
        {
            mirror_led_state = led_state;
            ioctl(ledFd,IOC_W_LED_ALL_STATE,&led_state);
        }
        
        cnt = (cnt + 1) & 0xFFFFFFFF;
        
        usleep(1000 * 10);
    }

THREAD_EXIT:
    pthread_exit("thread_led: /dev/gpio-led failed\n");
}