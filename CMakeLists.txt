# Check if cmake has the required version
cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

# Set name of our project
project(bmdaq)
MESSAGE("\n project: libufec \n")

set(LIBRARY_OUTPUT_PATH    "${CMAKE_SOURCE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

include(${CMAKE_BINARY_DIR}/FindLibUsb.cmake)
include(${CMAKE_BINARY_DIR}/FindCppUnit.cmake)
include(${CMAKE_BINARY_DIR}/FindZeroMQ.cmake)

find_package(Doxygen)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pthread -ludev -O3")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")

if (NOT DEFINED _VERBOSE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUFE_WARNING -DUFE_INFO")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUFE_WARNING -DUFE_INFO")
endif()

if (_VERBOSE GREATER 0)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUFE_WARNING")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUFE_WARNING")
endif (_VERBOSE GREATER 0)

if (_VERBOSE GREATER 1)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUFE_INFO")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUFE_INFO")
endif (_VERBOSE GREATER 1)

if (_VERBOSE GREATER 2)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUFE_DEBUG")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUFE_DEBUG")
endif (_VERBOSE GREATER 2)

if (DEFINED _USE_NETWORK_ZMQ)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DZMQ_ENABLE")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DZMQ_ENABLE")
  message(STATUS "networking enabled\n")
endif()

include_directories(  ${CMAKE_SOURCE_DIR}/src)

link_directories(     ${CMAKE_SOURCE_DIR}/lib
                      /usr/local/lib/)

message(STATUS "C flags      : " ${CMAKE_C_FLAGS})
message(STATUS "CXX flags    : " ${CMAKE_CXX_FLAGS})
message(STATUS "Linker flags : " ${CMAKE_EXE_LINKER_FLAGS}\n)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/tools)

if    (CPPUNIT_FOUND)
  add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif (CPPUNIT_FOUND)

set(THIS_USER $ENV{USER})
configure_file(  ${CMAKE_BINARY_DIR}/89-ufe-usb.rules.cmake
                 ${CMAKE_BINARY_DIR}/89-ufe-usb.rules)


if (_DOXYGEN_DOC AND DOXYGEN_FOUND)

  message(" doxygen documentation ...\n")
  add_custom_target(doc ALL)
  add_custom_command(TARGET doc
        COMMAND doxygen dox_config
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc)

endif ()

