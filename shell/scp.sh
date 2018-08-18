#!/usr/bin/expect -f

set FileToRead scp.ini
puts "File To Read is $FileToRead\r"
if { [file exists $FileToRead] } \
{
	puts "$FileToRead exists\r"
} else \
{
   	puts "$FileToRead does not exist\r"
	exit 1
}

#set timeout 30 
set fp [open $FileToRead]
while {-1 != [gets $fp line]} {
	# #´Â ÁÖ¼®..
	set comment [string first "#" $line]
	if { $comment == -1 } {
		set idx [string first ";" $line]
		incr idx -1
		set id [string range $line 0 $idx]
		incr idx +2
		set tmp [string range $line $idx end]

		set idx [string first ";" $tmp]
		incr idx -1
		set passwd [string range $tmp 0 $idx]
		incr idx +2
		set tmp [string range $tmp $idx end]

		set idx [string first ";" $tmp]
		incr idx -1
		set ip [string range $tmp 0 $idx]
		incr idx +2
		set tmp [string range $tmp $idx end]

		set idx [string first ";" $tmp]
		incr idx -1
		set port [string range $tmp 0 $idx]
		incr idx +2
		set tmp [string range $tmp $idx end]

		set idx [string first ";" $tmp]
		incr idx -1
		set src_file [string range $tmp 0 $idx]
		incr idx +2
		set tmp [string range $tmp $idx end]

		set idx [string first ";" $tmp]
		set dst_file [string range $tmp 0 end]

#		puts "id ==> $id"
#		puts "passwd ==> $passwd"
#		puts "ip ==> $ip"
#		puts "port ==> $port"
#		puts "src_file ==> $src_file"
#		puts "dst_file ==> $dst_file"

		set command "scp -P $port $src_file $id@$ip:$dst_file"
		eval spawn $command
		expect {
            stty -echo 
            "*yes/no*" { send "yes\r"; exp_continue } 
            "*assword:" { send "$passwd\r"; exp_continue }
            stty echo   
		}
#		interact
	}
}
close $fp
puts "***** Ending SCP"


