CXX = g++
CXXFLAGS = -std=c++17 -pthread -O2

all: sloppySim

sloppySim: sloppySim.cpp
	$(CXX) $(CXXFLAGS) -o sloppySim sloppySim.cpp

clean: rm -f sloppySim