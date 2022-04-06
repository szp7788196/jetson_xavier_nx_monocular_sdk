#include "serial.h"


enum SerialParity SerialGetParity(const char *buf, int *ressize)
{
    int r = 0;
    enum SerialParity p = SPAPARITY_NONE;

    if(!strncasecmp(buf, "none", 4))
    {
        r = 4;
        p = SPAPARITY_NONE; 
    }
    else if(!strncasecmp(buf, "no", 2))
    {
        r = 2;
        p = SPAPARITY_NONE;
    }
    else if(!strncasecmp(buf, "odd", 3))
    {
        r = 3;
        p = SPAPARITY_ODD;
    }
    else if(!strncasecmp(buf, "even", 4))
    {
        r = 4;
        p = SPAPARITY_EVEN;
    }
    else if(*buf == 'N' || *buf == 'n')
    {
        r = 1;
        p = SPAPARITY_NONE;
    }
    else if(*buf == 'O' || *buf == 'o')
    {
        r = 1;
        p = SPAPARITY_ODD;
    }
    else if(*buf == 'E' || *buf == 'e')
    {
        r = 1;
        p = SPAPARITY_EVEN;
    }

    if(ressize)
    {
        *ressize = r;
    }

    return p;
}

enum SerialProtocol SerialGetProtocol(const char *buf, int *ressize)
{
    int r = 0;
    enum SerialProtocol Protocol = SPAPROTOCOL_NONE;

    /* try some possible forms for input, be as gentle as possible */
    if(!strncasecmp("xonxoff",buf,7))
    {
        r = 7;
        Protocol=SPAPROTOCOL_XON_XOFF;
    }
    else if(!strncasecmp("xon_xoff",buf,8))
    {
        r = 8;
        Protocol=SPAPROTOCOL_XON_XOFF;
    }
    else if(!strncasecmp("xon-xoff",buf,8))
    {
        r = 8;
        Protocol=SPAPROTOCOL_XON_XOFF;
    }
    else if(!strncasecmp("xon xoff",buf,8))
    {
        r = 8;
        Protocol=SPAPROTOCOL_XON_XOFF;
    }
    else if(!strncasecmp("xoff",buf,4))
    {
        r = 4;
        Protocol=SPAPROTOCOL_XON_XOFF;
    }
    else if(!strncasecmp("xon",buf,3))
    {
        r = 3;
        Protocol=SPAPROTOCOL_XON_XOFF;
    }
    else if(*buf == 'x' || *buf == 'X')
    {
        r = 1;
        Protocol=SPAPROTOCOL_XON_XOFF;
    }
    else if(!strncasecmp("rtscts",buf,6))
    {
        r = 6;
        Protocol=SPAPROTOCOL_RTS_CTS;
    }
    else if(!strncasecmp("rts_cts",buf,7))
    {
        r = 7;
        Protocol=SPAPROTOCOL_RTS_CTS;
    }
    else if(!strncasecmp("rts-cts",buf,7))
    {
        r = 7;
        Protocol=SPAPROTOCOL_RTS_CTS;
    }
    else if(!strncasecmp("rts cts",buf,7))
    {
        r = 7;
        Protocol=SPAPROTOCOL_RTS_CTS;
    }
    else if(!strncasecmp("rts",buf,3))
    {
        r = 3;
        Protocol=SPAPROTOCOL_RTS_CTS;
    }
    else if(!strncasecmp("cts",buf,3))
    {
        r = 3;
        Protocol=SPAPROTOCOL_RTS_CTS;
    }
    else if(*buf == 'r' || *buf == 'R' || *buf == 'c' || *buf == 'C')
    {
        r = 1;
        Protocol=SPAPROTOCOL_RTS_CTS;
    }
    else if(!strncasecmp("none",buf,4))
    {
        r = 4;
        Protocol=SPAPROTOCOL_NONE;
    }
    else if(!strncasecmp("no",buf,2))
    {
        r = 2;
        Protocol=SPAPROTOCOL_NONE;
    }
    else if(*buf == 'n' || *buf == 'N')
    {
        r = 1;
        Protocol=SPAPROTOCOL_NONE;
    
    }
    if(ressize)
    {
        *ressize = r;
    }

    return Protocol;
}

void SerialFree(struct Serial *sn)
{
    if(sn->Stream)
    {
        /* reset old settings */
        tcsetattr(sn->Stream, TCSANOW, &sn->Termios);
        close(sn->Stream);
        sn->Stream = 0;
    }
}

int SerialInit(struct Serial *sn,
                 const char *Device,
                 enum SerialBaudrate Baud, 
                 enum SerialStopbits StopBits,
                 enum SerialProtocol Protocol,
                 enum SerialParity Parity,
                 enum SerialDatabits DataBits,
                 int original)
{
    struct termios newtermios;

    if((sn->Stream = open(Device, O_RDWR | O_NOCTTY | O_NONBLOCK)) <= 0)
    {
        return -1;
    }

    tcgetattr(sn->Stream, &sn->Termios);

    memset(&newtermios, 0, sizeof(struct termios));

    newtermios.c_cflag = Baud | StopBits | Parity | DataBits | CLOCAL | CREAD;

    if(Protocol == SPAPROTOCOL_RTS_CTS)
    {
        newtermios.c_cflag |= CRTSCTS;
    }
    else
    {
        newtermios.c_cflag |= Protocol;
    }

    if(original)
    {
        newtermios.c_lflag |= ICANON;
    }
    else
    {
        newtermios.c_lflag &= ~ICANON;
    }

    newtermios.c_cc[VMIN] = 1;

    tcflush(sn->Stream, TCIOFLUSH);
    tcsetattr(sn->Stream, TCSANOW, &newtermios);
    tcflush(sn->Stream, TCIOFLUSH);
    fcntl(sn->Stream, F_SETFL, O_NONBLOCK);

    return 0;
}

int SerialRead(struct Serial *sn, char *buffer, size_t size)
{
    int j = 0;
    
    j = read(sn->Stream, buffer, size);

    if(j < 0)
    {
        if(errno == EAGAIN)
        {
            return 0;
        }
        else
        {
            return j;
        }
    }

    return j;
}

int SerialWrite(struct Serial *sn, const char *buffer, size_t size)
{
    int j = 0;
    int ofs = 0;

    while(size > ofs)
    {
        j = write(sn->Stream, buffer + ofs, size - ofs);
        if(j < 0)
        {
            if(errno == EAGAIN)
            {
                return 0;
            }
            else
            {
                return j;
            }
        }
        else
        {
            ofs += j;
        }
    }

    return j;
}


