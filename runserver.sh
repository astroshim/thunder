#!/bin/sh

# inittab�� �̿��Ҷ���, ���� �⵿�� ������ �߸� 5�� �Ŀ� ����� �ȴ�.
# /var/log/message �� ���� �αװ� ���Եȴ�.
#pid=$(ps -ef |grep -v grep | grep "DNServer" |awk {'print $2'})
pid=$(ps -ef |grep -v grep | grep "DNServer" | head -n 1 | awk {'print $8'})
if [[ $pid != "./DNServer" ]]
then
	cd /home/wizmaster/DownloadServer
	./DNServer &

	sleep 1
	ps -e | grep DNServer
fi
