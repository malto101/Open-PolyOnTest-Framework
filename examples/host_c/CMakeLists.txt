cmake_minimum_required(VERSION 3.20)
project(polytest_host_c C)

set(POT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_executable(host_c_tests
  main.c
  ${POT_ROOT}/harness/c/polytest_core.c
  ${POT_ROOT}/harness/c/polytest_assert.c
)

target_include_directories(host_c_tests PRIVATE
  ${POT_ROOT}/harness/include
)

# Size profile: tiny | small | full (default full when unset / "full")
set(POT_PROFILE "full" CACHE STRING "PolyTest size profile: tiny|small|full")
set_property(CACHE POT_PROFILE PROPERTY STRINGS tiny small full)

if(POT_PROFILE STREQUAL "tiny")
  target_compile_definitions(host_c_tests PRIVATE POT_PROFILE_TINY)
elseif(POT_PROFILE STREQUAL "small")
  target_compile_definitions(host_c_tests PRIVATE POT_PROFILE_SMALL)
elseif(POT_PROFILE STREQUAL "full")
  target_compile_definitions(host_c_tests PRIVATE POT_PROFILE_FULL)
else()
  message(FATAL_ERROR "POT_PROFILE must be tiny, small, or full (got '${POT_PROFILE}')")
endif()

# Default host demo uses human-readable output; switch off for COBS+CLI.
# Tiny profile forces text output regardless.
option(POT_MINIMAL_PRINT "Human-readable output" ON)
if(POT_MINIMAL_PRINT OR POT_PROFILE STREQUAL "tiny")
  target_compile_definitions(host_c_tests PRIVATE POT_MINIMAL_PRINT)
endif()

target_compile_features(host_c_tests PRIVATE c_std_11)

# Target for testing crash behavior
add_executable(host_c_crash_tests
  main.c
  ${POT_ROOT}/harness/c/polytest_core.c
  ${POT_ROOT}/harness/c/polytest_assert.c
)
target_compile_definitions(host_c_crash_tests PRIVATE POT_TEST_CRASH POT_PROFILE_FULL POT_MINIMAL_PRINT)
target_include_directories(host_c_crash_tests PRIVATE ${POT_ROOT}/harness/include)
target_compile_features(host_c_crash_tests PRIVATE c_std_11)

# Target for testing hang/timeout behavior
add_executable(host_c_hang_tests
  main.c
  ${POT_ROOT}/harness/c/polytest_core.c
  ${POT_ROOT}/harness/c/polytest_assert.c
)
target_compile_definitions(host_c_hang_tests PRIVATE POT_TEST_HANG POT_PROFILE_FULL POT_MINIMAL_PRINT)
target_include_directories(host_c_hang_tests PRIVATE ${POT_ROOT}/harness/include)
target_compile_features(host_c_hang_tests PRIVATE c_std_11)
