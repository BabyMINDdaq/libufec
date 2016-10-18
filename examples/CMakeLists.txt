MESSAGE(" examples ...")

MESSAGE(STATUS "ufe-usb-reset")
add_executable (ufe-usb-reset usb_reset.c)
target_link_libraries(ufe-usb-reset ufec)

MESSAGE(STATUS "ufe-led-on")
add_executable (ufe-led-on led_on.c)
target_link_libraries(ufe-led-on ufec)

MESSAGE(STATUS "ufe-config")
add_executable (ufe-config config.c)
target_link_libraries(ufe-config ufec)

MESSAGE(STATUS "ufe-set-param")
add_executable (ufe-set-param set_param.c)
target_link_libraries(ufe-set-param ufec)

MESSAGE(STATUS "ufe-read-status")
add_executable (ufe-read-status read_status.c)
target_link_libraries(ufe-read-status ufec)

MESSAGE("")