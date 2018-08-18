#!/usr/bin/expect -f

proc auth_user { user pass } {
        global spawn_id
        set success 0
        log_user 0
        spawn ssh -l $user localhost
        set spid $spawn_id

        expect -i $spid \
                  "word: "      {
                                exp_send -i $spid "${pass}\r"
                                if { $success == 0 } {
                                        incr success -1
                                        exp_continue
                                }
                } "> "  {
                        exp_send -i $spid "exit\r"
                        set success 1
                } "continue connecting (yes/no)? "      {
                        exp_send -i $spid "yes\r"
                        exp_continue
                } "incorrect"   {
                        set success -4
                        exp_send -i $spid "\r"
                } "try again."  {
                        set success -4
                        exp_send -i $spid "\r"
                } timeout       {
                        set success -1
                }

        exp_close -i $spid
        exp_wait
        puts stderr authuser:$success
        return [ expr ( $success == 1 ) ? 1 : 0 ]
}

# function call
#	auth_user sunbee sim1234

# Program Start
set FileToRead ssh.ini
puts "File To Read is $FileToRead\r"

if { [file exists $FileToRead] } \
{
   puts "$FileToRead exists\r"
} else \
{
   puts "$FileToRead does not exist\r"
}

set fp [open $FileToRead]
while {-1 != [gets $fp line]} {
    # # 표시는 주석이다.
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
        set ssh_command [string range $tmp 0 end]

        set command "ssh -p $port -l $id $ip $ssh_command"
		eval spawn $command
		expect { 
			stty -echo 
			"*yes/no*" { send "yes\r"; exp_continue } 
			"*assword:" { send "$passwd\r"; exp_continue } 
			stty echo   
		} 
	}
}
close $fp

puts "Ending Program"
