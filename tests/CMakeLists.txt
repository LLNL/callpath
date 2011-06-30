add_executable(pack-test pack_test.C)
target_link_libraries(pack-test callpath ${MPI_C_LIBRARIES})

add_executable(runtime-test runtime_test.C)
target_link_libraries(runtime-test callpath ${MPI_C_LIBRARIES})
#???runtime_test_LDFLAGS = $(SW_LDFLAGS) $(SW_RPATH)

#????
#INCLUDES = \
#	$(MPI_CXXFLAGS) \
#	-I$(top_srcdir)/src



