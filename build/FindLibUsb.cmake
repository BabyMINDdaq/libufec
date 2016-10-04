# Find JsonCpp
# This module finds an installed JsonCpp package.
#
# It sets the following variables:
#  LIBUSB_INCLUDE_DIR, where to find header, etc.
#  LIBUSB_LIBRARIE, the libraries needed to use jsoncpp.
#  LIBUSB_FOUND, If false, do not try to use jsoncpp.

MESSAGE(" Looking for LibUsb-1.0 ...")

find_path( LIBUSB_INCLUDE_DIR NAMES libusb-1.0/libusb.h)
find_library( LIBUSB_LIBRARY NAMES usb-1.0)

IF (LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARY)

  SET(LIBUSB_FOUND TRUE)

ENDIF (LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARY)

IF (LIBUSB_FOUND)

  MESSAGE(STATUS "Found LibUsb: ${LIBUSB_LIBRARY}\n")

ELSE (LIBUSB_FOUND)

  MESSAGE(FATAL_ERROR "Could not find LibUsb!\n")

ENDIF (LIBUSB_FOUND)


