#
# The Callpath class itself is a separate library b/c it's used by 
# tools and other libraries (like libeffort).
#

if HAVE_SW
SW_ONLY_SRCS = CallpathRuntime.C 
endif

lib_LTLIBRARIES = libcallpath.la
libcallpath_la_SOURCES = \
	Callpath.C \
	FrameId.C \
	ModuleId.C \
	FrameInfo.C \
	Translator.C \
	string_utils.C \
	$(SW_ONLY_SRCS)
libcallpath_la_LDFLAGS = \
  -avoid-version \
	$(SW_LDFLAGS) $(SW_RPATH)

include_HEADERS = \
	FrameId.h \
	Callpath.h \
	CallpathRuntime.h \
  UniqueId.h \
	ModuleId.h \
	FrameInfo.h \
	Translator.h \
	safe_bool.h \
	string_utils.h

INCLUDES = \
	$(SW_CPPFLAGS) \
	$(MPI_CXXFLAGS) \
	-I$(top_srcdir)/libwavelet \
	$(BOOST_CPPFLAGS) \
	-I$(top_srcdir)

