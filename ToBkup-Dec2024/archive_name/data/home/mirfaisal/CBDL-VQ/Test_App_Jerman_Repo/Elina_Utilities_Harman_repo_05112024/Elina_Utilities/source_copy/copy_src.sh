#!/bin/bash

#########################################################################################################
# Author: Natesh <natesha.babu@harman.com>								#
# Date: 08/09/2017											#
# USAGE :-									#
#   ./copy_src.sh  <input file> <Destination_dir> <tar_file_name without \"tar.gz\" suffix> <Rel_tag>"	#
# USAGE - code_censor:											#
# NOTE :- $input_file = A text file that should contain the list of files to be removed from src	#
#	$Destination_dir = directory where this source needs to be copied				# 
#	$tar_file_name = src will be commpressed to tar.gz, hence provide name but not suffix tar.gz 	#
#	$Rel_tag = Release tag to which tip of the source tree will be set 				#
#########################################################################################################


#set -ex

input="$1"
DST_DIR="$2"
argc="$#"
TAR="$3"


copy_source()
{
	if [ "$argc" -ne 3 ]; then
		echo "Usage: $0 <input_file> <Destination_dir> <tar_file_name without \"tar.gz\" suffix>" 1>&2
		exit 1
	fi

	make distclean
	rm uImage* zImage*
	mkdir $DST_DIR
	cp -r ./* $DST_DIR
}

remove_files()
{
cd $DST_DIR
while IFS= read -r var
do
	skip_file=`echo $var | awk -F'/' '{print $NF}'`
	MAKE=${var%/*}

	if   [ -d "${var}" ]			# if it is directory
	then
                rm -rf "$var"
		KCONFIG="$MAKE/Kconfig"
		DEL_LINE=`echo $var | awk -F'/' '{print $NF}'`
		sed -i "/$DEL_LINE/d" $KCONFIG

		MAKE="$MAKE/Makefile"
		sed -i "/$DEL_LINE/d" $MAKE
		

	elif [ "$skip_file" == "Makefile" ] || [[ "$skip_file" =~ ".h" ]]
	then
                rm -rf "$var"
	else
		rm -rf "$var"
		echo "removed $var"
		DEL_LINE=`echo $var | awk -F'/' '{print $NF}'`
		DEL_LINE=${DEL_LINE%.*}
		MAKE="$MAKE/Makefile"	
		if [ -f "$MAKE" ]			#if Makefile exists
		then
			DEL_LINE=" $DEL_LINE.o"
			sed -i "/$DEL_LINE/d" $MAKE
		fi
	fi
done < "$input"


	TAR_DIR=`pwd`
	TAR_DIR=`echo $TAR_DIR | awk -F'/' '{print $NF}'`

	rm $0 $input "code_censor_tesla.sh" "file_list.txt" "skip_file_censor_tesla.txt"

	cd ../
	tar -zcvf "$TAR.tar.gz" $TAR_DIR
	mv "$TAR.tar.gz" $DST_DIR
}

copy_source
remove_files
