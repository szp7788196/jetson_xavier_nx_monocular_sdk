#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

enum SerialBaudrate
{
    SPABAUD_50 = B50, 
    SPABAUD_110 = B110, 
    SPABAUD_300 = B300, 
    SPABAUD_600 = B600,
    SPABAUD_1200 = B1200, 
    SPABAUD_2400 = B2400, 
    SPABAUD_4800 = B4800,
    SPABAUD_9600 = B9600, 
    SPABAUD_19200 = B19200,
    SPABAUD_38400 = B38400, 
    SPABAUD_57600 = B57600, 
    SPABAUD_115200 = B115200,
    SPABAUD_230400 = B230400,
    SPABAUD_460800 = B460800,
    SPABAUD_921600 = B921600
};

enum SerialDatabits 
{
    SPADATABITS_5 = CS5,
    SPADATABITS_6 = CS6,
    SPADATABITS_7 = CS7,
    SPADATABITS_8 = CS8
};

enum SerialStopbits 
{
    SPASTOPBITS_1 = 0,
    SPASTOPBITS_2 = CSTOPB
};

enum SerialParity 
{
    SPAPARITY_NONE = 0,
    SPAPARITY_ODD = PARODD | PARENB,
    SPAPARITY_EVEN = PARENB
};

enum SerialProtocol 
{
    SPAPROTOCOL_NONE = 0,
    SPAPROTOCOL_RTS_CTS = 9999,
    SPAPROTOCOL_XON_XOFF = IXOFF | IXON
};

struct Serial
{
    struct termios Termios;
    int Stream;
};


enum SerialParity SerialGetParity(const char *buf, int *ressize);
enum SerialProtocol SerialGetProtocol(const char *buf, int *ressize);
void SerialFree(struct Serial *sn);
int SerialInit(struct Serial *sn,
               const char *Device,
               enum SerialBaudrate Baud, 
               enum SerialStopbits StopBits,
               enum SerialProtocol Protocol,
               enum SerialParity Parity,
               enum SerialDatabits DataBits,
               int original);
int SerialRead(struct Serial *sn, char *buffer, size_t size);
int SerialWrite(struct Serial *sn, const char *buffer, size_t size);

#endif
