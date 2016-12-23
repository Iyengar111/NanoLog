# NanoLog
* Low Latency C++11 Logging Library. 
* It's fast. Very fast. See [Latency benchmark](#latency-benchmark-of-guaranteed-logger)
* NanoLog only uses standard headers so it should work with any C++11 compliant compiler.
* Supports typical logger features namely multiple log levels, log file rolling and asynchronous writing to file.

# Design highlights
* Zero copying of string literals.
* Lazy conversion of integers and doubles to ascii. 
* No heap memory allocation for log lines representable in less than ~256 bytes.
* Minimalistic header includes. Avoids common pattern of header only library. Helps in compilation times of projects.

# Guaranteed and Non Guaranteed logging
* Nanolog supports Guaranteed logging i.e. log messages are never dropped even at extreme logging rates.
* Nanolog also supports Non Guaranteed logging. Uses a ring buffer to hold log lines. In case of extreme logging rate when the ring gets full (i.e. the consumer thread cannot pop items fast enough), the previous log line in the slot will be dropped. Does not block producer even if the ring buffer is full.

# Usage
```c++
#include "NanoLog.hpp"

int main()
{
  // Ensure initialize is called once prior to logging.
  // This will create log files like /tmp/nanolog1.txt, /tmp/nanolog2.txt etc.
  // Log will roll to the next file after every 1MB.
  // This will initialize the guaranteed logger.
  nanolog::initialize(nanolog::GuaranteedLogger(), "/tmp/", "nanolog", 1);
  
  // Or if you want to use the non guaranteed logger -
  // ring_buffer_size_mb - LogLines are pushed into a mpsc ring buffer whose size
  // is determined by this parameter. Since each LogLine is 256 bytes,
  // ring_buffer_size = ring_buffer_size_mb * 1024 * 1024 / 256
  // In this example ring_buffer_size_mb = 3.
  // nanolog::initialize(nanolog::NonGuaranteedLogger(3), "/tmp/", "nanolog", 1);
  
  for (int i = 0; i < 5; ++i)
  {
    LOG_INFO << "Sample NanoLog: " << i;
  }
  
  // Change log level at run-time.
  nanolog::set_log_level(nanolog::LogLevel::CRIT);
  LOG_WARN << "This log line will not be logged since we are at loglevel = CRIT";
  
  return 0;
}
```
# Latency benchmark of Guaranteed logger
* A google search for fast logger C++ gives the first result [spdlog](https://github.com/gabime/spdlog)
* There's an interesting [article](https://kjellkod.wordpress.com/2015/06/30/the-worlds-fastest-logger-vs-g3log/) on worst case latency by the author of [g3log](https://github.com/KjellKod/g3log)
* So let's benchmark NanoLog vs [spdlog](https://github.com/gabime/spdlog) vs [g3log](https://github.com/KjellKod/g3log) vs [reckless](https://github.com/mattiasflodin/reckless).
* Take a look at [nano_vs_spdlog_vs_g3log_vs_reckless.cpp](https://github.com/Iyengar111/NanoLog/blob/master/nano_vs_spdlog_vs_g3log_vs_reckless.cpp)
* Benchmark was compiled with g++ 4.8.4 running Linux Mint 17 on Intel(R) Core(TM) i7-2630QM CPU @ 2.00GHz


#### Thread Count 1 - percentile latency numbers in microseconds (lower number means better performance)

|Logger|     50th|     75th|     90th|     99th|   99.9th|    Worst|  Average|
|:-------:|:-------:|:-----:|:----------:|:------:|:------:|:------:|:------:|
|nanolog_guaranteed      |  0|        1|        1|        4|        8|       68| 0.347930|
|spdlog|3|        3|        3|        5|       11|      129| 2.588590|
|g3log|     5|        6|        6|       10|       19|      186| 5.206230|
|reckless|  0|        0|        1|        1|      175|     1861| 1.829760|

#### Thread Count 2 - percentile latency numbers in microseconds (lower number means better performance)

|Logger|     50th|     75th|     90th|     99th|   99.9th|    Worst|  Average|
|:-------:|:-------:|:-----:|:----------:|:------:|:------:|:------:|:------:|
|nanolog_guaranteed|        0|        1|        1|        2|        5|       55| 0.457240|
|nanolog_guaranteed|        0|        1|        1|        2|        5|       81| 0.459090|
|spdlog|        2|        3|        3|        3|        5|       25| 2.449580|
|spdlog|        2|        3|        3|        3|        6|       21| 2.457150|
|g3log|        4|        5|        6|       12|       18|       64| 4.574850|
|g3log|        4|        5|        6|       12|       20|       84| 4.586590|
|reckless|        0|        1|        1|       11|      417|     1592| 4.412750|
|reckless|        0|        1|        1|       12|      417|     2138| 4.427810|


#### Thread Count 3 - percentile latency numbers in microseconds (lower number means better performance)

|Logger|     50th|     75th|     90th|     99th|   99.9th|    Worst|  Average|
|:-------:|:-------:|:-----:|:----------:|:------:|:------:|:------:|:------:|
|nanolog_guaranteed|        0|        1|        1|        3|        6|       91| 0.450700|
|nanolog_guaranteed|        0|        1|        2|        3|        7|       90| 0.676050|
|nanolog_guaranteed|        0|        1|        2|        3|        7|      262| 0.680430|
|spdlog|        2|        2|        2|        4|        6|     6729| 1.803570|
|spdlog|        3|        3|        3|        5|        8|       25| 2.679420|
|spdlog|        3|        3|        3|        5|       10|       50| 2.685230|
|g3log|        4|        4|        6|       17|       27|       53| 4.385530|
|g3log|        4|        4|        6|       16|       26|       55| 4.435680|
|g3log|        6|        7|        8|       19|       29|     1031| 5.896250|
|reckless|        1|        1|        1|      298|     1643|     3070|11.208420|
|reckless|        1|        1|        1|      382|     2266|     3006|12.310360|
|reckless|        1|        1|        1|      167|     2839|     3249|12.754520|


#### Thread Count 4 - percentile latency numbers in microseconds (lower number means better performance)
|Logger|     50th|     75th|     90th|     99th|   99.9th|    Worst|  Average|
|:-------:|:-------:|:-----:|:----------:|:------:|:------:|:------:|:------:|
|nanolog_guaranteed|        0|        1|        2|        3|        6|       53| 0.582140|
|nanolog_guaranteed|        0|        1|        2|        3|        7|       70| 0.608980|
|nanolog_guaranteed|        0|        1|        2|        3|        7|       62| 0.803630|
|nanolog_guaranteed|        0|        1|        2|        3|        7|       61| 0.797270|
|spdlog|        2|        2|        2|        3|        5|       40| 1.767930|
|spdlog|        2|        2|        2|        3|        6|       21| 1.768640|
|spdlog|        3|        3|        3|        4|        8|       24| 2.676170|
|spdlog|        3|        3|        3|        5|       10|       31| 2.698580|
|g3log|        4|        4|        5|       17|       30|     7766| 4.620760|
|g3log|        6|        7|        9|       21|       35|     8478| 6.368940|
|g3log|        6|        7|        8|       22|       32|     1327| 7.023880|
|g3log|        7|        8|        9|       23|       36|     8470| 7.831750|
|reckless|        1|        1|        1|      506|     3477|     9224|18.959310|
|reckless|        1|        1|        1|      479|     3636|     8471|19.181160|
|reckless|        1|        1|        1|      530|     2990|    11658|19.245110|
|reckless|        1|        1|        1|      436|     3641|     8626|19.342780|



# Latency benchmark of Non guaranteed logger
* Take a look at [non_guaranteed_nanolog_benchmark.cpp](https://github.com/Iyengar111/NanoLog/blob/master/non_guaranteed_nanolog_benchmark.cpp) for the code used to generate the latency numbers.
* Benchmark was compiled with g++ 4.8.4 running Linux Mint 17 on Intel(R) Core(TM) i7-2630QM CPU @ 2.00GHz
```
Thread count: 1
	Average NanoLog Latency = 131 nanoseconds
Thread count: 2
	Average NanoLog Latency = 182 nanoseconds
	Average NanoLog Latency = 272 nanoseconds
Thread count: 3
	Average NanoLog Latency = 216 nanoseconds
	Average NanoLog Latency = 209 nanoseconds
	Average NanoLog Latency = 315 nanoseconds
Thread count: 4
	Average NanoLog Latency = 229 nanoseconds
	Average NanoLog Latency = 221 nanoseconds
	Average NanoLog Latency = 233 nanoseconds
	Average NanoLog Latency = 332 nanoseconds
Thread count: 5
	Average NanoLog Latency = 247 nanoseconds
	Average NanoLog Latency = 240 nanoseconds
	Average NanoLog Latency = 320 nanoseconds
	Average NanoLog Latency = 345 nanoseconds
	Average NanoLog Latency = 383 nanoseconds
```
# Crash handling
* [g3log](https://github.com/KjellKod/g3log) has support for crash handling. I do not see the point in re-inventing the wheel. Have a look at that what's done there and if it works for you, give Kjell credit and use his crash handling code.

# Tips to make it faster!
* NanoLog uses standard library chrono timestamps. Your platform / os may have non-standard but faster timestamps. Use them!
