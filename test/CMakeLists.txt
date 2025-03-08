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
target_sources(fixed_string_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/fixed_string_test.cpp
                                         ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(fixed_string_test PRIVATE mtp::fixed_string Catch2::Catch2)
target_compile_features(fixed_string_test PRIVATE cxx_std_20)

if(MTP_BUILD_MODULE)
  target_compile_definitions(fixed_string_test PRIVATE MTP_BUILD_MODULE MTP)
  set_target_properties(fixed_string_test PROPERTIES CXX_SCAN_FOR_MODULES ON)
endif()
if(MTP_USE_STD_MODULE)
  target_compile_definitions(fixed_string_test PRIVATE MTP_USE_STD_MODULE)
endif()

include(CTest)
include(Catch)
catch_discover_tests(fixed_string_test)
