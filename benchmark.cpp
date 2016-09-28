#include "NanoLog.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <cstdio>
#include "spdlog/spdlog.h"

/* Returns microseconds since epoch */
uint64_t timestamp_now()
{
    return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::microseconds(1);
}

void nanolog_benchmark()
{
    char const * const benchmark = "benchmark";
    uint64_t begin = timestamp_now();
    for (int i = 0; i < 1000; ++i)
	LOG_INFO << "Logging " << benchmark << i << 0 << 'K' << -42.42;
    uint64_t end = timestamp_now();
    printf("\tAverage NanoLog Latency = %ld nanoseconds\n", end - begin);
}

std::shared_ptr < spdlog::logger > spd_logger;

void spdlog_benchmark()
{
    char const * const benchmark = "benchmark";
    uint64_t begin = timestamp_now();
    for (int i = 0; i < 1000; ++i)
	spd_logger->info("Logging {}{}{}{}{}", benchmark, i, 0, 'K', -42.42);
    uint64_t end = timestamp_now();
    printf("\tAverage spdlog Latency = %ld nanoseconds\n", end - begin);
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
    nanolog::initialize("/tmp/", "nanolog", 1);
    for (auto threads : { 1, 2, 3, 4, 5 })
    	run_benchmark(nanolog_benchmark, threads);
    
    spdlog::set_async_mode(1048576);
    spd_logger = spdlog::create < spdlog::sinks::simple_file_sink_mt >("file_logger", "/tmp/spd-async.txt", false);
    spd_logger->set_pattern("[%Y-%m-%d %T.%e]: %v");
    for (auto threads : { 1, 2, 3, 4, 5 })
    	run_benchmark(spdlog_benchmark, threads); 

    return 0;
}

