.SUFFIXES = .cpp .o 
LDFLAGS= -Wl,--hash-style=sysv
# CC     = g++  -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_ONESHOT -I /usr/local/opt/openssl/include
CC     = g++  -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_ONESHOT -I /usr/local/opt/openssl/include

CFLAGS = -g -Wall 

BASE		= $(HOME)/thunder/src
BIN			= $(BASE)

######################################################################
####### Object definition
######################################################################
OBJS = 	CircularBuff.o		\
		Client.o			\
		ChatUser.o		\
		ClientChatServer.o		\
		ClientServer.o		\
		ClientSocket.o		\
		IOMP_Select.o		\
		IOMP_KQUEUE.o		\
		Mutex.o				\
		NPDebug.o			\
		NPLog.o				\
		NPUtil.o			\
		Process.o			\
		Properties.o		\
		ServerInfoDN.o		\
		ServerInfoDNMgr.o	\
		ServerSocket.o		\
		SharedMemory.o		\
		Socket.o			\
		TcpSocket.o			\
		Thread.o			\
		ThreadManager.o		\
		ThreadAcceptor.o		\
		ThreadWorker.o		\
		ThreadReceiver.o	\
		ThreadSender.o		\
		BroadcastMessage.o		\
		ThreadBroadcaster.o		\
		ThreadTic.o			\
		CircularQueue.o		\
		ReleaseSlot.o		\
		ChatManager.o	\
		ChatServer.o			

SRCS      	= $(OBJS:.o=.cpp) 

######################################################################
####### Compile definition
######################################################################
TARGET = Blank	

all : $(TARGET) 

$(TARGET) : $(OBJS)

dep :
	gccmakedep $(SRCS)

.cpp.o:
	$(CC) $(CFLAGS) -c $<	

clean :
	rm -rf $(OBJS) $(TARGET) core 

# DO NOT DELETE
