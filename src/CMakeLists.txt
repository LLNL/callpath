#
# The Callpath class itself is a separate library b/c it's used by 
# tools and other libraries (like libeffort).
#
set(CALLPATH_SOURCES
	Callpath.cpp
	CallpathRuntime.cpp
	FrameId.cpp
	ModuleId.cpp
	FrameInfo.cpp
	Translator.cpp
	string_utils.cpp
  io_utils.cpp)

if (CALLPATH_HAVE_SW)
  list (APPEND CALLPATH_SOURCES CallpathRuntime.cpp)
endif()

add_library(callpath ${CALLPATH_SOURCES})
target_link_libraries(callpath)
# add stackwalker here.
#target_link_libraries(callpath stackwalker)
target_link_libraries(callpath ${STACKWALKER_LIBRARIES})
if (CALLPATH_HAVE_MPI)
  target_link_libraries(callpath ${MPI_LIBRARIES})
endif()
install(TARGETS callpath DESTINATION lib EXPORT callpath-libs)

set(CALLPATH_HEADERS
	FrameId.h
	Callpath.h
	CallpathRuntime.h
  UniqueId.h
	ModuleId.h
	FrameInfo.h
	Translator.h
	safe_bool.h
	string_utils.h
  io_utils.h
  mpi_utils.h)

install(FILES ${CALLPATH_HEADERS} DESTINATION include)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${SW_CPPFLAGS}
	${MPI_INCLUDE_PATH})
