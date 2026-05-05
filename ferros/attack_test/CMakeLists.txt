cmake_minimum_required(VERSION 3.10)
project(FerrosCpuStress)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization flags for maximum CPU stress
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -pthread")

add_executable(cpu_stress 
    src/cpu_stress.cpp
    src/workload_profiles.cpp
)

# Link threads library
find_package(Threads REQUIRED)
target_link_libraries(cpu_stress PRIVATE Threads::Threads)

# Set build type to Release by default if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

install(TARGETS cpu_stress DESTINATION bin)
