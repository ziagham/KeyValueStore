# Multi-threading In-memory Key-Value Store (KVS)
This in-memory KVS uses the Trie-tree as the main in-memory data structure for achieving high insert and lookup performance. For concurrency, this KVS uses Spin-lock mechanism to avoid race conditions and guarantee mutual exclusion. Also for benchmarking and evaluating the KVS system, two libraries have been used. For energy-efficiency evaluation, POET toolkit have been integrated to the proposed KVS and Yahoo! Cloud Serving Benchmark (YCSB) have been used for benchmarking.

# Benchmark program
First of all, it should be noted that the relevant datasets are placed in a folder called the **datasets**. The datasets are privided are categories in three type A, B, and C. Which indicates of YCSB types. A for 50:50, B for 95:05, and C for 100:0 percent of read/update ratio.


## Run a benchmark for one mode only
In this case, only one mode can be executed and benchmarked. One mode means that, for example, a doing benchmark for only 4 threads, or for a duration of 10 seconds, or for a dataset. For this purpose

1. Use ```make``` to compile the benchmark if necessary.

2. To run, execute this command in the terminal : 
```sudo HEARTBEAT_ENABLED_DIR=. ./bench_client -t <no. of threads> -d <duration>  -l <path to dataset> -j <json file for output logging> -r <csv file for output result>```.

It is necessary to enter the name of a file as the output file of the results (CSV file).

For an instance, the following command can run the benchmark for KVS in 4 threads and 10 seconds on the data type a (which has 1000 records) and save the result in the /result.csv.

```sudo HEARTBEAT_ENABLED_DIR=. ./bench_client -t 4 -d 10  -l datasets/a/1000.dat -j logging/result.json -r result.csv```

## Run a benchmark for multiple mode
But to run the benchmark for different modes, a script called **evaluation.sh** has been prepared. This script has two arguments. The first one is the name of the folder in which the various datasets are located, and the second argument is related to the duration of running benchmark. This script performs a benchmark for all the datasets in that folder and for different numbers of threads (from 1 to 32). The results of this execution will also be stored in a path called (evaluation/result.csv).

This script can be run in two modes. 

1. The first mode, executes a benchmark with a different number of threads for a specific duration (for example, 30 seconds) for all datasets in a folder.

For an instance, the following command can run the benchmark for all YCSB dataset located folder **/datasets/a/** for 30 seconds of duration for different threads:

```evaluation.sh datasets/a/ threads_mode 30```

In the above command, the word **threads_mode** refers to the state in which the benchmark should be executed (meaning for different threads) and 30 indicates the duration time for each mode.

2. The second mode, is the mode in which the benchmark is run for a specific thread and at different durtions. To run in this mode, run the following command:

```evaluation.sh datasets/a/ duration_mode 8```

In the above command, the word **duration_mode** refers to the state in which the benchmark should be executed (meaning for different durations) and 8 indicates the number of threads that can be used.

# Plotting the result
Once the benchmark is done and the results are generated, those results can be plotted. To do this, the **plotting.py** has been prepared which is responsible for reading the generated data and plotting them.

## Prerequisites
To run this plotting program, you need to have the Python programming language installed on your system. In addition, three python libraries must be installed. These libraries include **pandas**, **matplotlib**, and **numpy**. You can use the following commands to install them:

```pip3 install numpy```
```pip3 install matplotlib```
```pip3 install pandas```

## Run
To run this program and plot a chart, enter the following command in the terminal:

```python3 plotting.py -r <the result csv file> -t <type of plotting: "threads" or "duration"> [--pdf]```

In the above command,**-r** refere to the result path, **-t** indicated the plotting mode (wcich could be only "threads" or "duration" words.), **--pdf** is used for the case where we do not want the display the chart want to saved directly as a pdf file.

For example, to plot the benchmark's results which located in the **result/data.csv** path, the following command must be executed:

```python3 plotting.py -r result/data.csv -t threads --pdf```
