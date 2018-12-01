<<<<<<< HEAD
project(hubctrl)

file(GLOB sources *.cc)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} TUUSB++)
=======
cmake_minimum_required(VERSION 2.4)

project(hubctrl)

enable_language(CXX)
set(CMAKE_CXX_STANDARD		14)
set(CMAKE_CXX_STANDARD_REQUIRED	ON)
set(CMAKE_CXX_EXTENSIONS	OFF)
set(CMAKE_CXX_FLAGS_RELEASE	"-Wall -O3")
set(CMAKE_CXX_FLAGS_DEBUG	"-g")
set(CMAKE_BUILD_TYPE		Release)
add_definitions(-DNDEBUG)

file(GLOB sources *.cc)

add_executable(hubctrl ${sources} ${headers})
target_link_libraries(hubctrl TUUSB++ usb)

install(TARGETS hubctrl RUNTIME DESTINATION bin)

>>>>>>> d848d90d805a101b63d2d5f2d724c9aedf747dd6
