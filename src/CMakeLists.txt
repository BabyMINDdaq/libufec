MESSAGE(" libufec ...")

if (_STATIC)

  MESSAGE(STATUS "building static library\n")
  add_library(ufec libufe.c libufe-core.c libufe-tools.c)

else (_STATIC)

  MESSAGE(STATUS "building shered library\n")
  add_library(ufec SHARED libufe.c libufe-core.c libufe-tools.c)


endif ()

if (ZMQ_FOUND AND _USE_NETWORK_ZMQ)

  target_link_libraries(ufec ${LIBUSB_LIBRARY} ${ZMQ_LIBRARY})

else (ZMQ_FOUND AND _USE_NETWORK_ZMQ)

  target_link_libraries(ufec ${LIBUSB_LIBRARY})

endif()
