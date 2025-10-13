CXX = g++
CXXFLAGS = -std=c++17 -pthread -02

all: sloppySim

sloppySim: sloppySim.cpp
    $(CXX) $(CXXFLAGS) -o sloppySim sloppySim.cpp


clean: rm -f sloppySim