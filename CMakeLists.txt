cmake_minimum_required(VERSION 3.20)
project(AP1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -pthread)


add_executable(AP1 MainTrain.cpp anomaly_detection_util.cpp anomaly_detection_util.h AnomalyDetector.h CLI.cpp
        CLI.h commands.h HybridAnomalyDetector.cpp HybridAnomalyDetector.h minCircle.cpp minCircle.h
        SimpleAnomalyDetector.cpp SimpleAnomalyDetector.h timeseries.cpp timeseries.h standardIO.h Server.h timeseries.cpp Server.cpp)
