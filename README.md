# NanoLog
* Low Latency C++11 Logging Library. 
* It's fast. See [Latency benchmark](# Latency benchmark)
* NanoLog only uses standard headers so it should work with any C++11 compliant compiler.
* Supports typical logger features namely multiple log levels, log file rolling and asynchronous writing to file.

# Design highlights
* Zero copying of string literals.
* Lazy conversion of integers and doubles to ascii. 
* No heap memory allocation for log lines representable in less than ~256 bytes.
* Minimalistic header includes. Avoids common pattern of header only library. Helps in compilation times of projects.

# Usage
```c++
#include "NanoLog.hpp"

int main()
{
  // Ensure initialize is called once prior to logging.
  // This will create log files like /tmp/nanolog1.txt, /tmp/nanolog2.txt etc.
  // Log will roll to the next file after every 1MB.
  // Optional 4th parameter - ring_buffer_size_in_mb
  // This is passed as 8 in this example. Each slot in the ring buffer is 256 bytes.
  // So for 8 mega bytes, the ring buffer has 8 * 1024 * 1024 / 256 slots.
  // Default value is 4 mega bytes which is approx 16k slots.
  nanolog::initialize("/tmp/", "nanolog", 1, 8);
  
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
* Take a look at benchmark.cpp for the code used to generate the latency numbers.
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

# Tips to make it faster!
* NanoLog uses standard library chrono timestamps. Your platform / os may have non-standard but faster timestamps. Use them!
