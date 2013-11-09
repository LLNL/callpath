###############################################################
# Callpath
###############################################################
project(callpath)
cmake_minimum_required(VERSION 2.8)

# Version information comes from here.  Increment these when new stuff happens.
set(CALLPATH_MAJOR_VERSION 1)
set(CALLPATH_MINOR_VERSION 0)
set(CALLPATH_PATCH_VERSION 0)
set(CALLPATH_VERSION "${CALLPATH_MAJOR_VERSION}.${CALLPATH_MINOR_VERSION}.${CALLPATH_PATCH_VERSION}")

# Add our module directory to the include path.
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

#
# Let the user choose the walker mechanism.
#
set(CALLPATH_WALKER auto CACHE STRING
  "Underlying Stackwalker to use.  Options are (auto, dyninst, backtrace).  Default is auto.")

string(TOLOWER "${CALLPATH_WALKER}" requested)
set(WALKER_FOUND FALSE)
foreach (walker Dyninst Backtrace)
  string(TOLOWER "${walker}" cur)
  if (cur STREQUAL "${requested}" OR requested STREQUAL "auto")
    find_package(${walker})
    if (${walker}_FOUND)
      set(CALLPATH_WALKER ${cur})
      set(WALKER_FOUND TRUE)
    endif()
  endif()
endforeach()

if (NOT WALKER_FOUND)
  message(FATAL_ERROR "No suitable stackwalker was found!")
endif()

# Build shared libraries by default.
get_property(DEFAULT_SHARED GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
option(BUILD_SHARED_LIBS "Build shared libraries?" ${DEFAULT_SHARED})

# RPATH setup.  Default is to rpath everything.  Set the option to
# false if you don't want this behavior.
option(CMAKE_INSTALL_RPATH_USE_LINK_PATH "Add rpath for all dependencies." TRUE)

function(notify_package name)
  if (${name}_FOUND)
    message(STATUS "Found ${name} in ${${name}_DIR}.")
  else()
    message(STATUS "${name} package not found.")
  endif()
endfunction()

find_package(adept_utils REQUIRED)
notify_package(adept_utils)

# Find the MPI library and set some definitions
# This line ensures that we skpi C++ headers altogether, avoiding unnecessary symbols in the .o files.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOMPI_SKIP_MPICXX -DMPICH_SKIP_MPICXX")
find_package(MPI REQUIRED)
if (MPI_FOUND)
  set(CALLPATH_HAVE_MPI TRUE)
endif()

# Configure a CMake file and set it to be installed.  This file tells other projects
# how to use adept_utils.
foreach (lib callpath)
  get_target_property(${lib}_LIBRARY_PATH ${lib} LOCATION)
  get_filename_component(${lib}_LIBRARY ${${lib}_LIBRARY_PATH} NAME)
endforeach()

# Configure a header file with all the variables we found.
configure_file(
  ${PROJECT_SOURCE_DIR}/callpath-config.h.in
  ${PROJECT_BINARY_DIR}/callpath-config.h)
install(FILES ${PROJECT_BINARY_DIR}/callpath-config.h DESTINATION include)

# Make callpath findable
configure_file(
  ${PROJECT_SOURCE_DIR}/callpath-config.cmake.in
  ${PROJECT_BINARY_DIR}/callpath-config.cmake
  @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/callpath-config.cmake DESTINATION share/cmake/callpath)
install(EXPORT callpath-libs                              DESTINATION share/cmake/callpath)

# Configure subdirectories here, after everything else, so that they
# inherit variables set in the global scope.
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(scripts)
