add_executable(pack-test pack_test.C)
target_link_libraries(pack-test callpath)

add_executable(runtime-test runtime_test.C)
target_link_libraries(runtime-test callpath)
#???runtime_test_LDFLAGS = $(SW_LDFLAGS) $(SW_RPATH)

#????
#INCLUDES = \
#	$(MPI_CXXFLAGS) \
#	-I$(top_srcdir)/src



