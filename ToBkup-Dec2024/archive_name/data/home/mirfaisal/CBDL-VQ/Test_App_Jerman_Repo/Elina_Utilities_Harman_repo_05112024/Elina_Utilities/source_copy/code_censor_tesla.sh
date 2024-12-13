#!/bin/bash

#########################################################################################################
# Author: Natesh <natesha.babu@harman.com>								#
# Date: 08/09/2017											#
# Description:-												#
#   This tool is to censor some code under a prject specific config tag   				#
# While delivering source code of one project to a customer, many file may 				#
# contain code relavent to other projects, hence censoring such code blocks				#
#													#
# USAGE - 												#
#   ./code_censor.sh  <Exclude_file_list.txt>								#
# USAGE - code_censor:											#
#   1. Run this script with a txt file as commandline arg whis id  list of source files to skip processing #
#   2. Add the CONFIG TAGS in "config_tags" array to censor code block 					#
#	under that CONFIG_TAG 										#
#   3. List of files to censor will be taken by itself by 						#
#	"git diff --name-only..." command or User can also provide by modifying				#
#	this script.											#
#########################################################################################################


#set -ex

argc="$#"
REPLACE_TXT="............................."
EXCLUDE_FILES="$1"
EXCLUDE_CENSOR="skip_file_censor_tesla.txt"
config_tags=(CONFIG_IUKRAM_HIGH CONFIG_IUKRAM_MID CONFIG_MMT2020 CONFIG_NTG6_CSB)
exclude_tags=(CONFIG_TUNERGEN2)
REL_TAG="$2"

del_filename_list()
{
while IFS= read -r var
do
	file=`echo $var | awk -F'/' '{print $NF}'`
	sed -i "/$file/d" "file_list.txt"
done < "$EXCLUDE_FILES"

while IFS= read -r var
do
        file=`echo $var | awk -F'/' '{print $NF}'`
        sed -i "/$file/d" "file_list.txt"
done < "$EXCLUDE_CENSOR"
}


code_censor()
{

	if [ "$argc" -ne 2 ]; then
		echo "Usage: $0 <Exclude_file.txt> <Rel_tag>" 1>&2
		exit 1
	fi
git stash
git checkout $REL_TAG
git pull

#Below command is to get the list of HARMAN modified files, as we need not process every file in linux source
git diff --name-only TI-PROCESSOR-SDK-LINUX-AUTOMOTIVE-DRA7XX-EVM-03.00.00.03 > file_list.txt

#filter the files which are not required to censor from the list of harman files(file_list.txt)
# or user can comment this function call to skip this process and "exit 1" at usage block above
del_filename_list

count_nest=0

for ((i = 0; i < ${#config_tags[*]}; i++))
do
	while IFS= read -r filename
	do
		LINE_NUM=0
		REPLACE=0
		sync
		echo "<<<<<<<<<<<<<<  Processing file $filename      >>>>>>>>>>>"

		while read -r LINE
		do
			NOT_DEFCONFIG=0
			LINE_NUM=$((LINE_NUM + 1))

                        if echo "$LINE" | grep -q "${config_tags[$i]}" && echo "$LINE" | grep -q "#ifndef"
                        then
                                continue
                        fi

			if echo "$LINE" | grep -q "#ifdef" || echo "$LINE" | grep -q "#if"
			then
				NOT_DEFCONFIG=1
				if [[ "$REPLACE" == 1 ]]
				then
					count_nest=$((count_nest + 1))
				fi
			elif echo "$LINE" | grep -q "#endif" || echo "$LINE" | grep -q "#else" 
			then
				if [[ "$REPLACE" == 1  &&  "$count_nest" -ge 1 ]]
                                then
                                        count_nest=$((count_nest - 1))
                                fi

				if [[ "$count_nest" == 0 ]]
				then
					REPLACE=0
				fi
			fi

			if [[ "$REPLACE" == 1 ]]
			then
				echo "line-$LINE_NUM -  $LINE"
				sed -i "${LINE_NUM}s/.*/$REPLACE_TXT/g" $filename
			fi

			if echo "$LINE" | grep -q "${config_tags[$i]}"
			then
				if [[ "$NOT_DEFCONFIG" == 1 ]]; then
					REPLACE=1
				fi
				for ((j = 0; j < ${#exclude_tags[*]}; j++))
				do
					if echo "$LINE" | grep -q "${exclude_tags[$j]}"
					then
						REPLACE=0
					fi
				done
			fi
		done < "$filename"
		sync
	done < "file_list.txt"

done
}

code_censor
