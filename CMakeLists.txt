cmake_minimum_required(VERSION 3.22)

project(mtp_fixed_string LANGUAGES CXX)

option(MTP_BUILD_TEST "Build tests" ${PROJECT_IS_TOP_LEVEL})
option(MTP_NO_EXCEPTIONS "Disable exceptions" OFF)

add_library(mtp_fixed_string INTERFACE)
add_library(mtp::fixed_string ALIAS mtp_fixed_string)
target_include_directories(mtp_fixed_string INTERFACE include)

if(MTP_NO_EXCEPTIONS)
  target_compile_definitions(mtp_fixed_string INTERFACE MTP_NO_EXCEPTIONS)
endif()

if(MTP_BUILD_TEST)
  add_subdirectory(test)
endif()
