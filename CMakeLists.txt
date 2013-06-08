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

#also, elf and dwarf for BGP...
set(ELF_HOME /usr)
set(ELF_INCLUDE_PATH   ${ELF_HOME}/include)
include_directories(${ELF_INCLUDE_PATH})
set(DWARF_HOME /usr/global/tools/dyninst/libdwarf/chaos_4_x86_64_ib/libdwarf-20091230)
set(DWARF_INCLUDE_PATH   ${DWARF_HOME}/include)
include_directories(${DWARF_INCLUDE_PATH})

#find_package(stackwalker)   #this does not exist yet so set variables manually
set(STACKWALKER_HOME           /usr/global/tools/dyninst/stackwalker/chaos_4_x86_64_ib/stackwalker-2.0)
#set(STACKWALKER_PLATFORM       x86_64-unknown-linux2.4)
#set(STACKWALKER_HOME           /usr/global/tools/dyninst/stackwalker/bgp_cnk/stackwalker-2.0)
#set(STACKWALKER_PLATFORM       ppc32_bgp_compute)
set(STACKWALKER_INCLUDE_PATH   ${STACKWALKER_HOME}/include)
set(STACKWALKER_LIBRARIES
  ${STACKWALKER_HOME}/${STACKWALKER_PLATFORM}/lib/libstackwalk.so
  ${STACKWALKER_HOME}/${STACKWALKER_PLATFORM}/lib/libsymtabAPI.so
  iberty ${DWARF_HOME}/lib/libdwarf.so ${ELF_HOME}/lib64/libelf.so)
add_definitions(-DCALLPATH_HAVE_SYMTAB)

include_directories(${STACKWALKER_INCLUDE_PATH})

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
