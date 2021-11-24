#!/usr/bin/bash

workloadPath=../datasets/
extension="*.dat"

if [[ "$1" != "" ]]; then
    workloadPath="$1"
fi

path="ls $workloadPath$extension | sort -n"

evaluationPath="evaluation"

if [ ! -d "$evaluationPath" ]
then
    mkdir ./$evaluationPath
fi

for workload in $path
do
    for thread in 1 2 4 #8 16 32
    do
        for duration in 10 #20 30
        do
            sudo HEARTBEAT_ENABLED_DIR=. ./bench_client -t $thread -d $duration -l $workload -j logging/result.json -r ${evaluationPath}/result.txt
        done
    done
done