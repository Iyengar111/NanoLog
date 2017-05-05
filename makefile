all:
	g++ -g -O3 -std=c++11 -pthread NanoLog.cpp non_guaranteed_nanolog_benchmark.cpp -o non_guaranteed_nanolog_benchmark
	g++ -g -O3 -std=c++11 -fPIC -Ofast -m64 -Wl,-rpath -Wl,/usr/local/probe/lib -Wl,-rpath -Wl,/usr/local/probe/lib64  -lg3logger -pthread NanoLog.cpp nano_vs_spdlog_vs_g3log_vs_reckless.cpp -I /usr/local/probe/include/ -o nano_vs_g3log
