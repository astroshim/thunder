MAKE     = make

CC      = g++

DINC = -I./include -I /usr/local/opt/openssl/include
#LDFLAGS= -Wl,--hash-style=sysv
#LIBTHREAD = -lpthread
#LIBSOCKET = -lsocket -lnsl

LOPENSSL = -lcrypto

COMLIB= -L. -lpthread -L.. -lrt -lm

OBJ  = $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6)
OBJ3 = client_call.exe
SRC3 = client_call.cpp
OBJ4 = client.exe
SRC4 = client.cpp
OBJ5 = bbs.exe
SRC5 = bbs.cpp

OBJ6 = downwho.exe
SRC6 = ShmView.cpp

## Make Library
all : clean $(OBJ)

$(OBJ3) : $(SRC3)
	$(CC) $(SRC3) -o $(OBJ3) $(LDFLAGS)

$(OBJ4) : $(SRC4)
	$(CC) $(DINC) $(SRC4) log.cpp $(COMLIB) $(LOPENSSL) -o $(OBJ4) $(LDFLAGS)

$(OBJ5) : $(SRC5)
	$(CC) $(DINC) $(SRC5) $(COMLIB) -o $(OBJ5) $(LDFLAGS)

$(OBJ6) : $(SRC6)
	$(CC) $(DINC) $(SRC6) ../src/SharedMemory.cpp log.cpp $(COMLIB) -o $(OBJ6) $(LDFLAGS)

## Clear Module
clean:
	rm -f $(OBJ)
	rm -f core

