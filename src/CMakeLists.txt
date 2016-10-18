MESSAGE(" libufec ...")

if (STATIC)

  MESSAGE(STATUS "building static library\n")
  add_library(ufec libufe.c libufe-tools.c)
  target_link_libraries(ufec ${LIBUSB_LIBRARY})

else (STATIC)

  MESSAGE(STATUS "building shered library\n")
  add_library(ufec SHARED libufe.c libufe-tools.c)
  target_link_libraries(ufec ${LIBUSB_LIBRARY})

endif ()


# add_executable (config config.cpp)
# target_link_libraries(config bmdaq)