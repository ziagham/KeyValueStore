#!/usr/bin/bash

workloadPath=../datasets/
extension="*.dat"
mode="threads_mode"
input=8

if [[ "$1" != "" ]]; then
    workloadPath="$1"
fi

if [[ "$2" != "" ]]; then
    mode="$2"
fi

if [[ "$3" != "" ]]; then
    input="$3"
fi

path="ls $workloadPath$extension | sort -n"

evaluationPath="evaluation"
resultPath=${evaluationPath}/result-duration.csv

if [ $mode == "threads_mode" ]; then
    echo "Hello"
    resultPath=${evaluationPath}/result-threads.csv
fi

if [ ! -d "$evaluationPath" ]; then
    mkdir ./$evaluationPath
fi

for workload in $path
do
    if [ $mode == "threads_mode" ]; then
        duration=$input
        for thread in 1 2 4 8 16 32
        do
            sudo HEARTBEAT_ENABLED_DIR=. ./bench_client -t $thread -d $duration -l $workload -j logging/result.json -r $resultPath
        done
    elif [ $mode == "duration_mode" ]; then
        thread=$input
        for (( duration=10; duration<=40; duration+=10 ));
        do
            sudo HEARTBEAT_ENABLED_DIR=. ./bench_client -t $thread -d $duration -l $workload -j logging/result.json -r $resultPath
        done
    fi
done

# Cleanup heartbeats shared memory
MEMS=`ipcs | grep $USER | awk '{print $2}'`
for k in $MEMS
do
    echo Freeing $k
    ipcrm -m $k
done