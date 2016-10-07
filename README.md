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
* Nanolog also supports Non Guaranteed logging. Uses a ring buffer to hold log lines. When the ring gets full, the previous log line in the slot will be dropped. Does not block producer even if the ring buffer is full.

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
* Theres another interesting [article](https://kjellkod.wordpress.com/2015/06/30/the-worlds-fastest-logger-vs-g3log/) on worst case latency by the author of [g3log](https://github.com/KjellKod/g3log)
* So let's benchmark NanoLog vs [spdlog](https://github.com/gabime/spdlog) vs [g3log](https://github.com/KjellKod/g3log).
* Take a look at [nano_vs_spdlog_vs_g3log.cpp](https://github.com/Iyengar111/NanoLog/blob/master/nano_vs_spdlog_vs_g3log.cpp)
* Benchmark was compiled with g++ 4.8.4 running Linux Mint 17 on Intel(R) Core(TM) i7-2630QM CPU @ 2.00GHz
```
Thread count: 1

nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                0|                1|                2|                6|               70|         0.297110|
spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                3|                3|                3|                5|               11|               92|         2.735020|
g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                5|                6|                6|               10|               20|              186|         5.194330|



Thread count: 2

nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                1|                1|                2|                5|               94|         0.449340|
nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                1|                1|                2|                5|               94|         0.455510|

spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                2|                2|                2|                4|                6|               24|         1.671900|
spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                3|                3|                4|                4|               11|               27|         3.025790|

g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                4|                5|                7|               12|               20|               95|         4.599040|
g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                6|                7|                8|               14|               20|               76|         6.076550|





Thread count: 3

nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                1|                2|                3|                6|               87|         0.574600|
nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                1|                2|                3|                6|               92|         0.578910|
nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                1|                2|                3|                6|               93|         0.587460|

spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                2|                2|                2|                3|                4|               34|         1.739540|
spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                3|                3|                3|                4|                8|               34|         2.677610|
spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                3|                3|                3|                4|               10|              178|         2.710650|

g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                4|                4|                6|               18|               28|              104|         4.546910|
g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                4|                4|                6|               18|               28|              800|         4.569840|
g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                6|                7|               10|               21|               33|             2294|         7.167530|




Thread count: 4

nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                1|                2|                3|                7|               48|         0.619480|
nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                1|                2|                3|                7|               80|         0.644090|
nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                1|                2|                3|                8|               49|         0.667830|
nanolog_guaranteed latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                0|                1|                3|                4|                9|               49|         0.892340|

spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                3|                3|                3|                3|                8|              125|         2.691380|
spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                3|                3|                3|                3|                6|               22|         2.665510|
spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                3|                3|                3|                3|                8|             4026|         2.726420|
spdlog latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                3|                3|                3|                6|                9|             4053|         2.754710|

g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                6|                7|                8|               24|               36|             9086|         6.382610|
g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                6|                7|                8|               23|               35|             2027|         6.430090|
g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                6|                7|                9|               24|               39|             9047|         6.896570|
g3log latency numbers in microseconds
50th percentile  |75th percentile  |90th percentile  |99th percentile  |99.9th percentile|      Worst      |     Average     |
                7|                7|                9|               25|               41|             9387|         7.291690|


```

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
