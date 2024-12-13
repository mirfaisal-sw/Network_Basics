#!/bin/bash

#############################################################
# Display GDCN feature functional test
#
# Author: Kasthuri.Ramesh@harman.com
############################################################

path=/sys/devices/gdcn/gdcn_display/
path_597t=/sys/bus/spi/drivers/inap597t/spi9.0/
path_565t=/sys/bus/spi/drivers/inap565t/spi16.0/
gdcn_version=gdcn_client_version
gdcn_core_version=gdcn_core_version
link_status=link_info/link_status
link_type=link_info/link_type
apix_page=apix_page
apix_reg=apix_reg
apixreg_read=apixreg_read
file_name=verification.txt
t_t=0
t_p=0
t_f=0

check_gdcn_version()
{
    echo -e "\n\nTest-$((++t_t)): GDCN Version of $1 display\n"
    echo -e "Execute cat ${path}display-$1/$gdcn_version\n"
    if [ ! -d "$path" ]; then
       echo -e "\t Result: FAIL"
       echo -e "\t Directory doesn't exists"
       ((t_f++))
       return 1
    else
       local result=$(cat ${path}display-$1/$gdcn_version)
       if [ $result != 0 ]; then
          echo -e "\tGDCN Client Version: $result"
          echo -e "\tResult: PASS"
          ((t_p++))
          return 0
       else
          echo -e "\t Result: FAIL"
          ((t_f++))
          return 1
       fi
    fi
}

check_gdcn_core_protocol()
{
    echo -e "\n\nTest-$((++t_t)): GDCN Core Protocol Version of $1 display\n"
    echo -e "Execute cat ${path}display-$1/$gdcn_core_version\n"
    if [ ! -d "$path" ]; then
        echo -e "\t Result: FAIL"
        echo -e "\t Directory doesnt exists"
        ((t_f++))
        return 1
    else
        local result=$(cat ${path}display-$1/$gdcn_core_version)
        if [ $result != 0 ]; then
           echo -e "\tGDCN Core protocol Version: $result"
           echo -e "\tResult: PASS"
           ((t_p++))
           return 0
        else
           echo -e "\t Result: FAIL"
           ((t_f++))
           return 1
        fi
    fi
}

check_apix_link_status()
{
    echo -e "\n\nTest-$((++t_t)): Apix link status of $1 display\n"
    echo -e "Execute ${path}display-$1/$link_status\n"
    if [ ! -d "$path" ]; then
       echo -e "\t Result: FAIL"
       echo -e "\t Directory doesnt exists"
       ((t_f++))
       return 1
    else
      local result=$(cat ${path}display-$1/$link_status)
      if [[ $result == connected ]]; then
         echo -e "\tStatus: $result"
         echo -e "\tResult: PASS"
         ((t_p++))
         return 0
      else
         echo -e "\t Result: FAIL"
         ((t_f++))
         return 1
      fi
   fi
}

check_apix_link_type()
{
    echo -e "\n\nTest-$((++t_t)): Apix link type of $1 display\n"
	echo -e "Execute ${path}display-$1/$link_type\n"
    if [ ! -d "$path" ]; then
       echo -e "\t Result: FAIL"
       echo -e "\t Directory doesnt exists"
       ((t_f++))
       return 1
    else
       local result=$(cat ${path}display-$1/$link_type)
       if [[ $result == apix3 ]]; then
         echo -e "\tLink Type: $result"
         echo -e "\tResult: PASS"
         ((t_p++))
         return 0
       else
         echo -e "\t Result: FAIL"
         ((t_f++))
         return 1
      fi
    fi
}

check_get_display_id()
{
    echo -e "\n\nTest-$((++t_t)): Read display id from $1 display\n"
    echo -e "Execute ./get_display_id $1\n"
    local flag=0
    local filename=get_display_id
    local stringsearch=Done getting display ID
    if [ ! -f "$filename" ]; then
        echo -e "\t Result: FAIL"
        echo -e "\t File doesnt exists. Please copy get_display_id file to root directory"
        ((t_f++))
        return 1
    else
        local result=$(./"$filename" $1)
        echo "$result" > $file_name
	echo -e "\n$result"
        while IFS= read -r line
        do
        case $line in
             *${stringsearch}*)
             ((flag++))
             ;;
        esac
        done < $file_name
    fi

    if [ $flag != 0 ]; then
       echo -e "\tResult: PASS"
       ((t_p++))
    else
       echo -e "\tResult; FAIL"
       ((t_f++))
    fi
}

