# Check if cmake has the required version
cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

# Set name of our project
project(bmdaq)
MESSAGE("\n project: libufec \n")

set(LIBRARY_OUTPUT_PATH    "${CMAKE_SOURCE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

include(${CMAKE_BINARY_DIR}/FindLibUsb.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3")

if (VERBOSE GREATER 1)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUFE_INFO")
endif (VERBOSE GREATER 1)

if (VERBOSE GREATER 2)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUFE_DEBUG")
endif (VERBOSE GREATER 2)

include_directories(  ${CMAKE_SOURCE_DIR}/src)

link_directories(     ${CMAKE_SOURCE_DIR}/lib
                      /usr/local/lib/)

message(STATUS "C flags      : " ${CMAKE_C_FLAGS})
message(STATUS "CXX flags    : " ${CMAKE_CXX_FLAGS})
message(STATUS "Linker flags : " ${CMAKE_EXE_LINKER_FLAGS}\n)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/examples)

set(THIS_USER $ENV{USER})
configure_file(  ${CMAKE_BINARY_DIR}/config.h.cmake
                 ${CMAKE_BINARY_DIR}/89-ufe-usb.rules)
