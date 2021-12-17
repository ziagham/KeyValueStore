Dataset generation
==================

The YSCB package can generate dataset loads (sequences of queries) consisting of GET and PUT operations. The distribution of the key and the ratio of GET to PUT queries can easily be adjusted.

The `bench_client` program provided by the course expects a different format than the one provided by YSCB, so we have written a simple conversion routine to generate datasets in the format expected by `bench_client`. This conversion is quite fragile, but should work in the default configuration.

Feel free to use the provided datasets. However, it should be easy to generate and prepare new datasets as needed. In particular, if you change the type of query expected by your database, or need to test different kinds of workloads, you need to generate your own datasets.

There are a few places to adjust the dataset generation:
1. The settings in `workload.txt` (number of queries, distribution, etc.).
2. The header file `src/convert.h` (format of queries, key size).
3. YSCB command in `generate-datasets.sh` (other workload settings).

As to the third point, YSCB has a few different default configurations (called *workloads*) specifying different ratios of GET to PUT queries and so forth. See [[https://github.com/brianfrankcooper/YCSB/wiki/Core-Workloads]] for an overview. The workloads are labelled A through F, and are selected in `generate-datasets.sh` by the `-P ycsb-0.17.0/workloads/workloadb`-part. Feel free to try different workloads (e.g. `workloada, workloadc`).

YSCB distinguishes between inserts and updates, while these are treated as the same by `bench_client` program. We just treat YSCB "UPDATE" queries as "PUT" queries. YSCB also has separate *load* and *run* modes. The load mode just issues a bunch of "INSERT" queries, while the run mode issues "UPDATE" and "READ" (aka GET) queries. Here we just use the run mode. However, if your database needs to be loaded before being benchmarked, feel free to modify `generate-datasets.sh` to also generate a "loading" dataset (using the same settings as the corresponding "run" dataset.

How to generate datasets
========================

If you are going to generate your own datasets with your own settings, copy this directory to somewhere in your own home directory, and follow these instructions:

1. Adjust dataset settings as described above.
2. Make sure that the parameters in `src/convert.h` are as desired. In particular, check the `NKEY` constant, determining the size in bytes of database keys to generate.
4. Run `./generate-dataset.sh <dataset_name>`.
5. Deactivate the conda environment when you are done: `conda deactivate`.

Copyright 2021 Koen van Greevenbroek
Subject to the GNU General Public License 3.0
