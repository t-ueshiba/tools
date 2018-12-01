project(reset1394)

file(GLOB sources *.c)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} raw1394)
