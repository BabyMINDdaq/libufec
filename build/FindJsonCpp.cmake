# Find JsonCpp
# This module finds an installed JsonCpp package.
#
# It sets the following variables:
#  JSONCPP_INCLUDE_DIR, where to find header, etc.
#  JSONCPP_LIBRARIE, the libraries needed to use jsoncpp.
#  JSONCPP_FOUND, If false, do not try to use jsoncpp.

MESSAGE(" Looking for JsonCpp ...")

find_path( JSONCPP_INCLUDE_DIR NAMES json/json.h PATHS /usr/include/jsoncpp/)
find_library( JSONCPP_LIBRARY NAMES jsoncpp)

IF (JSONCPP_INCLUDE_DIR AND JSONCPP_LIBRARY)

  SET(JSONCPP_FOUND TRUE)

ENDIF (JSONCPP_INCLUDE_DIR AND JSONCPP_LIBRARY)

IF (JSONCPP_FOUND)

  MESSAGE(STATUS "Found JsonCpp: ${JSONCPP_LIBRARY}\n")

ELSE (JSONCPP_FOUND)

  MESSAGE(FATAL_ERROR "Could not find JsonCpp!\n")

ENDIF (JSONCPP_FOUND)


