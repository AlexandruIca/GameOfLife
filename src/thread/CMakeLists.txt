set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(gol_thread STATIC ${CMAKE_CURRENT_SOURCE_DIR}/thread_pool.cpp)
target_compile_features(gol_thread PUBLIC cxx_std_17)
