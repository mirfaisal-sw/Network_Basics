#!/bin/bash

declare -a PARTITION_NAMES=("APERS" "PATHO")
declare -a PARTITION_NUMBS=("5" "10")

function check_mount_failure
{
	ret=0
	mount  | grep "ubi0:$1" 
	RESULT=$?

	if [ $RESULT -ne 0 ]
	then
		echo "UBI Error >>  ubi:$1 / ubi:$2 mount failure"
		ret=1
	else
		echo "ubi:$1 / ubi:$2 mount found, no need for recovery"
	fi

	return $ret
}

function check_ubierror_fix
{ 
	dmesg | grep "UBIFS error (ubi0:$2" | grep "ubifs_read_node: bad node*.*"
	RESULT=$?

	if [ $RESULT -eq 0 ]; then
		echo "Detected ubi ubi0:$2 failure from dmesg"
		ubiupdatevol -t /dev/ubi0_$2       #//Clean up the UBI volume (volume will be truncated with empty data)
		mkfs.ubifs /dev/ubi0_$2            #// Format volume as UBIFS

		if [ $? -ne 0 ]; then
			echo "Recovery Unsuccessful"
		else
			echo "Recovery Successful"
		fi

	else
		echo "No UBI ubi0:5 mount failure" 
	fi
}

function main
{
	for ((index=0; index<${#PARTITION_NAMES[@]}; index++))
	do
		check_mount_failure ${PARTITION_NAMES[index]} ${PARTITION_NUMBS[index]}
		if [ $? -ne 0 ]
		then
			check_ubierror_fix ${PARTITION_NAMES[index]} ${PARTITION_NUMBS[index]}
		fi
	done
}

main
