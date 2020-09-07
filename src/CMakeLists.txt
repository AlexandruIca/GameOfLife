set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sdl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/view.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/preview_scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gol_scene.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:GOL_DEBUG> $<$<CONFIG:Release>:GOL_RELEASE>)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE project::options project::warnings SDL2::SDL2 glad::glad
                                                    spdlog::spdlog glm::glm)
