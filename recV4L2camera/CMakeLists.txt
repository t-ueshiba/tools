project(recV4L2Ccamera)

include_directories(${PROJECT_SOURCE_DIR}/../recIIDCcamera)
file(GLOB sources *.cc)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} TUvV4L2++ TUV4L2++ TUv++ TUTools++)
