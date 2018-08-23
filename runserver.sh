#!/bin/sh

pid=$(ps -ef |grep -v grep | grep "ChatServer" | head -n 1 | awk {'print $8'})
if [[ $pid != "./ChatServer" ]]
then
	./ChatServer &

	sleep 1
	ps -e | grep ChatServer
fi
