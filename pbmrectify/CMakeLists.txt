project(pbmrectify)

file(GLOB sources *.cc)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} TUTools)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
