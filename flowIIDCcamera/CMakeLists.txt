project(flowIIDCcamera)

file(GLOB sources *.cc)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} TUvIIDC)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
