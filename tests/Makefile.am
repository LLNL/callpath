
noinst_PROGRAMS = runtime-test
runtime_test_SOURCES = runtime_test.C
runtime_test_LDADD = ../src/libcallpath.la
runtime_test_LDFLAGS = \
  -avoid-version \
	$(SW_LDFLAGS) $(SW_RPATH)

INCLUDES = \
	$(MPI_CXXFLAGS) \
	-I$(top_srcdir)/src


