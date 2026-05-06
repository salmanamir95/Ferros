CXX=g++
CXXFLAGS=-O3 -std=c++17 -pthread
INCLUDES=-I../shared/inc
BUILD_DIR=../build_research

# Targets
SIMULATOR_SRC=src/simulator/simulator.cpp
LABELER_SRC=src/labeler/labeler.cpp

TARGETS=$(BUILD_DIR)/simulator $(BUILD_DIR)/labeler

all: $(TARGETS)

$(BUILD_DIR)/simulator: $(SIMULATOR_SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(SIMULATOR_SRC)

$(BUILD_DIR)/labeler: $(LABELER_SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(LABELER_SRC)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
