#!/bin/bash
# Cleanup heartbeats shared memory
# Connor Imes
# 2015-01-15

MEMS=`ipcs | grep $USER | awk '{print $2}'`
for k in $MEMS
do
        echo Freeing $k
        ipcrm -m $k
done

