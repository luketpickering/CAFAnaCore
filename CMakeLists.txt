############   top level config
cmake_minimum_required(VERSION 3.14)

project(CAFAnaCore)

find_package(cetmodules REQUIRED)
include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
              -fdiagnostics-color -grecord-gcc-switches -Wall -Wextra
)

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

set(CMAKE_CXX_FLAGS_DEBUG "-g -Og")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")
# notes for release:
# -O3 yields linker errors in some versions of gcc.
#   it's also apparently unstable across some architectures anyway,
#    and apparently doesn't often offer much in the way of performance increases.
#    so let's not cause ourselves headaches
# NDEBUG needed to ensure assert() causes an abort() even in optimized builds
# boost is noisy pointing out deprecated declarations
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -g -Wno-deprecated-declarations")

# default to 'release' build if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

cet_make_library(LIBRARY_NAME CAFAnaCore_options INTERFACE NO_SOURCE)

# make Stan dependency optional, but use it by default
option(CAFAnaCore_USE_STAN "Build against Stan?" OFF)
option(CAFAnaCore_USE_STAN_THREADS "Use threading with Stan?" OFF)
option(CAFAnaCore_USE_Ifdhc "Use Ifdhc?" OFF)

# Bake version number into the library (CAFAna/Core/Version.cxx)
execute_process(COMMAND git describe --tags OUTPUT_VARIABLE PROJECT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
SET(CAFAnaCore_VERSION ${PROJECT_VERSION})
target_compile_definitions(CAFAnaCore_options INTERFACE
  CAFAnaCore_VERSION=\"${CAFAnaCore_VERSION}\")

# Record what flags were used. Can dump with
# readelf -p .GCC.command.line libCAFAnaCoreExt.so
target_compile_definitions(CAFAnaCore_options INTERFACE _REENTRANT)

# non-negotiable dependencies
find_package(ROOT REQUIRED EXPORT)

# ROOT does things differently depending on whether it's built for C++17 or not :-\
string(REGEX REPLACE ".*c\\+\\+([0-9][0-9]).*" "\\1" ROOT_CXX_STANDARD ${ROOT_CXX_FLAGS})
if(ROOT_CXX_STANDARD LESS_EQUAL 14)
  message(WARNING "ROOT was built with C++${ROOT_CXX_STANDARD} standard. Overriding default C++ standard version (C++${CMAKE_CXX_STANDARD}) to use C++14 (lowest supported by this package).  Cross your fingers it builds...")
  set(CMAKE_CXX_STANDARD 14)
endif()

find_package(Eigen3 REQUIRED EXPORT)
find_package(Threads REQUIRED EXPORT)

option (SPACK "Building through Spack")
if (CAFAnaCore_USE_STAN)
  target_compile_definitions(CAFAnaCore_options CAFAnaCore_USE_STAN)
  cmake_policy(SET CMP0167 NEW) #don't use builtin FindBoost.cmake
  if (SPACK)
    find_package(Boost REQUIRED EXPORT)
    find_package(SUNDIALS REQUIRED EXPORT)
    find_package(TBB REQUIRED EXPORT)
  else()
    find_package(Boost REQUIRED MODULE EXPORT)
    find_package(Sundials REQUIRED MODULE EXPORT)
    find_package(TBB REQUIRED MODULE EXPORT)
  endif()
  # If we can't find "2019" in the version, ie it's newer than that
  if(TBB_VERSION AND NOT TBB_VERSION MATCHES 2019)
    target_compile_definitions(CAFAnaCore_options INTERFACE TBB_INTERFACE_NEW)
  endif()

  find_package(StanMath REQUIRED MODULE EXPORT)
  if(CAFAnaCore_USE_STAN_THREADS)
    target_compile_definitions(CAFAnaCore_options INTERFACE STAN_THREADS)
  endif()

  # this is clunky, but Stan-Math needs to be included before Eigen,
  # because it adds some type traits to Eigen templates for Stan types
  target_include_directories(CAFAnaCore_options INTERFACE ${STAN_MATH_INC})
  target_link_libraries(CAFAnaCore_options INTERFACE Boost::Boost TBB::tbb)
endif()

target_include_directories(CAFAnaCore_options INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(CAFAnaCore_options INTERFACE ROOT::Hist ROOT::Core Eigen3::Eigen Threads::Threads)

set(Ifdhc_FOUND FALSE)
if(CAFAnaCore_USE_Ifdhc)
  find_package(Ifdhc REQUIRED EXPORT)
  target_include_directories(CAFAnaCore_options INTERFACE ${IFDHC_INC})
endif()

###########   now go into the subdirs and do the actual work
add_subdirectory(CAFAna/Core)

cet_cmake_config()
