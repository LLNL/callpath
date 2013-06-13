#
# The Callpath class itself is a separate library b/c it's used by
# tools and other libraries (like libeffort).
#
set(CALLPATH_SOURCES
	Callpath.C
	CallpathRuntime.C
	FrameId.C
	ModuleId.C
	FrameInfo.C
	Translator.C)

if (CALLPATH_HAVE_SW)
  list (APPEND CALLPATH_SOURCES CallpathRuntime.C)
endif()

add_library(callpath ${CALLPATH_SOURCES})
target_link_libraries(callpath ${STACKWALKER_LIBRARIES} adept_utils)
#if (CALLPATH_HAVE_MPI)
#  target_link_libraries(callpath ${MPI_LIBRARIES})
#endif()
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
  ${adept_utils_INCLUDE_PATH}
	${MPI_INCLUDE_PATH})
