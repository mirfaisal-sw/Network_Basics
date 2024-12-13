#!/bin/bash

# ------------------------------------------------------------------
#	[Description] This script generates a script to partition flash based on the
#	configuration file.

#	[Author] Sathishkumar Sundararaju
#	[Email]	 sathishkumar.sundararaju@harman.com	
# ------------------------------------------------------------------

KILO=$((1024))
MEGA=$(($KILO*1024))
GIGA=$(($MEGA*1024))
PARTITION_BASE=$((8*1024))
EPT_BASE=0
PARTITION_TABLE_SZ=$((1*1024))
EPT_SZ=$((8*1024))
DEVICE="/dev/mmcblk0"
#DEVICE="test_file"

EXTENDED_PART=$(($TOTAL-$PRIMARY_1-$PRIMARY_2-$PRIMARY_3-$PARTITION_BASE))

RESCAN=bin/rescan
PART_CONF=""
IMAGE=emmc

while getopts "c:" opt; do
	case $opt in
	c)
	PART_CONF=$OPTARG
	#echo "Config file: $PART_CONF"
	;;
	\?)
		exit 1
	;;
	esac
done

if [ -z ${PART_CONF} ]
then
	printf "\n\tUsage:
		./generate-flash-script.sh -c config/ford-v2.conf\n\n"
	exit
fi

if [ ! -e "$PART_CONF" ]
then
	echo "Config file does not exist: $PART_CONF"
	exit
fi

. ./$PART_CONF

touch $IMAGE
truncate --size=${TOTAL} $IMAGE

SECTOR_SIZE=$((512))
S_PARTITION_BASE=$(($PARTITION_BASE/SECTOR_SIZE))
S_MEGA=$(($MEGA/$SECTOR_SIZE))
S_PARTITION_TABLE_SZ=$(($PARTITION_TABLE_SZ / $SECTOR_SIZE))
S_EPT_SZ=$(($EPT_SZ / $SECTOR_SIZE))
S_TOTAL=$(($TOTAL / $SECTOR_SIZE))

S_PRIMARY_1=$(($PRIMARY_1 / $SECTOR_SIZE))
S_PRIMARY_2=$(($PRIMARY_2 / $SECTOR_SIZE))
S_PRIMARY_3=$(($PRIMARY_3 / $SECTOR_SIZE))

S_EXTENDED_PART=$(($EXTENDED_PART / $SECTOR_SIZE))
S_EXT5=$(($EXT5 / $SECTOR_SIZE))
S_EXT6=$(($EXT6 / $SECTOR_SIZE))
S_EXT7=$(($EXT7 / $SECTOR_SIZE))
S_EXT8=$(($EXT8 / $SECTOR_SIZE))
S_EXT9=$(($EXT9 / $SECTOR_SIZE))
S_EXT10=$((EXT10 / $SECTOR_SIZE))
S_EXT11=$(($EXT11 / $SECTOR_SIZE))
S_EXT12=$(($EXT12 / $SECTOR_SIZE))
S_EXT13=$(($EXT13 / $SECTOR_SIZE))
S_EXT14=$(($EXT14 / $SECTOR_SIZE))
S_EXT15=$(($EXT15 / $SECTOR_SIZE)) 
S_EXT16=$(($EXT16 / $SECTOR_SIZE))
S_EXT17=$(($EXT17 / $SECTOR_SIZE))
S_EXT18=$(($EXT18 / $SECTOR_SIZE))
S_EXT19=$(($EXT19 / $SECTOR_SIZE))
S_EXT20=$(($EXT20 / $SECTOR_SIZE))
S_EXT21=$(($EXT21 / $SECTOR_SIZE))
S_EXT22=$(($EXT22 / $SECTOR_SIZE))
S_EXT23=$(($EXT23 / $SECTOR_SIZE))
S_EXT24=$(($EXT24 / $SECTOR_SIZE))

partition_flash_f()
{
	local n=$(($1))
	local start_sec=$S_PARTITION_BASE
	local part_size=0
	local end_sec=0
	local sectors=0
	local TYPE

	parted -s $IMAGE unit s mklabel msdos
	TYPE=primary
	for i in 1 2 3
	do
		eval sectors=\${S_PRIMARY_${i}}
		eval end_sec=$(($start_sec + $sectors - 1))
		parted -s $IMAGE unit s mkpart $TYPE ${start_sec}s ${end_sec}s
		start_sec=$(($end_sec+1))
	done
	sectors=$S_EXTENDED_PART
	end_sec=$((S_TOTAL-1))
	TYPE=extended
	parted -s $IMAGE unit s mkpart $TYPE ${start_sec}s ${end_sec}s
	parted -s $IMAGE unit s toggle 4 lba

	start_sec=$(($start_sec+$S_PARTITION_BASE))
	TYPE=logical
	for i in $(seq 5 $n)
	do
		eval sectors=\${S_EXT${i}}
		if [ $sectors -eq 0 ]
		then
			break
		fi
		eval end_sec=$(($start_sec + $sectors - 1))
		parted -s $IMAGE unit s mkpart $TYPE ${start_sec}s ${end_sec}s
		eval SEPT_E${i}=\$\(\(\${start_sec}-\${S_PARTITION_BASE}\)\)
		#eval echo \$SEPT_E${i}
		start_sec=$(($end_sec+1+$S_PARTITION_BASE))
	done
}

