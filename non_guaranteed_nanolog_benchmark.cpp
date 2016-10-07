#include "NanoLog.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <cstdio>

/* Returns microseconds since epoch */
uint64_t timestamp_now()
{
    return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::microseconds(1);
}

void nanolog_benchmark()
{
    int const iterations = 100000;
    char const * const benchmark = "benchmark";
    uint64_t begin = timestamp_now();
    for (int i = 0; i < iterations; ++i)
	LOG_INFO << "Logging " << benchmark << i << 0 << 'K' << -42.42;
    uint64_t end = timestamp_now();
    long int avg_latency = (end - begin) * 1000 / iterations;
    printf("\tAverage NanoLog Latency = %ld nanoseconds\n", avg_latency);
}

template < typename Function >
void run_benchmark(Function && f, int thread_count)
{
    printf("Thread count: %d\n", thread_count);
    std::vector < std::thread > threads;
    for (int i = 0; i < thread_count; ++i)
    {
	threads.emplace_back(f);
    }
    for (int i = 0; i < thread_count; ++i)
    {
	threads[i].join();
    }
}

int main()
{
    // Ring buffer size is passed as 10 mega bytes.
    // Since each log line = 256 bytes, thats 40960 slots.
    nanolog::initialize(nanolog::NonGuaranteedLogger(10), "/tmp/", "nanolog", 1);
    for (auto threads : { 1, 2, 3, 4, 5 })
    	run_benchmark(nanolog_benchmark, threads);

    return 0;
}

