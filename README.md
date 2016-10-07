# NanoLog
* Low Latency C++11 Logging Library. 
* It's fast. Very fast. See [Latency benchmark](#latency-benchmark)
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
# Latency benchmark
* A google search for fast logger C++ gives the first result [spdlog](https://github.com/gabime/spdlog)
* So let's benchmark NanoLog vs [spdlog](https://github.com/gabime/spdlog).
* Take a look at [benchmark.cpp](https://github.com/Iyengar111/NanoLog/blob/master/benchmark.cpp) for the code used to generate the latency numbers.
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
Thread count: 1
	Average spdlog Latency = 1711 nanoseconds
Thread count: 2
	Average spdlog Latency = 1687 nanoseconds
	Average spdlog Latency = 1702 nanoseconds
Thread count: 3
	Average spdlog Latency = 1699 nanoseconds
	Average spdlog Latency = 1712 nanoseconds
	Average spdlog Latency = 3014 nanoseconds
Thread count: 4
	Average spdlog Latency = 1678 nanoseconds
	Average spdlog Latency = 1706 nanoseconds
	Average spdlog Latency = 3002 nanoseconds
	Average spdlog Latency = 3204 nanoseconds
Thread count: 5
	Average spdlog Latency = 1699 nanoseconds
	Average spdlog Latency = 1727 nanoseconds
	Average spdlog Latency = 2665 nanoseconds
	Average spdlog Latency = 2663 nanoseconds
	Average spdlog Latency = 1627 nanoseconds
```
# Crash handling
* [g3log](https://github.com/KjellKod/g3log) has support for crash handling. I do not see the point in re-inventing the wheel. Have a look at that what's done there and if it works for you, give Kjell credit and use his crash handling code.

# Tips to make it faster!
* NanoLog uses standard library chrono timestamps. Your platform / os may have non-standard but faster timestamps. Use them!
