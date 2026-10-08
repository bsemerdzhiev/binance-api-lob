# Benchmarks
## Initial benchmark
After initial implementation, the execution time reported by google benchmark is the following:

```
borislav@borislav ~/Documents/asymptota-lob/build ./benchmarks/lob_benchmarks
2026-10-08T16:24:48+02:00
Running ./benchmarks/lob_benchmarks
Run on (8 X 1998.26 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 24576 KiB (x1)
Load Average: 0.28, 0.29, 0.23
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
BM_Custom/1000           27041 ns        26912 ns        24011
BM_Custom/10000         649849 ns       646897 ns         1047
BM_Custom/100000      11218808 ns     11148223 ns           51
BM_Custom/1000000    295738356 ns    294153436 ns            2
BM_Custom/10000000  6620992753 ns   6585786191 ns            1
BM_Default/1000          23016 ns        22907 ns        30346
BM_Default/10000        655163 ns       651491 ns         1067
BM_Default/100000     11178046 ns     11124620 ns           59
BM_Default/1000000   352382218 ns    350213831 ns            2
BM_Default/10000000 7487902992 ns   7454339473 ns            1
```

The integer `K` displayed after BM_Custom/K and BM_Default/K is the amount of updates performed for a single symbol. A conclusion that can be drawn is 
that as K grows, the execution speed-up of the custom allocator in comparison to the default allocator also grows.

| Number of Updates | Custom Allocator | Default Allocator | Speedup |
|---:|---:|---:|---:|
| 1K | 26.9 µs | 22.9 µs | **0.85×** |
| 10K | 646.9 µs | 651.5 µs | **1.007×** |
| 100K | 11.148 ms | 11.125 ms | **0.998×** |
| 1M | 294.15 ms | 350.21 ms | **1.191×** |
| 10M | 6.586 s | 7.454 s | **1.132×** |


A slight decrease can be seen between 1M and 10M updates, which could be contributed to execution noise, since there is a positive trend as seen from the other data points.

Running `perf stat ./benchmarks/lob_benchmarks --benchmark_filter=BM_Custom/10000000` on the google benchmark test using K=10'000'000 and the custom allocator, reports:
```

borislav@borislav ~/Documents/asymptota-lob/build perf stat ./benchmarks/lob_benchmarks --benchmark_filter=BM_Custom/10000000
2026-10-08T16:35:08+02:00
Running ./benchmarks/lob_benchmarks
Run on (8 X 800 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 24576 KiB (x1)
Load Average: 0.21, 0.41, 0.35
-------------------------------------------------------------
Benchmark                   Time             CPU   Iterations
-------------------------------------------------------------
BM_Custom/10000000 6684501498 ns   6622166693 ns            1

 Performance counter stats for './benchmarks/lob_benchmarks --benchmark_filter=BM_Custom/10000000':

                 0      context-switches:u               #      0.0 cs/sec  cs_per_second
                 0      cpu-migrations:u                 #      0.0 migrations/sec  migrations_per_second
             6,212      page-faults:u                    #    873.4 faults/sec  page_faults_per_second
          7,112.34 msec task-clock:u                     #      nan CPUs  CPUs_utilized
       137,293,300      branch-misses:u                  #     13.9 %  branch_miss_rate         (88.88%)
       988,299,886      branches:u                       #    139.0 M/sec  branch_frequency     (88.89%)
    33,470,595,903      cpu-cycles:u                     #      4.7 GHz  cycles_frequency       (88.88%)
     4,755,694,884      instructions:u                   #      0.1 instructions  insn_per_cycle  (88.87%)
            TopdownL1 #     21.6 %  tma_backend_bound
                                                         #     77.7 %  tma_bad_speculation      (88.91%)
                                                         #     -0.1 %  tma_frontend_bound       (77.78%)
                                                         #      0.7 %  tma_retiring             (88.89%)

       7.116387607 seconds time elapsed

       7.014586000 seconds user
       0.034609000 seconds sys


```


After making the benchmark more representative of the actual load the LOB would see,
the L1 and L2 cache misses for the custom allocator are:

```
borislav@borislav ~/Documents/asymptota-lob/build perf stat -e \
L1-dcache-loads,L1-dcache-load-misses,\
l2_rqsts.references,l2_rqsts.miss,\
LLC-loads,LLC-load-misses \
./benchmarks/lob_benchmarks \
  --benchmark_filter='BM_Custom/10000000$'
2026-10-08T18:27:09+02:00
Running ./benchmarks/lob_benchmarks
Run on (8 X 4192.28 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 24576 KiB (x1)
Load Average: 0.34, 0.34, 0.41
-------------------------------------------------------------
Benchmark                   Time             CPU   Iterations
-------------------------------------------------------------
BM_Custom/10000000  593919759 ns    591311157 ns            1

 Performance counter stats for './benchmarks/lob_benchmarks --benchmark_filter=BM_Custom/10000000$':

       826,981,977      L1-dcache-loads:u                                                       (66.61%)
       116,536,545      L1-dcache-load-misses:u                                                 (66.67%)
       149,396,634      l2_rqsts.references:u                                                   (66.72%)
        41,931,750      l2_rqsts.miss:u                                                         (66.77%)
           723,573      LLC-loads:u                                                             (66.67%)
           689,338      LLC-load-misses:u                                                       (66.56%)

       0.837526637 seconds time elapsed

       0.798632000 seconds user
       0.034798000 seconds sys


```
A more summarised version can be found below

| Cache level | References | Misses | Miss rate |
|---|---:|---:|---:|
| L1D | 826,981,977 | 116,536,545 | **14.09%** |
| L2 | 149,396,634 | 41,931,750 | **28.07%** |
| LLC / L3 | 723,573 | 689,338 | **95.27%** |


The L1D miss rate is approximately 14%, meaning most data accesses hit in L1. Combined with the high
branch-misprediction and bad-speculation rates, this suggests that unpredictable control flow is a larger 
performance bottleneck than the memory access.

The Flamegraph generated with

```
borislav@borislav ~/Documents/asymptota-lob/build perf record -o perf-custom.data -F 499 --call-graph dwarf \
./benchmarks/lob_benchmarks \
--benchmark_filter='BM_Custom/10000000$' \
--benchmark_min_time=5s
```

and 
```
borislav@borislav ~/Documents/asymptota-lob/build perf script -i perf-custom.data | stackcollapse-perf.pl | \
flamegraph.pl --width 1800 --height 14 --fontsize 10 --minwidth 0.5 \
> perf-custom.svg
```

reports that 83% of the samples from update_symbol() is spent in .find() and .lower_bound() executed for the corresponding bids/asks map. This suggests that 
the bottleneck of this implementation is mainly the algorithm itself, not the implementation, as lower_bound() and find() are the very primitives needed to insert an element and check if its already present.