check_video_data_on_connected_display()
{
    echo -e "\n\nTest-$((++t_t)): Activate video for $1 display\n"
    echo -e "Execute ./set_video_layer_on_off $1 1 and ./set_backlight_on_off $1 1\n"
    local files=("set_video_layer_on_off" "set_backlight_on_off")
    local cmd=(set_video_layer_on_off set_backlight_on_off)
    local stringsearch=("Success to turn on video layer: ACK Seq: 1" "Success to turn on display backlight: ACK Seq: 1")
    local flag=0
    for ((i=0; i<${#files[@]}; i++));
    do
        if [ -f "${files[$i]}" ] ; then
           local result=$(./"${cmd[$i]}" $1 1)
           echo "$result" > $file_name
           while IFS= read -r line
                do
                case $line in
                     *${stringsearch[$i]}*)
                     ((flag++))
                     ;;
                esac
                done < $file_name
        else
           echo -e "\t Result: FAIL"
           echo "\t File doesnt exists. Please copy necessary file to root directory"
           ((t_f++))
           return 1
       fi
   done
   if [ $flag == 2 ]; then
       echo -e "\t Result: PASS"
       ((t_p++))
   else
       echo -e "\t Result: FAIL"
       ((t_f++))
   fi
}

read_video_data_status()
{
    echo -e "Execute following commands\n"
    echo -e "echo 0x01 > $2$apix_page\n"
    echo -e "echo $1 > $2$apix_reg\n"
    echo -e "cat $2$apixreg_read\n"
    echo 0x01 > $2$apix_page
    echo $1 > $2$apix_reg
    local result=$(cat $2$apixreg_read)
    local video_datavalue=( $result )
    if [ $(((${video_datavalue[-1]#*:} & (1 << 3)))) != 0 ] ; then
        return 0
    else
        return 1
    fi
}

check_video_data_lock()
{
    echo -e "\n\nTest-$((++t_t)): Video lock status of $1 display\n"
    local video_offset=(0x121 0x122 0x123 0x124)
    case $1 in
         CID )
           read_video_data_status ${video_offset[0]} $path_597t
           local cidres=$?
           if [ $cidres == $2 ]; then
               echo -e "\tResult for $1 display: PASS"
	       ((t_p++))
           else
               echo -e "\tResult for $1 display: FAIL"
               ((t_f++))
           fi
           ;;
         PHUD-0 )
	    read_video_data_status ${video_offset[1]} $path_597t
            local phud0res=$?
            if [ $phud0res == $2 ]; then
               echo -e "\tResult for $1 display: PASS"
               ((t_p++))
            else
               echo -e "\tResult for $1 display: FAIL"
	       ((t_f++))
	    fi
	    ;;
	 PHUD-1 )
            read_video_data_status ${video_offset[2]} $path_597t
	    local phud1res=$?
            if [ $phud1res == $2 ]; then
               echo -e "\tResult for $1 display: PASS"
	       ((t_p++))
            else
               echo -e "\tResult for $1 display: FAIL"
               ((t_f++))
            fi
            ;;
        PHUD-2 )
            read_video_data_status ${video_offset[3]} $path_597t
            local phud2res=$?
            if [ $phud2res == $2 ]; then
               echo -e "\tResult for $1 display: PASS"
               ((t_p++))
	    else
               echo -e "\tResult for $1 display: FAIL"
	       ((t_f++))
            fi
            ;;
        RSE-0 )
            read_video_data_status ${video_offset[0]} $path_597t
            local rse0res=$?
            if [ $rse0res == $2 ]; then
               echo -e "\tResult for $1 display: PASS"
               ((t_p++))
            else
               echo -e "\tResult for $1 display: FAIL"
               ((t_f++))
            fi
            ;;
	RSE-1 )
            read_video_data_status ${video_offset[1]} $path_597t
            local rse1res=$?
            if [ $rse1res == $2 ]; then
               echo -e "\tResult for $1 display: PASS"
               ((t_p++))
            else
               echo -e "\tResult for $1 display: FAIL"
               ((t_f++))
            fi
            ;;
	HUD )
            read_video_data_status ${video_offset[0]} $path_565t
            local hudres=$?
            if [ $hudres == $2 ]; then
               echo -e "\tResult for $1 display: PASS"
               ((t_p++))
            else
               echo -e "\tResult for $1 display: FAIL"
               ((t_f++))
            fi
            ;;
	CDE )
            read_video_data_status ${video_offset[0]} $path_597t
            local hudres=$?
            if [ $hudres == $2 ]; then
               echo -e "\tResult for $1 display: PASS"
               ((t_p++))
            else
               echo -e "\tResult for $1 display: FAIL"
               ((t_f++))
            fi
            ;;
    esac
}

cleanup_files()
{
     if [ -f "$file_name" ] ; then
         rm -f $file_name
     fi
}

