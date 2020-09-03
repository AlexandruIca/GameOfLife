set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/sdl.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE project::options project::warnings SDL2::SDL2 glad::glad
                                                    spdlog::spdlog)
