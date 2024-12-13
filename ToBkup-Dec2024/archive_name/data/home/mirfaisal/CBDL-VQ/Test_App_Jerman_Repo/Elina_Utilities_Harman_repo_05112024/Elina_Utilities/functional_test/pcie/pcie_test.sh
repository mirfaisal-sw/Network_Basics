#!/bin/bash

#############################################################
# PCIe functional test
#
# Author: Mir.Faisal@harman.com
############################################################

t_passed=0
t_failed=0
tot_test=0

#---------------DEFAULT-INPUTS----------------------------#
pcie_ch02_sysfs_path=/sys/devices/platform/161c0000.pcie
pcie_ch03_sysfs_path=/sys/devices/platform/161c1000.pcie

pcie_ch2_bdf=0000:00:00.0
pcie_ch3_bdf=0001:00:00.0

pcie_wlan1_bdf=0000:01:00.0
pcie_wlan2_bdf=0001:01:00.0

pcie_port_vendor_id=144d
pcie_port_device_id=ecef

wlan_chip_vendor_id=12be
wlan_chip_device_id=bd31

wlan1_sysfs_path=/sys/bus/pci/devices/0000:01:00.0
wlan2_sysfs_path=/sys/bus/pci/devices/0001:01:00.0

log_file=TEST_LOGS.txt
#---------------------------------------------------------#

enumerate_both_channels()
{
	echo 1 > $pcie_ch02_sysfs_path/pcie_sysfs
        echo 1 > $pcie_ch03_sysfs_path/pcie_sysfs
}

poweroff_both_channels()
{
	echo 2 > $pcie_ch02_sysfs_path/pcie_sysfs
	echo 2 > $pcie_ch03_sysfs_path/pcie_sysfs
}

remove_wlan1_and_wlan2()
{
	echo 1 > $wlan1_sysfs_path/remove
	echo 1 > $wlan2_sysfs_path/remove
}

flush_out_both_wlans_detected()
{
	enumerate_both_channels
	poweroff_both_channels
	remove_wlan1_and_wlan2
}

check_pcie_controller_initialization()
{
	TEST_RESULT_HEADER="\n\nTEST-$((++tot_test)) : PCIe CONTROLLER INIT TEST\n"
        echo -e $TEST_RESULT_HEADER
	echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt

	if [ -d "$pcie_ch02_sysfs_path" ] && [ -d "$pcie_ch03_sysfs_path" ]; then
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST SUCCESS: PCIE CONTROLLER INIT TEST\n" >> TEST_LOGS.txt
                echo -e "\tTEST SUCCESS: PCIE CONTROLLER INIT TEST\n"
                ((t_passed++))
        else
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE CONTROLLER INIT TEST\n" >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE CONTROLLER INIT TEST\n"
                ((t_failed++))
        fi
}

# In IDCevo project, PCIe enumeration happens using command(s) invoked in init.wifi.rc.
# This test case verifies enumeration after boot-to-console due to the .
check_pcie_autopoweron_enumeration()
{
	TEST_RESULT_HEADER="\n\nTEST-$((++tot_test)) : PCIe AUTO ENUMERATION TEST\n"
        echo -e $TEST_RESULT_HEADER
	echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt

	devices_detected=$(lspci -nn | grep -I $wlan_chip_vendor_id | wc -l)

	if [[ $devices_detected -eq 2 ]]; then
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST SUCCESS: PCIE AUTO ENUM. TEST\n" >> TEST_LOGS.txt
                echo -e "\tTEST SUCCESS: PCIE AUTO ENUM. TEST\n"
                ((t_passed++))
	else
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE AUTO ENUM. TEST\n" >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE AUTO ENUM. TEST\n"
                ((t_failed++))
	fi
}

check_complete_de_enumerate()
{
	TEST_RESULT_HEADER="\n\nTEST-$((++tot_test)) : PCIe DE-ENUMERATE TEST\n"
        echo -e $TEST_RESULT_HEADER
	echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt

	#PCIe enumerate
	enumerate_both_channels

	# PCIe Power-off
	poweroff_both_channels

	# PCIe device remove
	remove_wlan1_and_wlan2

	# Verify PCIe enumeration using "lspci" that WLAN devices are removed.
        output=$(lspci -nn | grep -I $wlan_chip_vendor_id)

        if [[ -z "$output" ]]; then
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST SUCCESS: PCIE DE-ENUMERATE\n" >> TEST_LOGS.txt
                echo -e "\tTEST SUCCESS: PCIE DE-ENUMERATE\n"
                ((t_passed++))
        else
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE DE-ENUMERATE\n" >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE DE-ENUMERATE\n"
                ((t_failed++))
	fi
}

