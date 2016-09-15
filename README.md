# NanoLog
* Low Latency C++11 Logging Library. 
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
Coming soon...

# Tips to make it faster!
* NanoLog uses standard library chrono timestamps. Your platform / os may have non-standard but faster timestamps. Use them!