print_layout_f() {
	local n=$(($1))
	#local start_sec=$S_PARTITION_BASE
	local start_sec=$S_PARTITION_BASE
	local part_size=0
	local end_sec=0
	local sectors=0

	printf "\n\t\tFlash Memory Layout [%dMB %ds] [%d partitions]\n\n" \
			$(($TOTAL/$MEGA)) $S_TOTAL $n
	printf "\t%-20s %13s %20s %10s %10s\n"\
			"Partition" "Size" "Sectors" "Start" "End"
	
	for i in 1 2 3
	do
		eval sectors=\${S_PRIMARY_${i}}
		eval end_sec=$(($start_sec + $sectors - 1))
		printf "\t%-20s %10d %2s %20d %10d %10d\n" "Primary_$i" \
				$((${sectors}*${SECTOR_SIZE}/$KILO)) "KB" ${sectors} $start_sec $end_sec
		start_sec=$(($end_sec+1))
	done

	sectors=$S_EXTENDED_PART	
	end_sec=$((S_TOTAL-1)) 
	printf "\t%-20s %10d %2s %20d %10d %10d\n" "Extended" \
		$((${sectors}*${SECTOR_SIZE}/$MEGA)) "MB" ${sectors} $start_sec $end_sec

	start_sec=$(($start_sec+$S_PARTITION_BASE))
	for i in $(seq 5 $n)
	do
		eval sectors=\${S_EXT${i}}
		if [ $sectors -eq 0 ]
		then
			break
		fi
		eval end_sec=$(($start_sec + $sectors - 1))
		printf "\t%-20s %10d %2s %20d %10d %10d\n" "Extended_$i" \
				$((${sectors}*${SECTOR_SIZE}/$KILO)) "KB" ${sectors} $start_sec $end_sec
		start_sec=$(($end_sec+1+$S_PARTITION_BASE))
	done

}

get_partition_table_f()
{
	printf "\n"
	local n=$(($1))
	local sectors=0
	local sector=0

	#echo "dd if=PPT of=${DEVICE} bs=$SECTOR_SIZE  count=2"
	dd if=$IMAGE of=results/PPT bs=$SECTOR_SIZE  status=none count=2
	for i in $(seq 5 $n)
	do
		eval sectors=\${S_EXT${i}}
		if [ $sectors -eq 0 ]
		then
			break
		fi
	eval sector=\$SEPT_E${i}
	#echo "dd if=SEPT_E${i} of=${DEVICE} bs=$SECTOR_SIZE seek=${sector} count=$S_PARTITION_BASE"
	dd if=$IMAGE of=results/SEPT_E${i} status=none bs=$SECTOR_SIZE skip=${sector} count=$S_PARTITION_BASE
	done
	printf "\n"
}

mkdir -p results
rm -rf results/*
print_layout_f 24
partition_flash_f 24
get_partition_table_f 24
fdisk -l $IMAGE
rm $IMAGE
cp $RESCAN ./results
truncate --size=0 results/flash_binaries.sh
START_E7=$(($SEPT_E7+$S_PARTITION_BASE))
cat <<EOF >> results/flash_binaries.sh
#!/bin/sh
cat /proc/mounts | grep mmc
ret=\`echo \$?\`
if [ \$ret -eq 0 ]
then
	printf "\n\t Umount all mmc partitions before re-partitioning\n"
	exit
fi
printf "Flashing partition tables ...\n"
dd if=PPT of=${DEVICE} bs=512  count=1
EOF
for i in $(seq 5 24)
do
	eval sectors=\${S_EXT${i}}
	if [ $sectors -eq 0 ]
	then
		break
	fi
	eval sector=\$SEPT_E${i}
	echo "dd if=SEPT_E${i} of=${DEVICE} bs=512 seek=$sector count=16" >> results/flash_binaries.sh
done
cat <<EOF >> results/flash_binaries.sh
sync
./rescan
EOF
chmod 777 results/flash_binaries.sh

