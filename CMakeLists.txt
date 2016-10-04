# Check if cmake has the required version
cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

# Set name of our project
project(libbmfeb)
MESSAGE("\n*** LibBMFeb ***")

include(${CMAKE_BINARY_DIR}/FindLibUsb.cmake)
include(${CMAKE_BINARY_DIR}/FindJsonCpp.cmake)

set(LIBRARY_OUTPUT_PATH    "${CMAKE_SOURCE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3")

include_directories(  ${CMAKE_SOURCE_DIR}/.
                      ${CMAKE_SOURCE_DIR}/src
                      ${JSONCPP_INCLUDE_DIR}
                      ${LIBUSB_INCLUDE_DIR})

link_directories(     ${MIC11_DIR}/lib
                      /usr/local/lib/)

message(STATUS "C flags      : " ${CMAKE_C_FLAGS})
message(STATUS "CXX flags    : " ${CMAKE_CXX_FLAGS})
message(STATUS "Linker flags : " ${CMAKE_EXE_LINKER_FLAGS}\n)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)



