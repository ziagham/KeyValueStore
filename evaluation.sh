#!/usr/bin/bash

workloadPath=../datasets/
extension="*.dat"
duration=10

if [[ "$1" != "" ]]; then
    workloadPath="$1"
fi

if [[ "$2" != "" ]]; then
    duration="$2"
fi

path="ls $workloadPath$extension | sort -n"

evaluationPath="evaluation"
resultPath=${evaluationPath}/result.csv

if [ ! -d "$evaluationPath" ]
then
    mkdir ./$evaluationPath
fi

for workload in $path
do
    for thread in 1 2 4 8 16 32
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