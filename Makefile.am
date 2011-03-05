AUTOMAKE_OPTIONS = foreign   # Don't require NEWS, README, INSTALL, etc.emacs -
ACLOCAL_AMFLAGS = -I m4

# Build-time configuration of libraries.
include_HEADERS=callpath-config.h
DISTCLEANFILES = callpath-config.h _configs.sed

EXTRA_DIST = \
	LICENSE \
	README

if INSTALL_TESTS
TEST_DIR=tests
endif

SUBDIRS=src ${TEST_DIR}

