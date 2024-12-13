#!/bin/bash

#############################################################
# Display APIX feature functional test
#
# Author: Kasthuri.Ramesh@harman.com
#############################################################

path=/sys/devices/platform/10c80000.spi/spi_master/spi9/spi9.0/
path_565t=/sys/devices/platform/10d60000.spi/spi_master/spi16/spi16.0/
lane0_status=as0_link_info/link_status
hud_lane0_status=link_info/link_status
lane0_type=as0_link_info/link_type
hud_lane0_type=link_info/link_type
lane1_status=as1_rep0_link_info/link_status
lane1_type=as1_rep0_link_info/link_type
ashell0_send_data=as0_send_data
ashell1_send_data=as1_send_data
hud_ashell_send_data=ashell_send_data
remote_apixread=as0_remote_readreg
apix_page=apix_page
apix_reg=apix_reg
apixread=apixreg_read
HUD=HUD
file_name=verification.txt
t_t=0
t_p=0
t_f=0

check_apix_link_status()
{
    echo -e "\n\nTest-$((++t_t)): APIX link status of path $2 of $1"
    if [ ! -d "$2" ]; then
        echo -e "\t Result: FAIL"
        echo -e "\t Directory doesnt exists"
        ((t_f++))
        return 1
    else
	if [ "$1" == "SST" ] && [ "$(cat $2$lane0_status)" == "connected" ] ; then
            echo -e "\t APIX link status: $(cat $2$lane0_status)"
            echo -e "\t Result: PASS"
            ((t_p++))
            return 0
	elif [ "$1" == "MST" ] && [ "$(cat $2$lane0_status)" == "connected" ] && [ "$(cat $2$lane1_status)" == "connected" ] ;  then
            echo -e "\t APIX link status of lane 0 : $(cat $2$lane0_status)"
	    echo -e "\t APIX link status of lane 1 : $(cat $2$lane1_status)"
            echo -e "\t Result: PASS"
            ((t_p++))
            return 0
	elif [ "$1" == "HUD" ] && [ "$(cat $2$hud_lane0_status)" == "connected" ] ; then
	    echo -e "\t APIX link status: $(cat $2$hud_lane0_status)"
            echo -e "\t Result: PASS"
            ((t_p++))
            return 0
        else
	    echo -e "\t Connect the display and rerun the test\t";
            echo -e "\t Result: FAIL"
            ((t_f++))
            return 1
        fi
    fi
}

check_apix_link_type()
{
    echo -e "\n\nTest-$((++t_t)): APIX link type of path $2"
    if [ ! -d "$2" ]; then
       echo -e "\t Result: FAIL"
       echo -e "\t Directory doesnt exists"
       ((t_f++))
       return 1
    else
	if [ "$1" == "SST" ] && [ "$(cat $2$lane0_type)" == "apix3" ] ; then
            echo -e "\t APIX link status: $(cat $2$lane0_type)"
            echo -e "\t Result: PASS"
            ((t_p++))
            return 0
	elif [ "$1" == "MST" ] && [ "$(cat $2$lane0_type)" == "apix3" ] && [ "$(cat $2$lane1_type)" == "apix3" ] ; then
            echo -e "\t APIX link status of lane 0 : $(cat $2$lane0_type)"
	    echo -e "\t APIX link status of lane 1 : $(cat $2$lane1_type)"
            echo -e "\t Result: PASS"
            ((t_p++))
            return 0
	elif [ "$1" == "HUD" ] && [ "$(cat $2$hud_lane0_type)" == "apix3" ] ; then
            echo -e "\t APIX link status: $(cat $2$hud_lane0_type)"
            echo -e "\t Result: PASS"
            ((t_p++))
            return 0
	else
            echo -e "\t Result: FAIL"
            ((t_f++))
            return 1
       fi
    fi
}

