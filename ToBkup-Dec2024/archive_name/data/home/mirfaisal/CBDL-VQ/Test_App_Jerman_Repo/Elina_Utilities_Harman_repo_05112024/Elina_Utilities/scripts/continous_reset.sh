#!/bin/sh

collect_data()
{
        #add the things needed just before reset
        dmesg >> /home/root/log.txt
}

reboot_target()
{
        /sbin/reboot -f
}

echo "Waiting 30 seconds before reboot"
dmesg -n1
sleep 30

#exit if "stop" file is present
if [ -f /home/root/stop ]; then
        exit 0
fi

#Collect required data
collect_data

#save files
sync

# restart target
reboot_target
