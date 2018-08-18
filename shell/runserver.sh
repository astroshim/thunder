#!/bin/sh

# inittab을 이용할때는, 서버 기동시 에러가 뜨면 5분 후에 재시작 된다.
# /var/log/message 에 에러 로그가 남게된다.

pid=$(ps -ef |grep -v grep | grep "DNServer" |awk {'print $2'})
if [[ $pid = "" ]]
then
	cd /home/wizmaster/DownloadServer
	./DNServer &

	sleep 1
	ps -e | grep DNServer
fi

