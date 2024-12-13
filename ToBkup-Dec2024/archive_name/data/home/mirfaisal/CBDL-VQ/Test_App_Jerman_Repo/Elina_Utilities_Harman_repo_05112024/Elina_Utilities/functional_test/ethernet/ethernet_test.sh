#!/bin/bash
t_passed=0
t_failed=0
tot_test=10

#---------------DEFAULT-INPUTS---------------
interface=eth1
interface_ip=192.168.2.1
vnet_int=vnet23_0
vnet_ip=10.23.0.10
vlan_interface_ip1=192.168.3.1
target_bitrate=900
#--------------------------------------------

vnet_interface()
{
	INTERFACE_HEADER="\n\n---------TEST3:VNET_PINGCHECK----------\n\n"
	echo -e $INTERFACE_HEADER

	echo -e "\nTo check pings via vnet interface\n"
	echo -e "\nPlease set SYS to IVI network configurations including IP and subnet"
	echo -e "Verify whether new rule has been applied for routing policy in main routing table."
	echo -e "else exit and run "ip rule add pref 0 table main" in IVI side"
	sleep 3




 echo "Your vnet interface is: $vnet_int"
 echo "Your vnet IP is: $vnet_ip"
 echo -e "Pinging IVI via vnet interface.."
 ping=$(timeout 15 ping -c 1 -I $vnet_int  $vnet_ip | grep bytes | wc -l)
 if [ "$ping" -gt 1 ];then
	echo "PING IS SUCCESSFUL"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo -e "\n TEST: SUCCESS" >> TEST_LOGS.txt
	echo -e "\n TEST: SUCCESS"
	((t_passed++))

 else
	echo "PING FAILED"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo "TEST: FAILED" >> TEST_LOGS.txt
	echo "TEST: FAILED"
	((t_failed++))

 fi



}

vlantest()
{
	INTERFACE_HEADER="\n\n-----------TEST6:VLAN_PINGCHECK-------------\n\n"
	echo -e $INTERFACE_HEADER
	ip link add link $interface name $interface.73 type vlan id 73
	ifconfig eth1.73 192.168.3.1 up


	echo -e "\nTo check pings via VLAN ID 73\n"
	echo -e "\nPlease set peer to peer network configurations" 
	echo -e "including host IP, VLAN ID and subnet on Host"

	echo "Enter interface IP to ping(press enter to use default:192.168.3.5)"
	if read -t 5 vlan_interface_ip;then
		echo -e "Your interface IP is:$vlan_interface_ip\n"
	else
		echo -e "No input received. Using default value.\n"
		vlan_interface_ip=192.168.3.5
	fi




	echo -e "\nChecking ping to host"

	ping=$(timeout 15 ping -c 1 $vlan_interface_ip | grep bytes | wc -l)
	if [ "$ping" -gt 1 ];then
		echo "HOST is UP"
		echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
		echo -e "\n TEST: SUCCESS" >> TEST_LOGS.txt
		echo -e "\n TEST: SUCCESS"
		((t_passed++))

	else
		echo "HOST is DOWN"
		echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
		echo "TEST: FAILED" >> TEST_LOGS.txt
		echo "TEST: FAILED"
		((t_failed++))

	fi


}

vlantest_incoming()
{

	INTERFACE_HEADER="\n\n----------TEST7:VLAN_PING_INCOMING-----------\n\n"
	echo -e $INTERFACE_HEADER


# Checking for any Ping from Host to target
echo -e "\nTo check incoming pings\n"
echo -e "\nPlease set peer to peer network configurations."
echo -e "including host IP, VLAN ID and subnet"

#echo "Enter interface IP to ping(press enter to use default:192.168.3.1)"



echo "Your interface ip is: $vlan_interface_ip1"
echo -e "\nChecking incoming pings\n"
echo -e "Ping manually to $vlan_interface_ip from host"
sleep 5
iping=$(timeout 10 tcpdump -i $interface -c 5 -v dst $vlan_interface_ip1 | grep length | wc -l)

if [ "$iping" -gt 1 ];then
	echo "Incoming ping detected"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo -e "\n TEST: SUCCESS" >> TEST_LOGS.txt
	echo -e "\n TEST: SUCCESS"
	((t_passed++))

else
	echo "No incoming ping"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo "TEST: FAILED" >> TEST_LOGS.txt
	echo "TEST: FAILED"
	((t_failed++))

fi

}
iperf3_host_to_target()
{
	INTERFACE_HEADER="\n\n---------TEST8:IPERF3(HOST->TARGET)---------\n\n"
	echo -e $INTERFACE_HEADER
	# Perform iperf3 test from Host to target
	echo -e "\nTo perform iperf3 test from host to target."
	echo -e "Verify if host is at iperf version 3\n"



echo -e "\nRun iperf3 -c <IP> on host."
echo -e "(use -u for UDP traffic and -b <BANDWIDTH>: flags if required).."
sleep 5
echo -e "NOW\n"
iperf3_output=$(timeout 12 iperf3 -s)
echo "$iperf3_output"
# Extract the measured bitrate from the output
measured_bitrate=$(echo $iperf3_output |sed -nE 's/^.*\s([[:digit:]]+\s*Mbits\/sec).*$/\1/p' | awk '{print $1}')


# Print the results
echo "Measured bitrate: $measured_bitrate Mbits/sec"
if ((measured_bitrate >= target_bitrate)); then
	echo "Test SUCCESS: measured bitrate is above $target_bitrate Mbits/sec"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo -e "\nTest SUCCESS:measured bitrate is above $target_bitrateMbits/sec\n">>TEST_LOGS.txt
	echo -e "\n TEST: SUCCESS" >> TEST_LOGS.txt
	((t_passed++))  # Success
else
	echo "Test FAILED: measured bitrate is below $target_bitrate Mbits/sec"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo -e "\nTest FAILED:measured bitrate is below $target_bitrateMbits/sec\n">>TEST_LOGS.txt
	echo -e "\n TEST: FAILED" >> TEST_LOGS.txt
	((t_failed++))  # Failure
fi


}

