project(flowV4L2camera)

include_directories(${PROJECT_SOURCE_DIR}/../flowIIDCcamera)
file(GLOB sources *.cc)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} TUvV4L2++ TUV4L2++ TUv++ TUTools++)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
