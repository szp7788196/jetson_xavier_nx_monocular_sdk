#CROSS_COMPILE =/home/szp/tools/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
CROSS_COMPILE =
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

CFLAGS := -Wall -O2 -g
CFLAGS += -I $(shell pwd)/serial
CFLAGS += -I $(shell pwd)/net
CFLAGS += -I $(shell pwd)/ub482
CFLAGS += -I $(shell pwd)/common
CFLAGS += -I $(shell pwd)/cmd_parse
CFLAGS += -I $(shell pwd)/mpu9250
CFLAGS += -I $(shell pwd)/ui3240
CFLAGS += -I $(shell pwd)/cssc132
CFLAGS += -I $(shell pwd)/sync_module
CFLAGS += -I $(shell pwd)/led
CFLAGS += -I $(shell pwd)/sync

#LDFLAGS := -lm -lfreetype

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

TARGET := sdk

obj-y += main.o
obj-y += serial/
obj-y += net/
obj-y += ub482/
obj-y += common/
obj-y += cmd_parse/
obj-y += mpu9250/
obj-y += ui3240/
obj-y += cssc132/
obj-y += sync_module/
obj-y += led/
obj-y += sync/

all : 
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) $(LDFLAGS) -o $(TARGET) built-in.o -lpthread -lm -lueye_api -ljpeg

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	