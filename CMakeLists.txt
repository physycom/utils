cmake_minimum_required(VERSION 3.20)

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
  message(STATUS "Setting default vcpkg target triplet to $ENV{VCPKG_DEFAULT_TRIPLET}")
  set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_DEFAULT_TRIPLET})
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  message(STATUS "VCPKG found: $ENV{VCPKG_ROOT}")
  message(STATUS "Using VCPKG integration")
  message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
elseif(DEFINED CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")
  if(CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    message(STATUS "Toolchain uses VCPKG integration")
    message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
  endif()
else()
  message(WARNING "vcpkg not found, toolchain not defined, build might fail")
endif()

project(physycom-utils VERSION 1.0.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CXX_STANDARD_REQUIRED TRUE)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Install prefix" FORCE)
endif()

set(INSTALL_BIN_DIR      "bin"                   CACHE PATH "Path where exe and dll will be installed")
set(INSTALL_LIB_DIR      "lib"                   CACHE PATH "Path where lib will be installed")
set(INSTALL_INCLUDE_DIR  "include"               CACHE PATH "Path where headers will be installed ")
set(INSTALL_CMAKE_DIR    "share/${PROJECT_NAME}" CACHE PATH "Path where cmake configs will be installed")

find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(test_gnssconv     ${CMAKE_SOURCE_DIR}/test/test_gnssconv.cpp)
#add_executable(test_split        ${CMAKE_SOURCE_DIR}/test/test_split.cpp)
add_executable(test_combinatoric ${CMAKE_SOURCE_DIR}/test/test_combinatoric.cpp)
add_executable(test_histo        ${CMAKE_SOURCE_DIR}/test/test_histo.cpp)

target_include_directories(test_gnssconv     PRIVATE ${CMAKE_SOURCE_DIR})
#target_include_directories(test_split        PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(test_combinatoric PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(test_histo        PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(test_gnssconv     PRIVATE Boost::boost)
#target_link_libraries(test_split        PRIVATE Boost::boost Catch2::Catch2WithMain)
target_link_libraries(test_combinatoric PRIVATE Boost::boost)
target_link_libraries(test_histo        PRIVATE Boost::boost)

install(TARGETS test_gnssconv test_combinatoric test_histo
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)
#install(TARGETS test_split
#    BUNDLE  DESTINATION .
#    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
#)
