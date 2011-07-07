#
# The Callpath class itself is a separate library b/c it's used by 
# tools and other libraries (like libeffort).
#
set(CALLPATH_SOURCES
	Callpath.cpp
	FrameId.cpp
	ModuleId.cpp
	FrameInfo.cpp
	Translator.cpp
	string_utils.cpp)

if (LIBRA_HAVE_SW)
  list (APPEND CALLPATH_SOURCES CallpathRuntime.cpp)
endif()

add_library(callpath ${CALLPATH_SOURCES})
target_link_libraries(callpath)
# add stackwalker here.
#target_link_libraries(callpath stackwalker)
target_link_libraries(callpath "/g/g14/pearce8/opt/sw-1.2beta5/x86_64-unknown-linux2.4/lib/libstackwalk.so")
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
	string_utils.h)

install(FILES ${CALLPATH_HEADERS} DESTINATION include)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${SW_CPPFLAGS}
	${MPI_INCLUDE_PATH}
	${Boost_INCLUDE_DIR})
