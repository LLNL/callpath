#############################################################
# This is the main autoconf file for building Libra.
#############################################################
AC_INIT
AC_CONFIG_SRCDIR([src/Callpath.C])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(m4)

AM_INIT_AUTOMAKE(callpath,1.0)
AC_CONFIG_HEADERS([config.h])
AX_PREFIX_CONFIG_H([callpath-config.h])

# Programs we need
AC_PROG_CXX
AC_PROG_INSTALL

# now that we've specified deps, declare libtool
LT_INIT
AC_SUBST(LIBTOOL_DEPS)

#
# Test for various C++ libraries
#
AC_LANG_PUSH([C++])
    # Make sure we can compile MPI programs in C++
    LX_FIND_MPI
    AM_CONDITIONAL([HAVE_MPI], [test "x$have_CXX_mpi" == xyes])

    # Check for Wisconsin ParaDyn Tools (this includes checking for dependencies)
    LX_PARADYN_TOOLS
    AM_CONDITIONAL([HAVE_SW], [test "x$have_stackwalk" = xyes])
    AM_CONDITIONAL([HAVE_SYMTAB], [test "x$have_symtabAPI" = xyes])
AC_LANG_POP


# If this is provided, we use PMPI bindings instead of MPI bindings for communication.
AC_ARG_WITH([pmpi-bindings], 
    AS_HELP_STRING([--with-pmpi-bindings], 
                   [If provided, build with PMPI bindings instead of raw MPI bindings.  Useful for including this library in tools.]),
    [    if [[ "x$withval" = xyes ]]; then
             pmpi_bindings=yes
         else
             pmpi_bindings=no
         fi
     ],
    [    pmpi_bindings=no
     ]
)
if [[ "x$pmpi_bindings" = xyes ]]; then
    AC_DEFINE([USE_PMPI], [1], [Define to 1 to use PMPI bindings instead of raw MPI bindings.])
fi


echo
echo "========================================================"
echo "==        Callpath: final build configuration         =="
echo "========================================================"
echo "External Library Support:"
echo "  MPI ............................................ $have_CXX_mpi"
echo "  ParaDyn - Common ............................... $have_common"
echo "  ParaDyn - SymtabAPI ............................ $have_symtabAPI"
echo "  ParaDyn - Stackwalker .......................... $have_stackwalk"
echo
echo "Build Options:"
echo "  PMPI Bindings ................................... $pmpi_effort"
echo "========================================================"
echo

AC_CONFIG_FILES([Makefile \
                 src/Makefile])
AC_OUTPUT
