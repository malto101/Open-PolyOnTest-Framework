cmake_minimum_required(VERSION 3.20)
project(polytest_host_cpp LANGUAGES C CXX)

set(POLYTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_executable(host_cpp_tests
  main.cpp
  ${POLYTEST_ROOT}/harness/c/polytest_core.c
  ${POLYTEST_ROOT}/harness/c/polytest_assert.c
)

target_include_directories(host_cpp_tests PRIVATE
  ${POLYTEST_ROOT}/harness/include
  ${POLYTEST_ROOT}/harness/cpp
)

target_compile_definitions(host_cpp_tests PRIVATE POLYTEST_MINIMAL_PRINT)
target_compile_features(host_cpp_tests PRIVATE cxx_std_17 c_std_11)
set_source_files_properties(
  ${POLYTEST_ROOT}/harness/c/polytest_core.c
  ${POLYTEST_ROOT}/harness/c/polytest_assert.c
  PROPERTIES LANGUAGE C
)
