add_executable(ring_buffer_test ${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer_test.cpp)
target_compile_features(ring_buffer_test PRIVATE cxx_std_17)
target_include_directories(ring_buffer_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/)
target_link_libraries(ring_buffer_test PRIVATE doctest::doctest gol_thread)
add_test(ring_buffer ring_buffer_test)
