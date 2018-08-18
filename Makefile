#.SUFFIXES = .c .o 
#CC     = gcc

# if machine is 64 bits
#LDFLAGS= -Wl,--hash-style=sysv

.SUFFIXES = .cpp .o 
CC     = g++ -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_ONESHOT -D_CLIENT_ARRAY -D_FREEBSD -I /usr/local/opt/openssl/include

CFLAGS = -g -Wall 
#CFLAGS = -O2 -g

BASE		= $(HOME)/DownloadServer
BIN			= $(BASE)

LTHREAD = -lpthread
LCURL = -lcurl 
#CRYPTO = -lcrypto 

LIBS += $(LTHREAD) $(LCURL) $(CRYPTO)

######################################################################
####### Object definition
######################################################################
OBJ_MAIN = MainProcess.o 		\
		./src/CircularBuff.o      \
		./src/Client.o            \
		./src/ClientUserDN.o        \
		./src/ClientServer.o       \
		./src/ClientDN.o       		\
		./src/ClientSocket.o      \
		./src/IOMP_Select.o       \
		./src/IOMP_EPoll.o        \
		./src/Mutex.o             \
		./src/NPDebug.o           \
		./src/NPLog.o             \
		./src/NPUtil.o            \
		./src/Process.o           \
		./src/Properties.o        \
		./src/ServerInfoDN.o        \
		./src/ServerInfoDNMgr.o        \
		./src/ServerSocket.o      \
		./src/SharedMemory.o      \
		./src/Socket.o            \
		./src/TcpSocket.o         \
		./src/Thread.o            \
		./src/ThreadManager.o     \
		./src/ThreadAcceptor.o     \
		./src/ThreadWorker.o     \
		./src/ThreadReceiver.o    \
		./src/ThreadSender.o      \
		./src/ThreadTic.o       \
		./src/ThreadQoS.o       \
		./src/ThreadQoSEPoll.o       \
		./src/CircularQueue.o     \
		./src/ReleaseSlot.o     \
		./src/DownloadManager.o	  \
		./src/DownloadServer.o

SRCS        = $(OBJ_MAIN:.o=.cpp)

DIRS = ./src
######################################################################
####### Compile definition
######################################################################
TARGET = 	DNServer 

#all : $(TARGET)
all : comp $(TARGET)

comp:
	@for d in ${DIRS}; do \
	( /bin/echo "<< Change directory: `pwd`/$$d >>"; \
		cd $$d; \
		make -f Makefile; \
		/bin/echo " " \
	); done

$(TARGET) : $(OBJ_MAIN)
	$(CC) $(CFLAGS) $(LIBS) -o $(TARGET) $(OBJ_MAIN) $(LDFLAGS)

dep :
	gccmakedep $(SRCS)
#	/usr/bin/gccmakedep $(SRCS)

.cpp.o:
	$(CC) $(CFLAGS) -c $<	

install :
	cp $(TARGET) ./DownloadServer
	cp runserver.sh ./DownloadServer
	cp termserver.sh ./DownloadServer
	cp dstat ./DownloadServer

clean :
	rm -rf $(OBJ_MAIN) $(TARGET) core 

# DO NOT DELETE
