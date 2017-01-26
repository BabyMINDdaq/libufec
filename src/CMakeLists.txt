MESSAGE(" libufec ...")

if (_STATIC)

  MESSAGE(STATUS "building static library\n")
  add_library(ufec libufe.c libufe-core.c libufe-tools.c)
  target_link_libraries(ufec ${LIBUSB_LIBRARY})

else (_STATIC)

  MESSAGE(STATUS "building shered library\n")
  add_library(ufec SHARED libufe.c libufe-core.c libufe-tools.c)
  target_link_libraries(ufec ${LIBUSB_LIBRARY})

endif ()


