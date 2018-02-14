#
# Set up includes and linking for the stack trace library
# that we are using here.
#
if (CALLPATH_WALKER STREQUAL dyninst)
  include_directories(${Dyninst_INCLUDE_DIR})
  add_definitions(-DCALLPATH_USE_DYNINST)
  if (DEFINED SYMTAB_LIBRARY)
    add_definitions(-DCALLPATH_HAVE_SYMTAB)
  endif()
  set(WALKER_LIBRARIES ${STACKWALKER_LIBRARIES})

elseif (CALLPATH_WALKER STREQUAL backtrace)
  add_definitions(-DCALLPATH_USE_BACKTRACE)
  set(WALKER_LIBRARIES "")
endif()

#
# Include directories from external libraries
#
include_directories(
  ${PROJECT_BINARY_DIR}
  ${adept_utils_INCLUDE_PATH}
  ${MPI_INCLUDE_PATH})

#
# Headers -- these are installed into $prefix/include.
#
set(CALLPATH_HEADERS
	FrameId.h
	Callpath.h
	CallpathRuntime.h
    UniqueId.h
	ModuleId.h
	FrameInfo.h
	Translator.h
	safe_bool.h)

set(CALLPATH_SOURCES
	Callpath.C
	CallpathRuntime.C
	FrameId.C
	ModuleId.C
	FrameInfo.C
	Translator.C)

#
# Library source files.
#
add_static_and_shared_library(callpath ${CALLPATH_SOURCES})
target_link_libraries(
  callpath ${WALKER_LIBRARIES} adept_utils adept_cutils)
target_link_libraries(
  callpath_static ${WALKER_LIBRARIES} adept_utils_static adept_cutils_static)

#
# Things to install into the prefix.
#
install(TARGETS callpath callpath_static
        DESTINATION lib EXPORT callpath-libs)
install(FILES ${CALLPATH_HEADERS} DESTINATION include)


