# PolyTest CMake helpers (FetchContent / add_subdirectory)
# SPDX-License-Identifier: Apache-2.0
#
# Usage:
#   set(POLYTEST_PROFILE tiny)   # or small / full
#   option(POLYTEST_MINIMAL_PRINT ... ON)
#   include(cmake/PolyTest.cmake)
#   target_link_libraries(my_tests PRIVATE polytest_core)

if(NOT TARGET polytest_core)
  add_library(polytest_core STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../harness/c/polytest_core.c"
    "${CMAKE_CURRENT_LIST_DIR}/../harness/c/polytest_assert.c"
  )
  target_include_directories(polytest_core PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/../harness/include"
  )
  target_compile_features(polytest_core PUBLIC c_std_11)

  if(NOT DEFINED POLYTEST_PROFILE)
    set(POLYTEST_PROFILE "full")
  endif()
  if(POLYTEST_PROFILE STREQUAL "tiny")
    target_compile_definitions(polytest_core PUBLIC POLYTEST_PROFILE_TINY)
  elseif(POLYTEST_PROFILE STREQUAL "small")
    target_compile_definitions(polytest_core PUBLIC POLYTEST_PROFILE_SMALL)
  elseif(POLYTEST_PROFILE STREQUAL "full")
    target_compile_definitions(polytest_core PUBLIC POLYTEST_PROFILE_FULL)
  endif()

  if(POLYTEST_MINIMAL_PRINT OR POLYTEST_PROFILE STREQUAL "tiny")
    target_compile_definitions(polytest_core PUBLIC POLYTEST_MINIMAL_PRINT)
  endif()
endif()
