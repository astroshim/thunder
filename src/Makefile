#.SUFFIXES = .c .o 
#CC     = gcc

.SUFFIXES = .cpp .o 
LDFLAGS= -Wl,--hash-style=sysv
CC     = g++  -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_ONESHOT -D_CLIENT_ARRAY
#CC     = g++  -D_REENTRANT -D_FILE_OFFSET_BITS=64 

CFLAGS = -g -Wall 
#CFLAGS = -O2 -g

BASE		= $(HOME)/thunder/src
BIN			= $(BASE)

######################################################################
####### Object definition
######################################################################
OBJS = 	CircularBuff.o		\
		Client.o			\
		ClientUserDN.o		\
		ClientServer.o		\
		ClientDN.o			\
		ClientSocket.o		\
		IOMP_Select.o		\
		IOMP_EPoll.o		\
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
		ThreadTic.o			\
		ThreadQoS.o			\
		ThreadQoSEPoll.o	\
		CircularQueue.o		\
		ReleaseSlot.o		\
		DownloadManager.o	\
		DownloadServer.o				
#		ThreadTicDN.o		\
#		ThreadTicDNMgr.o	\
#		ClientUserMgr.o		\
#		SendQueue.o			\
#		FDQueue.o			\
#		EventQueue.o		\
#		ClientMember.o		\
#		ThreadCloser.o		\
#		CloseQueue.o		\
#		UniqueID.o			\
#		ProcessManager.o	\
#		ThreadAcceptor.o	\

SRCS      	= $(OBJS:.o=.cpp) 

######################################################################
####### Compile definition
######################################################################
TARGET = Blank	# object 파일을 만들기 위해서 임시로 Blank 를 만든다.

all : $(TARGET) 

$(TARGET) : $(OBJS)
#	$(CC) $(CFLAGS) -c $(SRCS) 	# 컴파일 안해도 된다.

dep :
	gccmakedep $(SRCS)

.cpp.o:
	$(CC) $(CFLAGS) -c $<	

clean :
	rm -rf $(OBJS) $(TARGET) core 

# DO NOT DELETE
