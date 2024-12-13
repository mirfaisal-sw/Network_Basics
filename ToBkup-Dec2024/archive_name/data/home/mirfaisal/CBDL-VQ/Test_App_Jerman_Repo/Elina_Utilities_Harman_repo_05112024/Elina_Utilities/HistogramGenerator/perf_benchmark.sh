#!/bin/sh

perf list > pre-defined_events.txt
perf stat -B dd if=/dev/zero of=/dev/null count=1000000 
perf stat -e cycles:u dd if=/dev/zero of=/dev/null count=100000 
perf stat -e cycles:k dd if=/dev/zero of=/dev/null count=100000
perf stat -r 5 sleep 1
perf bench all
time /bin/cyclictest -D 10s --policy=fifo -Sp40 -t5 -h4000 -q > output
sync
