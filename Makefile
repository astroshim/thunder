.SUFFIXES = .cpp .o
# CC     = g++ -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_ONESHOT -D_FREEBSD -D_DEBUG -I /usr/local/opt/openssl/include
CC     = g++ -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_ONESHOT -D_FREEBSD -D_DEBUG -I /usr/local/opt/openssl/include

CFLAGS = -g -Wall

BASE		= $(HOME)/ChatServer
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
		./src/ChatUser.o        \
		./src/ClientChatServer.o       \
		./src/ClientServer.o       \
		./src/ClientSocket.o      \
		./src/IOMP_Select.o       \
		./src/IOMP_KQUEUE.o        \
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
		./src/BroadcastMessage.o      \
		./src/ThreadBroadcaster.o      \
		./src/ThreadTic.o       \
		./src/CircularQueue.o     \
		./src/ReleaseSlot.o     \
		./src/ChatManager.o	  \
		./src/ChatServer.o		

SRCS        = $(OBJ_MAIN:.o=.cpp)

DIRS = ./src
######################################################################
####### Compile definition
######################################################################
TARGET = 	ChatServer

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
	# $(CC) $(CFLAGS) -c $<
	$(CC) $(CFLAGS) -c -o $@ $<

install :
	cp $(TARGET) ./ChatServer
	cp runserver.sh ./ChatServer
	cp termserver.sh ./ChatServer
	mkdir logs
	cp dstat ./ChatServer

clean :
	rm -rf $(OBJ_MAIN) $(TARGET) core

# DO NOT DELETE
