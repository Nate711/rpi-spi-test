cmake_minimum_required(VERSION 3.10)

project(pupperv3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(gflags REQUIRED)

add_executable(scheduled_spi scheduled_spi.cc)
target_link_libraries(scheduled_spi PUBLIC pigpio gflags)