check_touch_data_display()
{
    echo -e "\n\nTest-$((++t_t)): Check Display Touch Data"
    echo -e "\n\tPlease connect USB mouse to $1 Display and press enter"
    read -n 1 key
    if [ "$key" = "" ]; then
       echo -e "\n\tPlease move the mouse cursor"
       for n in {0..10};
       do
           local touch_index=$(cat /sys/class/input/input${n}/name)
           if [ $touch_index == gdcn_touch_adapter ]; then
              eventindex=event$n
              break
           fi
       done
       echo -e "\n\tExecute evtest /dev/input/$eventindex\n"
       evtest /dev/input/$eventindex > $file_name & _pid=$!
       sleep 5
       kill -9 "${_pid}"
    fi
    parse_the_video_data
}

parse_the_video_data()
{
    local i=0
    local t_p1=0
    local stringsearch=(ABS_MT_TRACKING_ID ABS_MT_POSITION_X ABS_MT_POSITION_Y BTN_TOUCH ABS_MT_TOUCH_MAJOR)
    if [ -f $file_name ] ; then
       while IFS= read -r line
       do
          case $line in
             *${stringsearch[0]}*)
                     ((t_p1++)) ;;
             *${stringsearch[1]}*)
                     ((t_p1++)) ;;
             *${stringsearch[2]}*)
                     ((t_p1++)) ;;
             *${stringsearch[3]}*)
                     button_touch=( $line )
                     if [ ${button_touch[-1]} == 1 ] || [ ! ${button_touch[-1]} == 0 ] ; then
                        ((t_p1++))
                     fi;;
             *${stringsearch[4]}*)
		     ((t_p1++))
          esac
        done < $file_name
    fi
    if [ $t_p1 -gt 7 ]; then
        echo -e "\t Result: PASS"
        ((t_p++))
    else
        echo -e "\t Result: FAIL"
        ((t_f++))
    fi
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
           "b1")
                  if [ ${display_status[-2]} == SST ] && [ ${re[2]:(-2)} == 02 ]; then # SST Display Configuration B1 SP21 Variant
	              verify_display_gdcn_module 1 0
                  elif [ ${display_status[-2]} == SST ] && [ ${re[2]:(-2)} == 03 ]; then # SST Display Configuration CDE Variant
	              verify_display_gdcn_module 1 7
                  elif [ ${display_status[-2]} == MST ] && [ ${re[2]:(-2)} == 81 ] ; then #MST Display Configuration B1 RSE Variant
				  verify_display_gdcn_module 2 4
				  elif [ ${display_status[-2]} == MST ] && [ ${re[2]:(-2)} == 02 ] ; then #MST Display Configuration B1 SP25 Variant
	              verify_display_gdcn_module 4 0
				  else
		          echo -e "\t Invalid data retrieved"
                  fi
                  ;;
	   "b2")
		   if [ ${display_status[-2]} == SST ] ; then # SST Display Configuration B2 SP21 Variant
	              verify_display_gdcn_module 1 0
		   elif [ ${display_status[-2]} == MST ] ; then #MST Display Configuration B1 SP25 Variant
	              verify_display_gdcn_module 4 0
		   else
		       echo -e "\t Invalid data retrieved"
                   fi
                   ;;
	   "b3")
		   if [ ${display_status[-2]} == MST ] ; then #MST Display Configuration B3 SP25 Variant
	              verify_display_gdcn_module 4 0
		      verify_display_gdcn_module 1 6
		   else
		       echo -e "\t Invalid data retrieved"
                   fi
                   ;;
	   #default case
           *)
                  echo -e "\t Invalid data retrieved"
                  ;;
     esac
}

verify_display_gdcn_module()
{
    local display_type=(CID PHUD-0 PHUD-1 PHUD-2 RSE-0 RSE-1 HUD CDE)
    for ((count=$2;count < $1+$2;count++))
       do
           check_gdcn_version  ${display_type[$count]}
           check_gdcn_core_protocol ${display_type[$count]}
           check_apix_link_status ${display_type[$count]}
           check_apix_link_type ${display_type[$count]}
	   check_get_display_id ${display_type[$count]}
           check_video_data_on_connected_display ${display_type[$count]}
           check_video_data_lock ${display_type[$count]} 0
       done
    if [ ${display_type[$2]} != "HUD" ] ; then
           check_touch_data_display ${display_type[$2]}
    fi
    cleanup_files
}

check_hardware_varient

echo -e "\n\nTotal Test Count:\t" $t_t
echo -e "\n\nPass Test Count:\t" $t_p
echo -e "\n\nFail Test Count:\t" $t_f
