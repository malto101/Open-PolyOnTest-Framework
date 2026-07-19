cmake_minimum_required(VERSION 3.20)
project(pot_host_cpp LANGUAGES C CXX)

set(POT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_executable(host_cpp_tests
  main.cpp
  ${POT_ROOT}/harness/c/pot_core.c
  ${POT_ROOT}/harness/c/pot_assert.c
)

target_include_directories(host_cpp_tests PRIVATE
  ${POT_ROOT}/harness/include
  ${POT_ROOT}/harness/cpp
)

set(POT_PROFILE "full" CACHE STRING "PolyOnTest size profile: tiny|small|full")
set_property(CACHE POT_PROFILE PROPERTY STRINGS tiny small full)

if(POT_PROFILE STREQUAL "tiny")
  target_compile_definitions(host_cpp_tests PRIVATE POT_PROFILE_TINY)
elseif(POT_PROFILE STREQUAL "small")
  target_compile_definitions(host_cpp_tests PRIVATE POT_PROFILE_SMALL)
else()
  target_compile_definitions(host_cpp_tests PRIVATE POT_PROFILE_FULL)
endif()

option(POT_MINIMAL_PRINT "Human-readable output" ON)
if(POT_MINIMAL_PRINT OR POT_PROFILE STREQUAL "tiny")
  target_compile_definitions(host_cpp_tests PRIVATE POT_MINIMAL_PRINT)
endif()

target_compile_features(host_cpp_tests PRIVATE cxx_std_17 c_std_11)
set_source_files_properties(
  ${POT_ROOT}/harness/c/pot_core.c
  ${POT_ROOT}/harness/c/pot_assert.c
  PROPERTIES LANGUAGE C
)
