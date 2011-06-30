if (HAVE_MPI)
  MPI_EXES = pack-test
endif()

add_program(pack-test pack_test.C)
target_link_libraries(pack-test callpath)

add_program(runtime-test runtime_test.C)
target_link_libraries(runtime-test callpath)
#???runtime_test_LDFLAGS = $(SW_LDFLAGS) $(SW_RPATH)

#????
#INCLUDES = \
#	$(MPI_CXXFLAGS) \
#	-I$(top_srcdir)/src