iperf3_target_to_host()
{
	INTERFACE_HEADER="\n\n----------TEST9:IPERF3(TARGET->HOST)----------\n\n"
	echo -e $INTERFACE_HEADER
	# Perform iperf3 test from Target to Host
	echo -e "\nTo perform iperf3 test from target to host."
	echo -e "(Verify if host is at iperf version 3)\n"
	#echo -e "\nPlease set peer to peer network configurations"




echo -e "\nRun iperf3 -c <IP> -R on host."
echo -e "(use -u for UDP traffic and -b <BANDWIDTH>: flags if required).."
sleep 5
echo -e "NOW\n"
iperf3_output=$(timeout 12 iperf3 -s)
echo "$iperf3_output"
# Extract the measured bitrate from the output
measured_bitrate=$(echo $iperf3_output |sed -nE 's/^.*\s([[:digit:]]+\s*Mbits\/sec).*$/\1/p' | awk '{print $1}')


# Print the results
echo "Measured bitrate: $measured_bitrate Mbits/sec"
if ((measured_bitrate >= target_bitrate)); then
	echo "Test SUCCESS: measured bitrate is above $target_bitrate Mbits/sec"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo -e "\nTest SUCCESS:measured bitrate is above $target_bitrateMbits/sec\n">>TEST_LOGS.txt
	echo -e "\n TEST: SUCCESS" >> TEST_LOGS.txt
	((t_passed++))  # Success
else
	echo "Test FAILED: measured bitrate is below $target_bitrate Mbits/sec"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo -e "\nTest FAILED:measured bitrate is below $target_bitrateMbits/sec\n">>TEST_LOGS.txt
	echo -e "\n TEST: FAILED" >> TEST_LOGS.txt
	((t_failed++))  # Failure
fi

}

pingincoming()
{
	INTERFACE_HEADER="\n\n------------TEST4:PING_INCOMING-------------\n\n"
	echo -e $INTERFACE_HEADER
	ifconfig eth1 192.168.2.1 up
	# Checking for any Ping from Host to target
	echo -e "\nTo check incoming pings\n"
	echo -e "\nPlease set peer to peer network configurations."
       echo -e "including host IP and subnet"




echo -e "\nChecking incoming pings\n"
echo -e "Ping manually to $interface_ip from host"
sleep 5
iping=$(timeout 10 tcpdump -i $interface -c 5 -v dst $interface_ip | grep length | wc -l)

if [ "$iping" -gt 1 ];then
	echo "Incoming ping detected"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo -e "\n TEST: SUCCESS" >> TEST_LOGS.txt
	echo -e "\n TEST: SUCCESS"
	((t_passed++))

else
	echo "No incoming ping"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo "TEST: FAILED" >> TEST_LOGS.txt
	echo "TEST: FAILED"
	((t_failed++))

fi


}



pingcheck()
{

	INTERFACE_HEADER="\n\n-------------TEST5:PINGCHECK---------------\n\n"
	echo -e $INTERFACE_HEADER

	echo "Enter Host IP to ping(press enter to use default:192.168.2.5)"

	if read -t 5 HOST_IP;then
		echo -e "Your interface ip is: $HOST_IP\n"
	else
		echo -e "No input received. Using Default value.\n"
		HOST_IP=192.168.3.5
	fi

	echo -e "\nChecking ping to host"

	ping=$(timeout 15 ping -c 1 $HOST_IP | grep bytes | wc -l)
	if [ "$ping" -gt 1 ];then
		echo "HOST is UP"
		echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
		echo -e "\n TEST: SUCCESS" >> TEST_LOGS.txt
		echo -e "\n TEST: SUCCESS"
		((t_passed++))

	else
		echo "HOST is DOWN"
		echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
		echo "TEST: FAILED" >> TEST_LOGS.txt
		echo "TEST: FAILED"
		((t_failed++))

	fi

}

