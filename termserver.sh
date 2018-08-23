pid=`ps -ef|grep ChatServer |grep -v grep | awk {'print $2'}`
echo $pid
kill -9 $pid
sleep 1
ps -e | grep ChatServer

