cmake_minimum_required (VERSION 3.1)
project (utils)
set (CMAKE_CXX_STANDARD 11)

if(MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})

add_executable(test_gnssconv ${CMAKE_SOURCE_DIR}/test/test_gnssconv.cpp)
add_executable(test_split ${CMAKE_SOURCE_DIR}/test/test_split.cpp)