check_remove_rescan_verify()
{
	TEST_RESULT_HEADER="\n\nTEST-$((++tot_test)): PCIE REMOVE RESCAN TEST\n"
        echo -e $TEST_RESULT_HEADER
	echo -e $TEST_RESULT_HEADER >> PCIe_REMOVE_RESCAN_TEST.txt

	# Flush out both WLAN1 and WLAN2 devices
	flush_out_both_wlans_detected

	# PCIe enumerate Channel - 02 or WLAN-1
	echo 1 > /sys/devices/platform/161c0000.pcie/pcie_sysfs
	output=$(lspci -nn | grep -I $wlan_chip_vendor_id)

        if [[ -z "$output" ]]; then
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE REMOVE RESCAN TEST\n" >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE REMOVE RESCAN TEST\n"
                ((t_failed++))
		return 1
	fi

	# Remove WLAN-1 device
	echo 1 > $wlan1_sysfs_path/remove
	echo -e "\tPCIE REMOVE INVOKED\n" >> TEST_LOGS.txt

	# Verify using "lspci" that WLAN devices are removed.
	output=$(lspci -nn | grep -I $wlan_chip_vendor_id)

	if [[ -z "$output" ]]; then
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
		echo -e "\tPCIE REMOVE SUCCESS\n" >> TEST_LOGS.txt
        else
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tPCIE REMOVE FAIL\n" >> TEST_LOGS.txt
		return 1
        fi

	# Pefrom PCIe rescan
	echo 1 > /sys/bus/pci/rescan
	echo -e "\tPCIE RESCAN INVOKED\n" >> TEST_LOGS.txt

	# Verify PCIe enumeration or WIFI chip detection
	output=$(lspci -nn | grep -I $wlan_chip_vendor_id)

        if [[ -z "$output" ]]; then
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST FAIL:PCIE REMOVE RESCAN TEST\n" >> TEST_LOGS.txt
                echo -e "\tTEST FAIL:PCIE REMOVE RESCAN TEST\n"
                ((t_failed++))
                return 1
        else
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST SUCCESS:PCIE REMOVE RESCAN TEST\n" >> TEST_LOGS.txt
                echo -e "\tTEST SUCCESS:PCIE REMOVE RESCAN TEST\n"
                ((t_passed++))
        fi

	# PCIe enumerate both WLAN devices
	enumerate_both_channels
}

check_on_off_remove_on()
{
	TEST_RESULT_HEADER="\n\nTEST-$((++tot_test)) : PCIE ON-OFF-REMOVE-ON TEST\n"
	echo -e $TEST_RESULT_HEADER
	echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt

	#PCIe enumerate
	enumerate_both_channels

	# Verify PCIe enumeration or WIFI chip detection
	output=$(lspci -nn | grep -I $wlan_chip_vendor_id)

	if [[ -z "$output" ]]; then
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST FAIL:PCIE ON-OFF-REMOVE-ON TEST\n" >> TEST_LOGS.txt
                echo -e "\tTEST FAIL:PCIE ON-OFF-REMOVE-ON TEST\n"
                ((t_failed++))
                return 1
	fi

	# PCIe Power-off
	poweroff_both_channels

	# PCIe device remove
        remove_wlan1_and_wlan2

	# Verify PCIe WIFI devices removal
        output=$(lspci -nn | grep -I $wlan_chip_vendor_id)

	if [[ ! -z "$output" ]]; then
                echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE ON-OFF-REMOVE-ON TEST\n" >> TEST_LOGS.txt
                echo -e "\tTEST FAIL: PCIE ON-OFF-REMOVE-ON TEST\n"
                ((t_failed++))
                return 1
        fi

	#PCIe enumerate
        enumerate_both_channels

	# Verify PCIe WIFI devices removal
        output=$(lspci -nn | grep -I $wlan_chip_vendor_id)

	if [[ ! -z "$output" ]]; then
		echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
		echo -e "\tTEST SUCCESS: PCIE ON-OFF-REMOVE-ON TEST\n" >> TEST_LOGS.txt
		echo -e "\tTEST SUCCESS: PCIE ON-OFF-REMOVE-ON TEST\n"
		((t_passed++))
		return 1
	fi
}

check_pcie_repetive_enumeration()
{
	TEST_RESULT_HEADER="\n\nTEST-$((++tot_test)) : PCIE REPETIVE ENUM. TEST\n"
	echo -e $TEST_RESULT_HEADER
	echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt

	cnt=0
	while [ $cnt -lt 10 ]
	do
		#PCIe enumerate
		enumerate_both_channels

		# PCIe Power-off
		poweroff_both_channels

		# PCIe device remove
		remove_wlan1_and_wlan2

		#PCIe enumerate
		enumerate_both_channels

		# Verify PCIe WIFI devices removal
		output=$(lspci -nn | grep -I $wlan_chip_vendor_id)

		if [[ -z "$output" ]]; then
			echo -e $TEST_RESULT_HEADER >> TEST_LOGS.txt
			echo -e "\tTEST FAIL: PCIE REPETIVE ENUM TEST\n" >> TEST_LOGS.txt
			echo -e "\tTEST FAIL: PCIE REPETIVE ENUM TEST\n"
			((t_failed++))
			return 1
		fi

	let cnt++
	done

	echo -e "\tTEST SUCCESS: PCIE REPETIVE ENUM SUCCEEDED EVERY $cnt TIMES\n" >> TEST_LOGS.txt
	echo -e "\tTEST SUCCESS: PCIE REPETIVE ENUM SUCCEEDED EVERY $cnt TIMES\n"
	((t_passed++))
}

cleanup_log()
{
	rm -f $log_file
}

do_test()
{
	cleanup_log
	check_pcie_controller_initialization
	check_pcie_autopoweron_enumeration
	check_complete_de_enumerate
	check_remove_rescan_verify
	check_on_off_remove_on
	check_pcie_repetive_enumeration

	echo -e "Total Test Count : $tot_test\n"
	echo -e "Passed Test Count : $t_passed\n"
	echo -e "Failed Test Count : $t_failed\n"

	return 0
}
do_test
