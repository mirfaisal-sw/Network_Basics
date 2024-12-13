#!/bin/bash

###########################################
# Boot-Info feature functional test
#
# Author: sanjay.rawat@harman.com
###########################################

boot_info_file=/proc/bootinfo
t_t=0	# total tests counter
t_p=0	# pass test counter
t_f=0	# fail test counter

# check boot-info data is valid
boot_info_valid()
{
	echo -e "\n\nTEST-$((++t_t)): Boot-Info : Valid Data"
	dmesg | grep "Boot-Info data is not Valid"
	if [ $? == 0 ]; then
		echo -e "\t Result: FAIL"
		((t_f++))
		return 1
	else
		echo -e "\t Result: PASS"
		((t_p++))
		return 0
	fi
}

# check boot-info feature is working
boot_info_functional()
{
	echo -e "\n\nTEST-$((++t_t)): Boot-Info : Featuring Working"
	if [ ! -f "$boot_info_file" ]; then
		echo -e "\t Result: FAIL"
		((t_f++))
		return 1
	else
		echo -e "\t Result: PASS"
		((t_p++))
		return 0
	fi
}

# read boot-info data
boot_info_read()
{
	echo -e "\n\nTEST-$((++t_t)): Boot-Info : Read Boot-Info Data"
	dmesg | grep "boot-info:"
	if [ $? != 0 ]; then
		echo -e "\t FAIL: Feature is not working"
		((t_f++))
		return 1
	else
		echo -e "\t Result: PASS"
		((t_p++))
		return 0
	fi
}

echo -e "################################################################################"
boot_info_valid
boot_info_functional
boot_info_read
echo -e "\n\nTotal Testcount:" $t_t
echo -e "Pass Testcount:\t" $t_p
echo -e "Fail Testcount:\t" $t_f
echo -e "Date / Time:" $(date)
echo -e "################################################################################"
