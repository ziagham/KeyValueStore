#!/usr/bin/bash

# Copyright 2021 Amin Ziagham

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

# for thread in 1 2 4 8 16 32
# do
#     sudo HEARTBEAT_ENABLED_DIR=. ./bench_client -t $thread -d $1 -l $2 -j logging/result.json -r evaluation/result.txt
# done

#../datasets/workloads/a/1000.dat