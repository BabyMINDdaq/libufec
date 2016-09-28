# add_executable (test testlibusb.c)
# target_link_libraries(test usb)

add_library(bmfeb SHARED libbmfeb.c)
target_link_libraries(bmfeb usb-1.0)

add_executable (test_cpp test.cpp)
target_link_libraries(test_cpp bmfeb)

add_executable (test_c test.c)
target_link_libraries(test_c bmfeb)
