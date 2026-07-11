cmake_minimum_required(VERSION 3.20)
project(polytest_host_c C)

set(POLYTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_executable(host_c_tests
  main.c
  ${POLYTEST_ROOT}/harness/c/polytest_core.c
  ${POLYTEST_ROOT}/harness/c/polytest_assert.c
)

target_include_directories(host_c_tests PRIVATE
  ${POLYTEST_ROOT}/harness/include
)

# Size profile: tiny | small | full (default full when unset / "full")
set(POLYTEST_PROFILE "full" CACHE STRING "PolyTest size profile: tiny|small|full")
set_property(CACHE POLYTEST_PROFILE PROPERTY STRINGS tiny small full)

if(POLYTEST_PROFILE STREQUAL "tiny")
  target_compile_definitions(host_c_tests PRIVATE POLYTEST_PROFILE_TINY)
elseif(POLYTEST_PROFILE STREQUAL "small")
  target_compile_definitions(host_c_tests PRIVATE POLYTEST_PROFILE_SMALL)
elseif(POLYTEST_PROFILE STREQUAL "full")
  target_compile_definitions(host_c_tests PRIVATE POLYTEST_PROFILE_FULL)
else()
  message(FATAL_ERROR "POLYTEST_PROFILE must be tiny, small, or full (got '${POLYTEST_PROFILE}')")
endif()

# Default host demo uses human-readable output; switch off for COBS+CLI.
# Tiny profile forces text output regardless.
option(POLYTEST_MINIMAL_PRINT "Human-readable output" ON)
if(POLYTEST_MINIMAL_PRINT OR POLYTEST_PROFILE STREQUAL "tiny")
  target_compile_definitions(host_c_tests PRIVATE POLYTEST_MINIMAL_PRINT)
endif()

target_compile_features(host_c_tests PRIVATE c_std_11)
