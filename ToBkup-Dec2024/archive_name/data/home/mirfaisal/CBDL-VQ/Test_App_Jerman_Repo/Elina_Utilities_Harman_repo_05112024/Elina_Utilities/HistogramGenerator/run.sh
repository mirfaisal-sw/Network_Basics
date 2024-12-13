#!/bin/sh

cd /home/root/plot/
exec /bin/cyclictest -D 10s --policy=fifo -Sp80 -h4000 -q > output_$1
cd -
