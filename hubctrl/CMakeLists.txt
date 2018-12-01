project(hubctrl)

file(GLOB sources *.cc)
add_executable(hubctrl ${sources} ${headers})
target_link_libraries(hubctrl TUUSB++ usb)

install(TARGETS hubctrl RUNTIME DESTINATION bin)