read_video_data_status()
{
	local video_offset=(0x121 0x122 0x123 0x124)
	local passing_count=0
	for ((count=0;count < $1;count++))
	do
	   echo 0x01 > $2$apix_page
           echo ${video_offset[$count]} > $2$apix_reg
           local result=$(cat $2$apixread)
           local video_datavalue=( $result )
           if [ $(((${video_datavalue[-1]#*:} & (1 << 3)))) != 0 ] ; then
                ((passing_count++))
           fi
	done
	echo "$passing_count $1"
}

check_video_data_lock() #pass the variant and check the video lock
{
    echo -e "\n\nTest-$((++t_t)): Video lock established status"
    case $1 in
         b1 )
	   if [ "$2" == "SST" ] ; then
		   retval=$( read_video_data_status 1 $path )
           elif [ "$2" == "MST" ] && [ ${re[2]:(-2)} == 81 ] ; then
		   retval=$( read_video_data_status 2 $path )
           elif [ "$2" == "MST" ] && [ ${re[2]:(-2)} == 02 ] ; then
		   retval=$( read_video_data_status 4 $path )
	   else
	       echo -e "\t Invalid data retrieved"
               echo -e "\t Result: FAIL"
               ((t_f++))
               return 1
           fi
           ;;
        b2 )
	   if [ "$2" == "SST" ] ; then
		   retval=$( read_video_data_status 1 $path )
           elif [ "$2" == "MST" ] ; then
		   retval=$( read_video_data_status 4 $path )
	   else
	       echo -e "\t Invalid data"
               echo -e "\t Result: FAIL"
               ((t_f++))
               return 1
           fi
           ;;
	b3 )
           if [ "$2" == "MST" ] ; then
	       retval_597=$( read_video_data_status 4 $path )
	       retval_565=$( read_video_data_status 1 $path_565t )
	       passing_count_597=$(echo "$retval_597" | cut -d' ' -f1)
               arg1_597=$(echo "$retval_597" | cut -d' ' -f2)
	       passing_count_565=$(echo "$retval_565" | cut -d' ' -f1)
               arg1_565=$(echo "$retval_565" | cut -d' ' -f2)
	       retval=$((passing_count_597 + passing_count_565))
	       displaycount=$((arg1_597 + arg1_565))
	       retval="$retval $displaycount"
           else
	       echo -e "\t Invalid data"
               echo -e "\t Result: FAIL"
               ((t_f++))
               return 1
           fi
           ;;
	*)
           echo -e "\t Invalid data retrieved"
           ;;
    esac
    passing_count=$(echo "$retval" | cut -d' ' -f1)
    displaycount=$(echo "$retval" | cut -d' ' -f2)
    if [ "$passing_count" == "$displaycount" ] ; then
          echo -e "\t Result: PASS"
          ((t_p++))
	  return 0
    else
	  echo -e "\t Result: FAIL"
          ((t_f++))
	  return 1
    fi
}

check_modeset_apix()
{
    echo -e "\n\nTest-$((++t_t)): Modes Test for Apix"
    local status_string=connected
    local mode_string=preferred
    modetest -D dpu@1 -c > $file_name
    while IFS= read -r line
    do
        case $line in
             *${status_string}*)
                 display_status=( $line ) ;;
             *${mode_string}*)
                 mode=( $line ) ;;
        esac
    done < $file_name
    if [ ! ${display_status[0]} ] && [ ! ${mode[1]} ] ; then
         echo -e "\t Result: FAIL"
         ((t_f++))
    else
         echo -e "\t Result: PASS"
         ((t_p++))
    fi
    modetest -D dpu@1 -s ${display_status[0]}:${mode[1]}
}

check_ashell_send_data()
{
    echo -e "\n\nTest-$((++t_t)): Test AShell Send data"
    echo -e "\n\tPlease login to GED4K console , type below command and press enter\t"
    echo -e "\n\tExecute echo 1 > /sys/kernel/tracing/events/inap/enable\t"
    read -n 1 key
    if [ "$key" = "" ]; then
       if [ ! -d $2 ]; then
           echo -e "\t Result: FAIL"
           echo -e "\t Directory doesnt exists"
           ((t_f++))
           return 1
       else
	 if [ "$1" == "SST" ] ; then
            echo 1111111 > $2$ashell0_send_data
            echo -e "\n\tExecute cat /sys/kernel/tracing/trace \t"
            echo -e "\t Result: PASS"
            ((t_p++))
            return 0
         elif [ "$1" == "MST" ]; then
             echo 1234567 > $2$ashell0_send_data
             echo 1111111 > $2$ashell1_send_data
             echo -e "\n\tExecute cat /sys/kernel/tracing/trace \t"
             echo -e "\t Result: PASS"
             ((t_p++))
             return 0
	 elif [ "$1" == "HUD" ] ; then
            echo 1111111 > $2$hud_ashell_send_data
            echo -e "\n\tExecute cat /sys/kernel/tracing/trace \t"
            echo -e "\t Result: PASS"
            ((t_p++))
            return 0
         else
            echo -e "\t Result: FAIL"
            ((t_f++))
            return 1
        fi
     fi
  fi
}

check_weston_video_data()
{
    echo -e "\n\nTest-$((++t_t)): Weston Video activation status"
    local weston_data=/var/log/weston.log
    local stringsearch=Output
    if [ -f $weston_data ] ; then
        while IFS= read -r line
        do
           case $line in
	     *$stringsearch*)
               weston_status=( $line )
               if [ ${weston_status[3]} == enabled ]; then
                   ((flag++))
               fi
               ;;
           esac
        done < $weston_data
   fi
   if [ ! flag ]; then
       echo -e "\t Result: FAIL"
       ((t_f++))
   else
       echo -e "\t Result: PASS"
       ((t_p++))
   fi
}

