cmake_minimum_required(VERSION 3.20)

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
  message(STATUS "Setting default vcpkg target triplet to $ENV{VCPKG_DEFAULT_TRIPLET}")
  set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_DEFAULT_TRIPLET})
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
  set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)
  message(STATUS "VCPKG found: $ENV{VCPKG_ROOT}")
  message(STATUS "Using VCPKG integration")
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  if(VCPKG_MANIFEST_FEATURES)
    message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
  endif()
elseif(DEFINED CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")
  if(CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    message(STATUS "Toolchain uses VCPKG integration")
    if(VCPKG_MANIFEST_FEATURES)
      message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
    endif()
  endif()
else()
  message(WARNING "vcpkg not found, toolchain not defined, build might fail")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json VCPKG_JSON_STRING)
  string(JSON VERSION_STRING GET ${VCPKG_JSON_STRING} version)
else()
  set(VERSION_STRING "0.0.0")
endif()


project(physycom-utils VERSION ${VERSION_STRING} LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CXX_STANDARD_REQUIRED TRUE)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Install prefix" FORCE)
endif()

set(INSTALL_BIN_DIR      "bin"                     CACHE PATH "Path where exe and dll will be installed")
set(INSTALL_LIB_DIR      "lib"                     CACHE PATH "Path where lib will be installed")
set(INSTALL_INCLUDE_DIR  "include/${PROJECT_NAME}" CACHE PATH "Path where headers will be installed ")
set(INSTALL_CMAKE_DIR    "share/${PROJECT_NAME}"   CACHE PATH "Path where cmake configs will be installed")

find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)

file(GLOB interfaceSRC
    "${CMAKE_SOURCE_DIR}/physycom/*.hpp"
)
add_library(utils INTERFACE ${interfaceSRC})
target_include_directories(utils INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}> $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>)
set_target_properties(utils PROPERTIES PUBLIC_HEADER "${interfaceSRC}")

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

INSTALL(TARGETS utils
  EXPORT physycom-utils-config
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR}
  COMPONENT dev
)

INSTALL(EXPORT physycom-utils-config NAMESPACE Physycom:: DESTINATION ${INSTALL_CMAKE_DIR})
