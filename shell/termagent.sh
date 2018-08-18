pid=`ps -ef|grep Agent.exe |grep -v grep | awk {'print $2'}`
echo $pid
kill -9 $pid
sleep 1
ps -e | grep Agent
