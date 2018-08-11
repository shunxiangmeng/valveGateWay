#!/bin/bash

CROSS_COMPILING_PATH = ~/Desktop/mt7688/mt7688Tools/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-uclibc-gcc-4.8.3
#VPATH
VPATH += ./

#target name
TARGET = gateWay

#lib path
LIB_PATH = -L ./

#src path
VPATH += ../slave

#CFLAG include path
CFLAG = -I./

CFLAG += -lpthread
CFLAG += -lm

#LIB += -lSi44xx

CC = $(CROSS_COMPILING_PATH)
CXX = $(CROSS_COMPILING_PATH)

RM = rm
SOURCES := $(foreach dir,$(VPATH),$(wildcard $(dir)/*))
SRCS = $(filter %.c,$(SOURCES))
CPP_SRCS = $(filter %.cpp,$(SOURCES))

C_OBJS = $(SRCS:%.c=%.o)
CPP_OBJS = $(CPP_SRCS:%.cpp=%.o)
OBJS = $(C_OBJS) $(CPP_OBJS)

$(TARGET):$(OBJS)
	$(CC) $^ $(CFLAG) $(LIB) $(LIB_PATH) -o $@
	cp $(TARGET) /private/tftpboot/
	
$(C_OBJS):%.o:%.c
	$(CC) -c $(CFLAG) $< -o $@
	
$(CPP_OBJS):%.o:%.cpp
	$(CXX) -c $(CFLAG) $< -o $@
	
clean:
	-$(RM) $(OBJS)
	-$(RM) $(TARGET)
	
	