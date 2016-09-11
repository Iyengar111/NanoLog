# NanoLog
* Low Latency C++11 Logging Library. 
* NanoLog only uses standard C++11 headers so it should work with any C++11 compliant compiler.
* Supports typical logger features namely multiple log levels, log file rolling and asynchronous writing to file.

# Design highlights
* No heap memory allocation for log lines representable in less than ~256 bytes.
* Zero copying of string literals.
* Lazy conversion of integers and double to ascii. 
* Minimalistic header includes. Avoids common pattern of header only library. Helps in compilation times of projects.

# Usage
```
#include "NanoLog.hpp"

int main()
{
  // Ensure initialize is called once prior to logging.
  // This will create log files like /tmp/nanolog1.txt, /tmp/nanolog2.txt etc.
  // Log will roll to the next file after every 1MB.
  nanolog::initialize("/tmp/", "nanolog", 1);
  
  for (int i = 0; i < 5; ++i)
  {
    LOG_INFO << "Sample NanoLog: " << i;
  }
  
  return 0;
}
```
# Latency benchmark
Coming soon...
