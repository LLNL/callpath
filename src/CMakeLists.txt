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
	Translator.cpp)

if (CALLPATH_HAVE_SW)
  list (APPEND CALLPATH_SOURCES CallpathRuntime.cpp)
endif()

add_library(callpath ${CALLPATH_SOURCES})
target_link_libraries(callpath ${STACKWALKER_LIBRARIES} adept-utils)
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
	safe_bool.h)

install(FILES ${CALLPATH_HEADERS} DESTINATION include)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${SW_CPPFLAGS}
  ${AdeptUtils_INCLUDE_PATH}
	${MPI_INCLUDE_PATH})
