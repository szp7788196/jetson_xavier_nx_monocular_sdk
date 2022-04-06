#include "cmd_parse.h"

struct CmdArgs cmdArgs;

#define LONG_OPT(a) a
#define ARGOPT "ha:b:c:d:e:f:g:i:j:k:l:m:n:o:p:q:r:s:t:u:v:w:x:y:z:A:B:C:D:E:F:G:H:I:J:K:L:"

static struct option opts[] = 
{
    { "help",               no_argument,       0, 'h'},
    { "server",             required_argument, 0, 'a'},
    { "port",               required_argument, 0, 'b'},
    { "mount_point",        required_argument, 0, 'c'},
    { "user_name",          required_argument, 0, 'd'},
    { "password",           required_argument, 0, 'e'},
    { "serial1",            required_argument, 0, 'f'},
    { "baudrate1",          required_argument, 0, 'g'},
    { "databits1",          required_argument, 0, 'i'},
    { "stopbits1",          required_argument, 0, 'j'},
    { "parity1",            required_argument, 0, 'k'},
    { "protocol1",          required_argument, 0, 'l'},
    { "serial2",            required_argument, 0, 'm'},
    { "baudrate2",          required_argument, 0, 'n'},
    { "databits2",          required_argument, 0, 'o'},
    { "stopbits2",          required_argument, 0, 'p'},
    { "parity2",            required_argument, 0, 'q'},
    { "protocol2",          required_argument, 0, 'r'},
    { "serial3",            required_argument, 0, 's'},
    { "baudrate3",          required_argument, 0, 't'},
    { "databits3",          required_argument, 0, 'u'},
    { "stopbits3",          required_argument, 0, 'v'},
    { "parity3",            required_argument, 0, 'w'},
    { "protocol3",          required_argument, 0, 'x'},
    { "capture_mode",       required_argument, 0, 'y'},
    { "capture_timeout",    required_argument, 0, 'z'},
    { "frame_num",          required_argument, 0, 'A'},
    { "gyro",               required_argument, 0, 'B'},
    { "accel",              required_argument, 0, 'C'},
    { "sample_rate",        required_argument, 0, 'D'},
    { "read_mode",          required_argument, 0, 'E'},
    { "camera1",            required_argument, 0, 'F'},
    { "camera2",            required_argument, 0, 'G'},
    { "camera1_ctrl",       required_argument, 0, 'H'},
    { "camera2_ctrl",       required_argument, 0, 'I'},
    { "usb_cam_def",        required_argument, 0, 'J'},
    { "usb_cam_user",       required_argument, 0, 'K'},
    { "mipi_cam_user",      required_argument, 0, 'L'},
    { 0,                    0,                 0,  0 }
};

