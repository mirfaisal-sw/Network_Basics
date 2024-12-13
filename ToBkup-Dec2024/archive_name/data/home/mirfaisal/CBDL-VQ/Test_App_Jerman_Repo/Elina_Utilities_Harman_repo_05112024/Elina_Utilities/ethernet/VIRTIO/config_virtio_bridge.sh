#!/bin/bash

TEST_PC_IP=169.254.34.183
IP_3_OCTECTS=`echo $TEST_PC_IP | sed 's/\.[^.]*$//'`

ifconfig vmtap0 down
ifconfig eth0 down

brctl addbr br0
brctl addif br0 eth0
brctl addif br0 vmtap0

ifconfig br0 $IP_3_OCTECTS.188
ifconfig vmtap0 $IP_3_OCTECTS.181 up
ifconfig eth0 up

