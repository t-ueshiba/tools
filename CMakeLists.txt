cmake_minimum_required(VERSION 3.0)
project(tools)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

enable_language(CXX)
set(CMAKE_CXX_STANDARD		17)
set(CMAKE_CXX_STANDARD_REQUIRED	ON)
set(CMAKE_CXX_EXTENSIONS	OFF)
set(CMAKE_CXX_FLAGS_RELEASE	"-Wall -O3")
set(CMAKE_CXX_FLAGS_DEBUG	"-g")
set(CMAKE_BUILD_TYPE		Release)
add_definitions(-DNDEBUG)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} "/usr/local/share/cmake/TU")
include(FindSIMD)
FindSIMD()

if(AVX2_FOUND)
  add_definitions(-DAVX2)
  set(CMAKE_CXX_FLAGS -mavx2)
elseif(AVX_FOUND)
  add_definitions(-DAVX)
  set(CMAKE_CXX_FLAGS -mavx)
elseif(SSE4_2_FOUND)
  add_definitions(-DSSE4)
  set(CMAKE_CXX_FLAGS "-msse4.2 -mpopcnt")
elseif(SSSE3_FOUND)
  add_definitions(-DSSSE3)
  set(CMAKE_CXX_FLAGS "-msse3 -mssse3")
elseif(SSE3_FOUND)
  add_definitions(-DSSE3)
  set(CMAKE_CXX_FLAGS -msse3)
elseif(SSE2_FOUND)
  add_definitions(-DSSE2)
  set(CMAKE_CXX_FLAGS -msse2)
elseif(NEON_FOUND)
  add_definitions(-DNEON)
  set(CMAKE_CXX_FLAGS "-mcpu=cortex-a7 -mtune=cortex-a7 -mfpu=neon -funsafe-math-optimizations")
endif()

include_directories(${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory(embedCalib)
add_subdirectory(epicheck)
add_subdirectory(pbmheader)
add_subdirectory(pbmrectify)
add_subdirectory(pbmselect)
add_subdirectory(pbmrot)
add_subdirectory(pbmdecode)
add_subdirectory(fv)
#add_subdirectory(flowIIDCcamera)
#add_subdirectory(recIIDCcamera)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  add_subdirectory(flowV4L2camera)
  add_subdirectory(recV4L2camera)
  add_subdirectory(reset1394)
#  add_subdirectory(hubctrl)
endif()