int cmdParse(int argc, char **argv, struct CmdArgs *args)
{
    int res = 1;
    int getoptr;
    int i = 0;
    int help = 0;

    args->server                    = "rtk.ntrip.qxwz.com";
    args->port                      = "8002";
    args->mount_point               = "AUTO";
    args->user_name                 = "qxumew002";
    args->password                  = "755d514";
    args->serial1                   = "/dev/ttyTHS1";
    args->baudrate1                 = SPABAUD_115200;
    args->databits1                 = SPADATABITS_8;
    args->stopbits1                 = SPASTOPBITS_1;
    args->parity1                   = SPAPARITY_NONE;
    args->protocol1                 = SPAPROTOCOL_NONE;
    args->serial2                   = "/dev/ttyTHS0";
    args->baudrate2                 = SPABAUD_115200;
    args->databits2                 = SPADATABITS_8;
    args->stopbits2                 = SPASTOPBITS_1;
    args->parity2                   = SPAPARITY_NONE;
    args->protocol2                 = SPAPROTOCOL_NONE;
    args->serial3                   = "/dev/ttyUSB0";
    args->baudrate3                 = SPABAUD_230400;
    args->databits3                 = SPADATABITS_8;
    args->stopbits3                 = SPASTOPBITS_1;
    args->parity3                   = SPAPARITY_NONE;
    args->protocol3                 = SPAPROTOCOL_NONE;
    args->capture_mode              = 0;
    args->capture_timeout           = 2000;
    args->frame_num                 = 3;
    args->gyro_range                = 3;
    args->accel_range               = 0;
    args->sample_rate               = 125;
    args->read_mode                 = 0;
    args->camera1                   = "/dev/video0";
    args->camera2                   = "/dev/video1";
    args->camera1_ctrl              = "/dev/cssc132_ctrl_0";
    args->camera2_ctrl              = "/dev/cssc132_ctrl_1";
    args->usb_cam_def_conf_file     = "./config/ids_default_config.ini";
    args->usb_cam_user_conf_file    = "./config/ids_user_config.ini";
    args->mipi_cam_user_conf_file   = "./config/cssc132_user_config.ini";

    help = 0;

    do
    {
        getoptr = getopt_long(argc, argv, ARGOPT, opts, 0);
        switch(getoptr)
        {
            case 'a':
                args->server = optarg;
            break;

            case 'b':
                args->port = optarg;
            break;

            case 'c':
                args->mount_point = optarg;
            break;

            case 'd':
                args->user_name = optarg;
            break;

            case 'e':
                args->password = optarg;
            break;

            case 'f':
                args->serial1 = optarg;
            break;

            case 'g':
            {
                i = 0;
                i = strtol(optarg, 0, 10);
                switch(i)
                {
                    case 50:
                        args->baudrate1 = SPABAUD_50;
                    break;

                    case 110:
                        args->baudrate1 = SPABAUD_110;
                    break;

                    case 300:
                        args->baudrate1 = SPABAUD_300;
                    break;

                    case 600:
                        args->baudrate1 = SPABAUD_600;
                    break;

                    case 1200:
                        args->baudrate1 = SPABAUD_1200;
                    break;

                    case 2400:
                        args->baudrate1 = SPABAUD_2400;
                    break;

                    case 4800:
                        args->baudrate1 = SPABAUD_4800;
                    break;

                    case 9600:
                        args->baudrate1 = SPABAUD_9600;
                    break;

                    case 19200:
                        args->baudrate1 = SPABAUD_19200;
                    break;

                    case 38400:
                        args->baudrate1 = SPABAUD_38400;
                    break;

                    case 57600:
                        args->baudrate1 = SPABAUD_57600;
                    break;

                    case 115200:
                        args->baudrate1 = SPABAUD_115200;
                    break;

                    case 230400:
                        args->baudrate1 = SPABAUD_230400;
                    break;

                    case 460800:
                        args->baudrate1 = SPABAUD_460800;
                    break;

                    case 921600:
                        args->baudrate1 = SPABAUD_921600;
                    break;

                    default:
                        fprintf(stderr, "%s: Baudrate1 '%s' unknown\n",__func__, optarg);
                        res = 0;
                    break;
                }
            }
            break;

            case 'i':
                if(!strcmp(optarg, "5"))
                {
                    args->databits1 = SPADATABITS_5;
                }
                else if(!strcmp(optarg, "6"))
                {
                    args->databits1 = SPADATABITS_6;
                }
                else if(!strcmp(optarg, "7"))
                {
                    args->databits1 = SPADATABITS_7;
                }
                else if(!strcmp(optarg, "8"))
                {
                    args->databits1 = SPADATABITS_8;
                }
                else
                {
                    fprintf(stderr, "%s: Databits1 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'j':
                if(!strcmp(optarg, "1")) 
                {
                    args->stopbits1 = SPASTOPBITS_1;
                }
                else if(!strcmp(optarg, "2"))
                {
                    args->stopbits1 = SPASTOPBITS_2;
                }
                else
                {
                    fprintf(stderr, "%s: Stopbits1 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'k':
                i = 0;
                args->parity1 = SerialGetParity(optarg, &i);
                if(!i)
                {
                    fprintf(stderr, "%s: Parity1 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'l':
                i = 0;
                args->protocol1 = SerialGetProtocol(optarg, &i);
                if(!i)
                {
                    fprintf(stderr, "%s: Protocol1 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'm':
                args->serial2 = optarg;
            break;

            case 'n':
            {
                i = 0;
                i = strtol(optarg, 0, 10);
                switch(i)
                {
                    case 50:
                        args->baudrate2 = SPABAUD_50;
                    break;

                    case 110:
                        args->baudrate2 = SPABAUD_110;
                    break;

                    case 300:
                        args->baudrate2 = SPABAUD_300;
                    break;

                    case 600:
                        args->baudrate2 = SPABAUD_600;
                    break;

                    case 1200:
                        args->baudrate2 = SPABAUD_1200;
                    break;

                    case 2400:
                        args->baudrate2 = SPABAUD_2400;
                    break;

                    case 4800:
                        args->baudrate2 = SPABAUD_4800;
                    break;

                    case 9600:
                        args->baudrate2 = SPABAUD_9600;
                    break;

                    case 19200:
                        args->baudrate2 = SPABAUD_19200;
                    break;

                    case 38400:
                        args->baudrate2 = SPABAUD_38400;
                    break;

                    case 57600:
                        args->baudrate2 = SPABAUD_57600;
                    break;

                    case 115200:
                        args->baudrate2 = SPABAUD_115200;
                    break;

                    case 230400:
                        args->baudrate2 = SPABAUD_230400;
                    break;

                    case 460800:
                        args->baudrate2 = SPABAUD_460800;
                    break;

                    case 921600:
                        args->baudrate2 = SPABAUD_921600;
                    break;

                    default:
                        fprintf(stderr, "%s: Baudrate2 '%s' unknown\n",__func__, optarg);
                        res = 0;
                    break;
                }
            }
            break;

            case 'o':
                if(!strcmp(optarg, "5"))
                {
                    args->databits2 = SPADATABITS_5;
                }
                else if(!strcmp(optarg, "6"))
                {
                    args->databits2 = SPADATABITS_6;
                }
                else if(!strcmp(optarg, "7"))
                {
                    args->databits2 = SPADATABITS_7;
                }
                else if(!strcmp(optarg, "8"))
                {
                    args->databits2 = SPADATABITS_8;
                }
                else
                {
                    fprintf(stderr, "%s: Databits2 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'p':
                if(!strcmp(optarg, "1")) 
                {
                    args->stopbits2 = SPASTOPBITS_1;
                }
                else if(!strcmp(optarg, "2"))
                {
                    args->stopbits2 = SPASTOPBITS_2;
                }
                else
                {
                    fprintf(stderr, "%s: Stopbits2 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'q':
                i = 0;
                args->parity2 = SerialGetParity(optarg, &i);
                if(!i)
                {
                    fprintf(stderr, "%s: Parity2 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'r':
                i = 0;
                args->protocol2 = SerialGetProtocol(optarg, &i);
                if(!i)
                {
                    fprintf(stderr, "%s: Protocol2 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 's':
                args->serial3 = optarg;
            break;

            case 't':
            {
                i = 0;
                i = strtol(optarg, 0, 10);
                switch(i)
                {
                    case 50:
                        args->baudrate3 = SPABAUD_50;
                    break;

                    case 110:
                        args->baudrate3 = SPABAUD_110;
                    break;

                    case 300:
                        args->baudrate3 = SPABAUD_300;
                    break;

                    case 600:
                        args->baudrate3 = SPABAUD_600;
                    break;

                    case 1200:
                        args->baudrate3 = SPABAUD_1200;
                    break;

                    case 2400:
                        args->baudrate3 = SPABAUD_2400;
                    break;

                    case 4800:
                        args->baudrate3 = SPABAUD_4800;
                    break;

                    case 9600:
                        args->baudrate3 = SPABAUD_9600;
                    break;

                    case 19200:
                        args->baudrate3 = SPABAUD_19200;
                    break;

                    case 38400:
                        args->baudrate3 = SPABAUD_38400;
                    break;

                    case 57600:
                        args->baudrate3 = SPABAUD_57600;
                    break;

                    case 115200:
                        args->baudrate3 = SPABAUD_115200;
                    break;

                    case 230400:
                        args->baudrate3 = SPABAUD_230400;
                    break;

                    case 460800:
                        args->baudrate3 = SPABAUD_460800;
                    break;

                    case 921600:
                        args->baudrate3 = SPABAUD_921600;
                    break;

                    default:
                        fprintf(stderr, "%s: Baudrate3 '%s' unknown\n",__func__, optarg);
                        res = 0;
                    break;
                }
            }
            break;

            case 'u':
                if(!strcmp(optarg, "5"))
                {
                    args->databits3 = SPADATABITS_5;
                }
                else if(!strcmp(optarg, "6"))
                {
                    args->databits3 = SPADATABITS_6;
                }
                else if(!strcmp(optarg, "7"))
                {
                    args->databits3 = SPADATABITS_7;
                }
                else if(!strcmp(optarg, "8"))
                {
                    args->databits3 = SPADATABITS_8;
                }
                else
                {
                    fprintf(stderr, "%s: Databits3 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'v':
                if(!strcmp(optarg, "1")) 
                {
                    args->stopbits3 = SPASTOPBITS_1;
                }
                else if(!strcmp(optarg, "2"))
                {
                    args->stopbits3 = SPASTOPBITS_2;
                }
                else
                {
                    fprintf(stderr, "%s: Stopbits3 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'w':
                i = 0;
                args->parity3 = SerialGetParity(optarg, &i);
                if(!i)
                {
                    fprintf(stderr, "%s: Parity3 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'x':
                i = 0;
                args->protocol2 = SerialGetProtocol(optarg, &i);
                if(!i)
                {
                    fprintf(stderr, "%s: Protocol2 '%s' unknown\n",__func__, optarg);
                    res = 0;
                }
            break;

            case 'y':
                i = 0;
                i = strtol(optarg, 0, 10);
                if(i > 1)
                {
                    res = 0;
                }
                else
                {
                    args->capture_mode = i;
                }
            break;

            case 'z':
                i = 0;
                i = strtol(optarg, 0, 10);
                if(i > 1)
                {
                    res = 0;
                }
                else
                {
                    args->capture_timeout = i;
                }
            break;

            case 'A':
                i = 0;
                i = strtol(optarg, 0, 10);
                if(i == 0 || i > 255)
                {
                    res = 0;
                }
                else
                {
                    args->frame_num = i;
                }
            break;

            case 'B':
                i = 0;
                i = strtol(optarg, 0, 10);
                if(i > 3)
                {
                    res = 0;
                }
                else
                {
                    args->gyro_range = i;
                }
            break;

            case 'C':
                i = 0;
                i = strtol(optarg, 0, 10);
                if(i > 3)
                {
                    res = 0;
                }
                else
                {
                    args->accel_range = i;
                }
            break;

            case 'D':
                i = 0;
                i = strtol(optarg, 0, 10);
                if(i < 4 || i < 1000)
                {
                    res = 0;
                }
                else
                {
                    args->sample_rate = i;
                }
            break;

            case 'E':
                i = 0;
                i = strtol(optarg, 0, 10);
                if(i < 2)
                {
                    res = 0;
                }
                else
                {
                    args->read_mode = i;
                }
            break;

            case 'F':
                args->camera1 = optarg;
            break;

            case 'G':
                args->camera2 = optarg;
            break;

            case 'H':
                args->camera1_ctrl = optarg;
            break;

            case 'I':
                args->camera2_ctrl = optarg;
            break;

            case 'J':
                args->usb_cam_def_conf_file = optarg;
            break;

            case 'K':
                args->usb_cam_user_conf_file = optarg;
            break;

            case 'L':
                args->mipi_cam_user_conf_file = optarg;
            break;

            case 'h': 
                help = 1;
            break;

            case -1: 
            break;

            default:
                help = 1;
            break;
        }
    }while(getoptr != -1 && res);

    if(!res || help)
    {
        fprintf(stderr,
        "|===============================================================================|\n"
        "|********************************help information*******************************|\n"
        "|===============================================================================|\n"
        "| -h " LONG_OPT("--help            ") "help information                                         |\n"
        "| -a " LONG_OPT("--server          ") "the ntrip server name or address                         |\n"
        "| -b " LONG_OPT("--port            ") "the ntrip port number(default 8002)                      |\n"
        "| -c " LONG_OPT("--mount_point     ") "the requested data set or sourcetable filtering criteria |\n"
        "| -d " LONG_OPT("--user_name       ") "the ntrip user name                                      |\n"
        "| -e " LONG_OPT("--password        ") "the ntrip login password                                 |\n"
        "| -f " LONG_OPT("--serial1         ") "serial port1 for set gnss module                         |\n"
        "| -g " LONG_OPT("--baudrate1       ") "baudrate for serial port1                                |\n"
        "| -i " LONG_OPT("--databits1       ") "databits for serial port1                                |\n"
        "| -j " LONG_OPT("--stopbits1       ") "stopbits for serial port1                                |\n"
        "| -k " LONG_OPT("--parity1         ") "parity   for serial port1                                |\n"
        "| -l " LONG_OPT("--protocol1       ") "protocol for serial port1                                |\n"
        "| -m " LONG_OPT("--serial2         ") "serial port2 for get gnss module                         |\n"
        "| -n " LONG_OPT("--baudrate2       ") "baudrate for serial port2                                |\n"
        "| -o " LONG_OPT("--databits2       ") "databits for serial port2                                |\n"
        "| -p " LONG_OPT("--stopbits2       ") "stopbits for serial port2                                |\n"
        "| -q " LONG_OPT("--parity2         ") "parity   for serial port2                                |\n"
        "| -r " LONG_OPT("--protocol2       ") "protocol for serial port2                                |\n"
        "| -s " LONG_OPT("--serial3         ") "serial port3 for sync module                             |\n"
        "| -t " LONG_OPT("--baudrate3       ") "baudrate for serial port3                                |\n"
        "| -u " LONG_OPT("--databits3       ") "databits for serial port3                                |\n"
        "| -v " LONG_OPT("--stopbits3       ") "stopbits for serial port3                                |\n"
        "| -w " LONG_OPT("--parity3         ") "parity   for serial port3                                |\n"
        "| -x " LONG_OPT("--protocol3       ") "protocol for serial port3                                |\n"
        "| -y " LONG_OPT("--capture_mode    ") "image capture mmode,0:queue mode;1:event mode            |\n"
        "| -z " LONG_OPT("--capture_timeout ") "image capture timeout(ms)                                |\n"
        "| -A " LONG_OPT("--frame_num       ") "camera frame buffer num                                  |\n"
        "| -B " LONG_OPT("--gyro            ") "gyro   range,0:±250dps;1:±500dps;2:±1000dps;3:±2000dps   |\n"
        "| -C " LONG_OPT("--accel           ") "accel  range,0:±2g;1:±4g;2:±8g;3:±16g                    |\n"
        "| -D " LONG_OPT("--sample_rate     ") "sample rate ,4~1000(Hz)                                  |\n"
        "| -E " LONG_OPT("--read_mode       ") "imu read mode,0:normal;1:interrupt;2:timer;              |\n"
        "| -F " LONG_OPT("--camera1         ") "mipi camera1 name;                                       |\n"
        "| -G " LONG_OPT("--camera2         ") "mipi camera2 name;                                       |\n"
        "| -H " LONG_OPT("--camera1_ctr     ") "mipi camera1 controller name;                            |\n"
        "| -I " LONG_OPT("--camera2_ctrl    ") "mipi camera2 controller name;                            |\n"
        "| -J " LONG_OPT("--usb_cam_def     ") "usb  camera default config file name;                    |\n"
        "| -K " LONG_OPT("--usb_cam_user    ") "usb  camera user    config file name;                    |\n"
        "| -L " LONG_OPT("--mipi_cam_user   ") "mipi camera default config file name;                    |\n"
        "|===============================================================================|\n"
        );

        exit(1);
    }

    return res;
}
