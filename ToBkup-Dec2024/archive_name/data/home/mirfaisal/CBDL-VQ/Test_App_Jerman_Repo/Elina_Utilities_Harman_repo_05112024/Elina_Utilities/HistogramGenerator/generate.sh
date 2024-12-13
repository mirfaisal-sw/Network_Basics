#!/bin/bash

if [ $# -lt 2 ]
then
	echo -e "\nUsage: $0 <cores> <file> <title>\n"
	exit
fi
NO_OF_CORES=$1
FILE_PATH=$2
FILE_NAME=$(basename "$FILE_PATH")
FILE_DIR=$(dirname "$FILE_PATH")

if [ -z $3 ]
then
	TITLE="Latency plot"
else
	TITLE="$3 $4"
fi

cp $FILE_PATH ./output
sync

# 1. Run cyclictest
#cyclictest -l100000000 -m -Sp90 -i200 -h400 -q > output 

# 2. Get maximum latency
max=`grep "Max Latencies" output | tr " " "\n" | sort -n | tail -1 | sed s/^0*//`
min=`grep "Min Latencies" output | tr " " "\n" | sort -n | tail -1 | sed s/^0*//`
avg=`grep "Avg Latencies" output | tr " " "\n" | sort -n | tail -1 | sed s/^0*//`

# 3. Grep data lines, remove empty lines and create a common field separator
grep -v -e "^#" -e "^$" output | tr " " "\t" >histogram 

# 4. Set the number of cores, for example
cores=$NO_OF_CORES

# 5. Create two-column data sets with latency classes and frequency values for each core, for example
for i in `seq 1 $cores`
do
	column=`expr $i + 1`
	cut -f1,$column histogram >histogram$i
done

# 6. Create plot command header
echo -n -e "set title \"$TITLE\"\n\
	set terminal png\n\
	set xlabel \"Latency (us), max $max\us min $min\us avg $avg\us\"\n\
	set logscale y\n\
	set xrange [0:400]\n\
	set yrange [0.8:*]\n\
	set ylabel \"Number of latency samples\"\n\
	set output \"plot_$4.png\"\n\
	plot " >plotcmd

# 7. Append plot command data references
for i in `seq 1 $cores`
do
	if test $i != 1
	then
		echo -n ", " >>plotcmd
	fi
	cpuno=`expr $i - 1`
	max=`grep "Max Latencies" output | tr " " "\n" | head -$((3 + $i)) | tail -1`
	min=`grep "Min Latencies" output | tr " " "\n" | head -$((3 + $i)) | tail -1`
	avg=`grep "Avg Latencies" output | tr " " "\n" | head -$((3 + $i)) | tail -1`

	echo "CPU$cpuno $min $max $avg"
	if test $cpuno -lt 10
	then
		title=" CPU$cpuno -> Min:$min\us Max:$max\us Avg:$avg\us"
	else
		title="CPU$cpuno -> Min:$min\us Max:$max\us Avg:$avg\us"
	fi
	echo -n "\"histogram$i\" using 1:2 title \"$title\" with histeps" >>plotcmd
done

# 8. Execute plot command
gnuplot -persist <plotcmd
mv plot_*.png $FILE_DIR
