#kill -9 `ps -ef |grep "DNServer" |grep -v grep | awk {'print $2'}`
#pid=`ps -ef|grep DNServer |egrep -v vi| egrep -v tail |egrep -v grep |cut -c10-15`
pid=`ps -aux|grep DNServer |grep -v grep | awk {'print $2'}`
echo $pid
kill -9 $pid
sleep 1
ps -aux | grep DNServer

