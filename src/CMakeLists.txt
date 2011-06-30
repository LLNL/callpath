#
# The Callpath class itself is a separate library b/c it's used by 
# tools and other libraries (like libeffort).
#
set(CALLPATH_SOURCES
	Callpath.C
	FrameId.C
	ModuleId.C
	FrameInfo.C
	Translator.C
	string_utils.C)

if (LIBRA_HAVE_SW)
  list (APPEND CALLPATH_SOURCES CallpathRuntime.C)
endif()

add_library(callpath ${CALLPATH_SOURCES})
target_link_libraries(callpath)
install(TARGETS callpath DESTINATION lib)
# add stackwalker here.
#target_link_libraries(callpath)

set(CALLPATH_HEADERS
	FrameId.h
	Callpath.h
	CallpathRuntime.h
  UniqueId.h
	ModuleId.h
	FrameInfo.h
	Translator.h
	safe_bool.h
	string_utils.h)

install(FILES ${CALLPATH_HEADERS} DESTINATION include)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${SW_CPPFLAGS}
	${MPI_INCLUDE_PATH}
	${Boost_INCLUDE_DIR})
