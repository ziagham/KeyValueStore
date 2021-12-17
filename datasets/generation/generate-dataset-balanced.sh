#!/usr/bin/bash

# Copyright 2021 Koen van Greevenbroek         
# Subject to the GNU General Public License 3.0

TEMPFILE=$(mktemp)

source venv/bin/activate 
./ycsb-0.17.0/bin/ycsb run basic -P ycsb-0.17.0/workloads/workloadb -P workload_balanced.txt > $TEMPFILE
make
./convert $TEMPFILE $1
rm $TEMPFILE
deactivate

