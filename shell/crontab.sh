#!/bin/bash

crontab -l > /home/wizmaster/DownloadServer/.crontab.tmp
crontab -l | grep "DownloadServer" &> /dev/null
if [ "$?" != "0" ] ; then
  echo "##### DNServer (`date`... added by astrosim )#####
0-59 * * * * /home/wizmaster/DownloadServer/runserver.sh 1> /dev/null 2>&1" >> /home/wizmaster/DownloadServer/.crontab.tmp
  crontab  /home/wizmaster/DownloadServer/.crontab.tmp
#  echo "rm -f /tmp/.suidc_file_server_check" >> /etc/rc.d/rc.local
fi

######################################################
# comment inittab
######################################################
#sed '/dsm:2345:respawn:\/wizbong\/bin\/DSMgr/ s/^/#/g' /etc/inittab.ins > /etc/inittab2.ins
#mv /etc/inittab2.ins /etc/inittab.ins


#cat /etc/inittab.ins |grep "DSMgr"
#grep "DSMgr" aaa.txt &> /dev/null
#if [ "$?" != "0" ] ; then
	#sed -e '/dsm:2345:respawn:\/wizbong\/bin\/DSMgr/d' aaa.txt > aaa2.txt
	#sed 's/ddd/111/g' aaa.txt > aaa2.txt
	#sed 's/ddd/111/g' aaa.txt > aaa2.txt
	#sed -e `%s/dsm:2345:respawn:\/wizbong\/bin\/DSMgr/#dsm:2345:respawn:\/wizbong\/bin\/DSMgr/g`
#fi

