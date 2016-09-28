all:
	g++ -g -O3 -std=c++11 -pthread NanoLog.cpp benchmark.cpp -I /home/karthik/spdlog/spdlog/include
