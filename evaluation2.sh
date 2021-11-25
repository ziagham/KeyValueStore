#!/usr/bin/bash

workloadPath=../datasets/
extension="*.dat"
thread=8

if [[ "$1" != "" ]]; then
    workloadPath="$1"
fi

if [[ "$2" != "" ]]; then
    thread="$2"
fi

path="ls $workloadPath$extension | sort -n"

evaluationPath="evaluation"
resultPath=${evaluationPath}/result2.csv

if [ ! -d "$evaluationPath" ]
then
    mkdir ./$evaluationPath
fi

for workload in $path
do
    for (( duration=10; duration<=40; duration+=10 ));
    do
        #for (( duration=10; duration<=$2; duration+=10 ));
        #do
        sudo HEARTBEAT_ENABLED_DIR=. ./bench_client -t $thread -d $duration -l $workload -j logging/result.json -r $resultPath
        #done
    done
done

# Cleanup heartbeats shared memory
MEMS=`ipcs | grep $USER | awk '{print $2}'`
for k in $MEMS
do
        echo Freeing $k
        ipcrm -m $k
done