check_dp_initization_status()
{
    echo -e "\n\nTest-$((++t_t)): DP initialization by reading link training status"
    echo 0x0A > $1$apix_page
    echo 0x12A > $1$apix_reg
    local result=$(cat $1$apixread)
    local video_datavalue=( $result )
    if [ "$(((${video_datavalue[-1]#*:} & (1 << 3))))" != 0 ] && [ "$(((${video_datavalue[-1]#*:} & (1 << 2))))" != 0 ] && [ "$(((${video_datavalue[-1]#*:} & (1 << 2))))"  != 0 ] && [ "${video_datavalue[-1]#*:} & 1)" != 0 ] ; then
        echo -e "\t Result: PASS"
        ((t_p++))
    else
        echo -e "\t Result: FAIL"
        ((t_f++))
    fi
}

check_apix_upstream_status()
{
    echo -e "\n\nTest-$((++t_t)): APIX3 Upstream lane frame alignment status"
    echo 0x00 > $1$apix_page
    echo 0x116 > $1$apix_reg
    local result=$(cat $1$apixread)
    local video_datavalue=( $result )
    if [ "$(((${video_datavalue[-1]#*:} & 1 )))" != 0 ] ; then
        echo -e "\t Result: PASS"
        ((t_p++))
    else
        echo -e "\t Result: FAIL"
        ((t_f++))
    fi
}

check_apix_ashell_communication_status()
{
    echo -e "\n\nTest-$((++t_t)): APIX3 ashell communication status"
    echo 0x02 > $1$apix_page
    echo 0x99 > $1$apix_reg
    local result=$(cat $1$remote_apixread)
    local video_datavalue=( $result )
    if [ "$(((${video_datavalue[-1]#*:} & (1 << 3))))" != 0 ] && [ "$(((${video_datavalue[-1]#*:} & (1 << 1))))" != 0 ] ; then
        echo -e "\t Result: PASS"
        ((t_p++))
    else
        echo -e "\t Result: FAIL"
        ((t_f++))
    fi
}

check_apix_ashell_crc_error()
{
    echo -e "\n\nTest-$((++t_t)): APIX3 Ashell crc failure status"
    echo 0x02 > $1$apix_page
    echo 0x9A > $1$apix_reg
    local result=$(cat $1$remote_apixread)
    local video_datavalue=( $result )
    if [  "${video_datavalue[-1]#*:}" == 0x00 ] ; then
        echo -e "\t Result: PASS"
        ((t_p++))
    else
        echo -e "\t Result: FAIL"
        ((t_f++))
    fi
}

check_dp_link_bandwidth()
{
    echo -e "\n\nTest-$((++t_t)): DP link bandwidth status read link training status"
    echo 0x0A > $1$apix_page
    echo 0x129 > $1$apix_reg
    local result=$(cat $1$apixread)
    local video_datavalue=( $result )
    if [ "$(((${video_datavalue[-1]#*:} & (1 << 4))))" != 0 ] && [ "$(((${video_datavalue[-1]#*:} & (1 << 2))))" != 0 ] ; then
        echo -e "\t Result: PASS"
        ((t_p++))
    else
        echo -e "\t Result: FAIL"
        ((t_f++))
    fi
}

check_dp_hpd()
{
    echo -e "\n\nTest-$((++t_t)): DP Hot plug detection status check"
    echo 0x0A > $1$apix_page
    echo 0x12B > $1$apix_reg
    local result=$(cat $1$apixread)
    local video_datavalue=( $result )
    if [  "$(((${video_datavalue[-1]#*:} & 1 )))" != 0 ] ; then
        echo -e "\t Result: PASS"
        ((t_p++))
    else
        echo -e "\t Result: FAIL"
        ((t_f++))
    fi
}