verify_interface()
{

	ip link set dev $interface up
	ip link set dev $interface down
	ip link set dev $interface up
	sleep 1
	ip_output=$(ip a s "$interface" | grep 'state UP')

	if [[ -z "$ip_output" ]]; then
		echo -e "\n\n----------TEST1:INTERFACE-VERIFICATION----------------\n\n"
		echo -e "\nInterface $interface is down.\n"
		INTERFACE_HEADER="\n\n--------TEST1:INTERFACE-VERIFICATION--------\n\n"
		echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
		echo "Interface $interface is up." >> TEST_LOGS.txt
		echo "TEST: FAILED" >> TEST_LOGS.txt
		echo "TEST: FAILED"
		((t_failed++))



	else
		echo -e "\n\n------------TEST1:INTERFACE-VERIFICATION--------------\n\n"
		echo -e "\nInterface $interface is up."
		#dmesg | grep $interface
		INTERFACE_HEADER="\n\n------------TEST1:INTERFACE-VERIFICATION-------------\n\n"
		echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
		echo "Interface $interface is up." >> TEST_LOGS.txt
		echo "TEST: SUCCESS" >> TEST_LOGS.txt
		echo "TEST: SUCCESS"
		((t_passed++))


	fi


	INTERFACE_HEADER="\n\n------------DMESG-LOGS-------------\n\n"

	echo -e $INTERFACE_HEADER >> NETWORK_INTERFACE_LOGS.txt
	dmesg | grep $interface >> NETWORK_INTERFACE_LOGS.txt

}



mac_address()
{


	INTERFACE_HEADER="\n\n---------TEST2:MAC_ADDRESS-VERIFICATION------------\n\n"
	echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
	echo -e $INTERFACE_HEADER
	echo -e "\n$interface:" >> TEST_LOGS.txt
	path=$(pwd)
	cd /sys/class/net/$interface
	op=$(cat address)
	echo -e "\n$interface:"
	echo -e "MAC Address:" $op
	if [[ -z "$op" ]]; then
		cd
		cd $path
		echo "TEST: FAILED" >> TEST_LOGS.txt
		echo -e "\nTEST: FAILED"
		((t_failed++))


	else
		cd
		cd $path
		echo $op >> TEST_LOGS.txt
		echo "TEST: SUCCESS" >> TEST_LOGS.txt
		echo -e "\n TEST: SUCCESS"
		((t_passed++))
		timestamp=$(date +"%Y-%m-%d %T")


	fi
}

network_interface_statistics()
{
	INTERFACE_HEADER="\n\n-----------NETWORK-INTERFACE-STATS----------\n\n"
	echo -e $INTERFACE_HEADER
	head -2 /proc/net/dev
	grep $interface /proc/net/dev


	echo -e $INTERFACE_HEADER >> NETWORK_INTERFACE_LOGS.txt
	head -2 /proc/net/dev >> NETWORK_INTERFACE_LOGS.txt
	grep $interface /proc/net/dev >> NETWORK_INTERFACE_LOGS.txt

}

cleanup()
{

	rm -rf NETWORK_INTERFACE_LOGS.txt
	rm -rf TEST_LOGS.txt
	rm -rf KPI_MEASUREMENT.txt
}


kpi_measurement()
{
	INTERFACE_HEADER="\n\n-----------TEST10:KPI---------\n\n"

	echo -e $INTERFACE_HEADER

	INTERFACE_HEADER="\n\n----------DMESG-LOGS-----------\n\n"

	echo -e $INTERFACE_HEADER >> KPI_MEASUREMENT.txt

	dmesg | grep -I $interface >> KPI_MEASUREMENT.txt

	output=$(grep -i "link is up" KPI_MEASUREMENT.txt)

	INTERFACE_HEADER="\n\n----------SYSTEMD-ANALYZE_BLAME-----------\n\n"

	echo -e $INTERFACE_HEADER >> KPI_MEASUREMENT.txt

	systemd-analyze blame | grep -C 3 -E 'network|net' >> KPI_MEASUREMENT.txt
	echo -e "\n SYSTEMD-ANALYZE BLAME\n"
	systemd-analyze blame | grep -C 3 -E 'network|net'

	if [[ -z "$output" ]]; then
		echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
		echo -e "\nTEST FAILED:KPI" >> TEST_LOGS.txt
		echo -e "\nTEST FAILED:KPI"
		((t_failed++))
	else
		echo -e $INTERFACE_HEADER >> TEST_LOGS.txt
		echo -e "\nTEST SUCCESS:KPI" >> TEST_LOGS.txt
		echo -e "\nTEST SUCCESS:KPI"
		((t_passed++))
	fi


}





main()
{
	cleanup
	verify_interface
	mac_address
	vnet_interface
	pingincoming
	pingcheck
	vlantest
	vlantest_incoming
	iperf3_host_to_target
	iperf3_target_to_host
	network_interface_statistics
	kpi_measurement

	echo -e "\n\n------------------TEST_CASES--------------------------\n\n"

	echo -e "Total Test Cases:"$tot_test
	echo -e "Total Test Cases Passed:"$t_passed
	echo -e "Total Test Cases Failed:"$t_failed

}
main

