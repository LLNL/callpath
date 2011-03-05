
if HAVE_MPI
MPI_EXES = pack-test
endif

noinst_PROGRAMS = $(MPI_EXES) runtime-test

pack_test_SOURCES = pack_test.C
pack_test_LDADD = ../src/libcallpath.la

runtime_test_SOURCES = runtime_test.C
runtime_test_LDADD = ../src/libcallpath.la
runtime_test_LDFLAGS = $(SW_LDFLAGS) $(SW_RPATH)

INCLUDES = \
	$(MPI_CXXFLAGS) \
	-I$(top_srcdir)/src

