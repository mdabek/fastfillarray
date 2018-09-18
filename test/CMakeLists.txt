cmake_minimum_required(VERSION 3.10)

enable_testing()
find_package(GTest REQUIRED)

add_compile_options(
    "-Wall" "-Wpedantic" "-Wextra" "-fexceptions"
      )
 # "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
add_executable(test_ffarray test_fastfillarray.cpp)

set_target_properties(test_ffarray PROPERTIES CXX_STANDARD 17)
target_compile_features(test_ffarray
                        PRIVATE cxx_std_17)
target_link_libraries(test_ffarray GTest::GTest GTest::Main)
target_include_directories(test_ffarray
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/)

add_executable(test_navarroarray test_navaroarray.cpp)
set_target_properties(test_navarroarray PROPERTIES CXX_STANDARD 17)
target_compile_features(test_navarroarray
                        PRIVATE cxx_std_17)
target_link_libraries(test_navarroarray PRIVATE GTest::GTest GTest::Main)
target_include_directories(test_navarroarray
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/)

add_test(FastFillArrayTests test_ffarray)
add_test(NavaroArrayTests test_navarroarray)