check_ashell_packet()
{
	local inap565t_tx_flag=0
	local inap565t_rx_flag=0
	local inap597t_tx_flag=0
	local spi_slave_rx_flag=0
	local file=get_display_id
	local inap565t_dump_flag=/sys/module/inap565t/parameters/inap565t_debug
	local inap597t_dump_flag=/sys/module/inap597t/parameters/inap597t_debug
	local spi_slave_dump_flag=/sys/module/spi_slave_exynosauto_debug/parameters/spi_slave_debug
	local dump_buf=dmesg
	local inap565t_dump_file=inap565t_dump.txt
	local inap597t_dump_file=inap597t_dump.txt
	local spi_slave_dump_file=spi_slave_dump.txt
	if [ "$1" == "HUD" ] ; then
		echo -e "\n\nTest-$((++t_t)):Dump INAP565T AShell packets to dmesg buffer"
		echo -e "\n\nEnsure $file is present in board"
		read -n 1 key
		if [ "$key" = "" ]; then
			if [ -f "${file}" ] ; then
				echo -e "\n\nEnable INAP565T dump flag"
				echo 0x03 > $inap565t_dump_flag
				./$file HUD
				$dump_buf | tail -30 > $inap565t_dump_file
				tx_string="inap565t: AShell Tx Packet:"
				rx_string="inap565t: AShell Rx packet:"
				while IFS= read -r line
				do
					case $line in
						*${tx_string}*)
						((inap565t_tx_flag++))
						;;
					esac
					case $line in
						*${rx_string}*)
						((inap565t_rx_flag++))
						;;
					esac
				done < $inap565t_dump_file

				if [ $inap565t_tx_flag -gt 0 ] && [ $inap565t_rx_flag -gt 0 ]; then
					echo -e "\t Result: PASS"
					((t_p++))
				else
					echo -e "\t Result: FAIL"
					((t_f++))
				fi
			else
				echo "\t File doesnt exists. Please copy files to root directory"
			fi
		fi
	elif [ "$1" == "MST" ] ; then
		echo -e "\n\nTest-$((++t_t)):Dump INAP597T and SPI slave AShell packets to dmesg buffer"
		echo -e "\n\nEnsure $file is present in board"
		read -n 1 key
		if [ "$key" = "" ]; then
			if [ -f "${file}" ] ; then
				echo -e "\n\nEnable INAP597T dump flag"
				echo 0x01 > $inap597t_dump_flag
				./$file CID
				$dump_buf | tail -30 > $inap597t_dump_file
				tx_string="inap597t: AShell Tx packet:"
				while IFS= read -r line
				do
					case $line in
						*${tx_string}*)
						((inap597t_tx_flag++))
						;;
					esac
				done < $inap597t_dump_file

				echo -e "\n\nEnable SPI-Slave dump flag"
				echo 0x01 > $spi_slave_dump_flag
				./$file CID
				$dump_buf | tail -30 > $spi_slave_dump_file
				rx_string="exynosauto_spi_slave: AShell Rx packet:"
				while IFS= read -r line
				do
					case $line in
						*${rx_string}*)
						((spi_slave_rx_flag++))
						;;
					esac
				done < $spi_slave_dump_file

				if [ $inap597t_tx_flag -gt 0 ] && [ $spi_slave_rx_flag -gt 0 ]; then
					echo -e "\t Result: PASS"
					((t_p++))
				else
					echo -e "\t Result: FAIL"
					((t_f++))
				fi
			else
				echo "\t File doesnt exists. Please copy files to root directory"
			fi
		fi
	fi
}

cleanup_files()
{
     if [ -f "$file_name" ] ; then
         rm -f $file_name
     fi
}

verify_display_apix_module()
{
	if [ "$2" == "b3" ]; then
           check_apix_link_status $HUD $path_565t
	   check_apix_link_type $HUD $path_565t
	   check_ashell_send_data $HUD $path_565t
	   check_ashell_packet $HUD
	fi

	check_apix_link_status $1 $path
	check_apix_link_type $1 $path
	check_video_data_lock $2 $1 $3
	check_weston_video_data
	check_modeset_apix
	check_ashell_send_data $1 $path
	check_dp_hpd $path
	check_dp_link_bandwidth $path
	check_apix_ashell_crc_error $path
	check_apix_ashell_communication_status $path
	check_apix_upstream_status $path
	check_dp_initization_status $path
	check_ashell_packet $1
        cleanup_files
}

check_hardware_varient()
{
     cd /proc/device-tree
     local result=$(hexdump harman_board_rev)
     re=( $result )
     cd
     local line=$( dmesg | grep protocol | grep dp_link_training )
     display_status=( $line )
     case "${re[1]:(-2)}" in
	     "b1" | "b2" | "b3" )
		   verify_display_apix_module ${display_status[-2]} ${re[1]:(-2)} ${re[2]:(-2)} # (display_config) - (H/W variant) - (board ID) eg: MST b1 02
              ;;
	      #default case
              *)
              echo -e "\t Invalid data retrieved"
              ;;
     esac
}

check_hardware_varient

echo -e "\n\nTotal Test Count:\t" $t_t
echo -e "\n\nPass Test Count:\t" $t_p
echo -e "\n\nFail Test Count:\t" $t_f
