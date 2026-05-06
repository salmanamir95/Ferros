CXX=g++
CXXFLAGS=-O3 -std=c++17
INCLUDES=-I../../shared/inc

all: LabelEngine

LabelEngine: LabelEngine.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o LabelEngine LabelEngine.cpp

clean:
	rm -f LabelEngine
