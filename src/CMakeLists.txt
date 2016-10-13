# add_executable (test testlibusb.c)
# target_link_libraries(test usb)

add_library(ufe SHARED libufe.c)
target_link_libraries(ufe usb-1.0)

add_library(bmdaq SHARED UFEConfigFrame.cpp
                         UFEConfigLib.cpp
                         UFEError.cpp)
target_link_libraries(bmdaq ufe jsoncpp)

add_executable (test_cpp test.cpp)
target_link_libraries(test_cpp ufe)

# add_executable (test_c test.c)
# target_link_libraries(test_c ufe)

add_executable (reset reset.cpp)
target_link_libraries(reset ufe)

add_executable (config config.cpp)
target_link_libraries(config bmdaq)