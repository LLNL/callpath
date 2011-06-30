###############################################################
# Callpath
#
###############################################################
project(callpath)
cmake_minimum_required(VERSION 2.8)

# Version information comes from here.  Increment these when new stuff happens.
set(CALLPATH_MAJOR_VERSION 1)
set(CALLPATH_MINOR_VERSION 0)
set(CALLPATH_PATCH_VERSION 0)
set(CALLPATH_VERSION "${CALLPATH_MAJOR_VERSION}.${CALLPATH_MINOR_VERSION}.${CALLPATH_PATCH_VERSION}")

#find_package(stackwalker)   #this does not exist yet
#what can I replace it with??????????????????????????????????????????????
#include_directories(path)
#target_link_libraries(hardcode path to library here - or put -L)

# Build shared libraries by default.
get_property(DEFAULT_SHARED GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
option(BUILD_SHARED_LIBS "Build shared libraries?" ${DEFAULT_SHARED})

# Check for various timing functions, so we can support highest-resolution timers available.
include(CheckFunctionExists)

# Muster requires boost for matrices and random number generators.
find_package(Boost)

# Find the MPI library and set some definitions
# This line ensures that we skpi C++ headers altogether, avoiding unnecessary symbols in the .o files.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOMPI_SKIP_MPICXX -DMPICH_SKIP_MPICXX")
find_package(MPI QUIET)
if (MPI_FOUND)
  set(CALLPATH_HAVE_MPI TRUE)
endif()

set(CMAKE_REQUIRED_INCLUDES  time.h sys/time.h)
set(CMAKE_REQUIRED_LIBRARIES rt)
check_function_exists(clock_gettime CALLPATH_HAVE_CLOCK_GETTIME)

set(CMAKE_REQUIRED_INCLUDES  sys/time.h)
unset(CMAKE_REQUIRED_LIBRARIES)
check_function_exists(gettimeofday CALLPATH_HAVE_GETTIMEOFDAY)

set(CMAKE_REQUIRED_INCLUDES  mach/mach.h mach/mach_time.h)
unset(CMAKE_REQUIRED_LIBRARIES)
check_function_exists(mach_absolute_time CALLPATH_HAVE_MACH_TIME)

# Configure a CMake file and set it to be installed.  This file tells other projects
# how to use Adept Utils.
foreach (callpath)
  get_target_property(${lib}_LIBRARY_PATH ${lib} LOCATION)
  get_filename_component(${lib}_LIBRARY ${${lib}_LIBRARY_PATH} NAME)
endforeach()

# Configure a header file with all the variables we found.
configure_file(
  ${PROJECT_SOURCE_DIR}/callpath-config.h.in
  ${PROJECT_BINARY_DIR}/callpath-config.h)
install(FILES ${PROJECT_BINARY_DIR}/callpath-config.h DESTINATION include)

# Make callpath findable
foreach (lib callpath)
  get_target_property(${lib}_LIBRARY_PATH ${lib} LOCATION)
  get_filename_component(${lib}_LIBRARY ${${lib}_LIBRARY_PATH} NAME)
endforeach()
configure_file(
  ${PROJECT_SOURCE_DIR}/callpath-config.cmake.in
  ${PROJECT_BINARY_DIR}/callpath-config.cmake
  @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/callpath-config.cmake DESTINATION lib/cmake/callpath)
install(EXPORT callpath-libs                              DESTINATION lib/cmake/callpath)

# Configure subdirectories here, after everything else, so that they
# inherit variables set in the global scope.
add_subdirectory(src) 
