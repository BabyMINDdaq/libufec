# Find zeromq
# This module finds an installed zeromq package.
#
# It sets the following variables:
#  ZMQ_INCLUDE_DIR, where to find header, etc.
#  ZMQ_LIBRARY, the libraries needed to use zeromq.
#  ZMQ_FOUND, If false, do not try to use zeromq.

MESSAGE(" Looking for zeromq ...")

# find_path( ZMQ_INCLUDE_DIR NAMES  zmq.hpp PATHS /usr/include/)
# find_library( ZMQ_LIBRARY NAMES libzmq.so  PATHS /user/lib/)

find_path( ZMQ_INCLUDE_DIR NAMES  zmq.h )
find_library( ZMQ_LIBRARY NAMES libzmq.so )

IF (ZMQ_INCLUDE_DIR AND ZMQ_LIBRARY)

  SET(ZMQ_FOUND TRUE)

ENDIF (ZMQ_INCLUDE_DIR AND ZMQ_LIBRARY)

IF (ZMQ_FOUND)

  MESSAGE(STATUS "Found zeromq: ${ZMQ_LIBRARY}\n")

ELSE (ZMQ_FOUND)

  MESSAGE(STATUS "Could not find zeromq!\n")

ENDIF (ZMQ_FOUND)