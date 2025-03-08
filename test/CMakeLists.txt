find_package(Catch2 2.5.0...<3 QUIET) # v2.5.0 for template test cases
if(NOT Catch2_FOUND)
  message(NOTICE "Catch2 (version 2.5.0 <= ... < 3 ) not found. Fetching from GitHub.")
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/catch2.git
    GIT_TAG v2.13.10
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(Catch2)
  list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)
endif()

add_executable(fixed_string_test)
target_sources(fixed_string_test PRIVATE fixed_string_test.cpp main.cpp)
target_link_libraries(fixed_string_test PRIVATE mtp::fixed_string Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(fixed_string_test)
