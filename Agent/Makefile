.SUFFIXES = .c .o 
CC     = gcc 
CFLAGS = -O2 -g

OBJECTS = $(OBJS) 
OBJS = Agent.o sysinfo.o log.o 

SRCS = $(OBJS:.o=.c)
TARGET = Agent.exe

all : $(TARGET)  
Agent.exe : $(OBJS)
	$(CC) -o Agent.exe $(OBJS) 
#	$(CC) -lcurl -o Agent.exe $(OBJS) 

dep :
	/usr/bin/X11/gccmakedep $(SRCS)

clean :
	rm -rf $(OBJECTS) $(TARGET) core 



