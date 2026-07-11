cmake_minimum_required(VERSION 3.20)
project(polytest_host_fff C)

set(POLYTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

set(POLYTEST_PROFILE "small" CACHE STRING "PolyTest size profile")
set(POLYTEST_MINIMAL_PRINT ON CACHE BOOL "Human-readable output")

include(${POLYTEST_ROOT}/cmake/PolyTest.cmake)

add_executable(host_fff_tests main.c)
target_include_directories(host_fff_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${POLYTEST_ROOT}/plugins/extension/fff_fakes
)
target_link_libraries(host_fff_tests PRIVATE polytest_core)
target_compile_features(host_fff_tests PRIVATE c_std_11)
