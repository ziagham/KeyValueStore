#!/usr/bin/bash

# Copyright 2021 Koen van Greevenbroek         
# Subject to the GNU General Public License 3.0

TEMPFILE=$(mktemp)
workloadConfig="workload_a_zipfian.txt"
workloadType="a"

if [[ "$2" != "" ]]; then
    workloadType="$2"
else
	echo "Please provide a dataset type (should be 'a' to 'f') as a second argument."
	exit 1
fi

if [[ "$3" != "" ]]; then
    workloadConfig="$3"
fi

source venv/bin/activate 
./ycsb-0.17.0/bin/ycsb run basic -P ycsb-0.17.0/workloads/workload$workloadType -P $workloadConfig > $TEMPFILE
make
./convert $TEMPFILE $1
rm $TEMPFILE
deactivate

