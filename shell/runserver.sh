#!/bin/sh

# inittab�� �̿��Ҷ���, ���� �⵿�� ������ �߸� 5�� �Ŀ� ����� �ȴ�.
# /var/log/message �� ���� �αװ� ���Եȴ�.

pid=$(ps -ef |grep -v grep | grep "DNServer" |awk {'print $2'})
if [[ $pid = "" ]]
then
	cd /home/wizmaster/DownloadServer
	./DNServer &

	sleep 1
	ps -e | grep DNServer
fi

