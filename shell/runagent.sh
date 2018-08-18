#!/bin/sh

pid=$(ps -ef |grep -v grep | grep "Agent.exe" |awk {'print $2'})
if [[ $pid = "" ]]
then
    cd /home/wizmaster/DownloadServer
    ./Agent.exe &

	sleep 1
    ps -ef | grep Agent.exe
fi
