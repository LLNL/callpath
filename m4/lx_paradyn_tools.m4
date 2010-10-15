#################################################################################################
# Copyright (c) 2010, Lawrence Livermore National Security, LLC.  
# Produced at the Lawrence Livermore National Laboratory  
# Written by Todd Gamblin, tgamblin@llnl.gov.
# LLNL-CODE-417602
# All rights reserved.  
# 
# This file is part of Libra. For details, see http://github.com/tgamblin/libra.
# Please also read the LICENSE file for further information.
# 
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
# 
#  * Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the disclaimer below.
#  * Redistributions in binary form must reproduce the above copyright notice, this list of
#    conditions and the disclaimer (as noted below) in the documentation and/or other materials
#    provided with the distribution.
#  * Neither the name of the LLNS/LLNL nor the names of its contributors may be used to endorse
#    or promote products derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# LAWRENCE LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#################################################################################################

#
# LX_PARADYN_TOOLS
#  ------------------------------------------------------------------------
# Tests for everything needed for paradyn tools API: dwarf, xml2, symtabAPI,
# and DynStackwalkerAPI.  Sets have_symtabAPI, have_stackwalk if either is found.
#
# Output shell variables:
#   have_stackwalk    Set to 'yes' or 'no'
#   have_symtabAPI    Set to 'yes' or 'no'
#   have_common       Set to 'yes' or 'no'
#
# AC_SUBST variables:
#   COMMON_CPPFLAGS    SYMTAB_CPPFLAGS      SW_CPPFLAGS 
#   COMMON_LDFLAGS     SYBTAB_LDFLAGS       SW_LDFLAGS  
#   COMMON_RPATH       SYMTAB_RPATH         SW_RPATH    
#
# AC_DEFINES:
#   HAVE_COMMON        HAVE_SYMTAB          HAVE_SW
#
AC_DEFUN([LX_PARADYN_TOOLS],
[
  AC_ARG_WITH(
     [paradyn],
     AS_HELP_STRING([--with-paradyn=<dir>], 
                    [Path to the platform-specific installation directory of ParaDyn tools. Includes, etc. are inferred.]),
  [echo "checking for ParaDyn tools..."
   if [[ -d "$withval" ]]; then
     if [[ -d "$withval/include" ]]; then
         paradyn_home="$withval"
         paradyn_include="$paradyn_home/include"
         paradyn_lib="$paradyn_home/lib"
     else
         paradyn_home=`echo ${withval} | ${SED} 's_/[[^/]]*\$__'`    
         paradyn_platform=`echo ${withval} | ${SED} 's_.*\/\([[^/]]*\)$_\1_'`
         paradyn_include="$paradyn_home/include"
         paradyn_lib="$paradyn_home/$paradyn_platform/lib"
     fi

     if [[ -d "$paradyn_include" ]]; then
       if [[ "x$paradyn_platform" != "xppc32_bgcompute" ]]; then
         LX_LIB_ELF([$paradyn_lib])
         LX_LIB_DWARF([$paradyn_lib])
         LX_LIB_XML2([$paradyn_lib])
       fi

       # Adding boost headers here, just in case we need them (current version of
       # Stackwalker wants them, but Matt tells me it's not a permanent dependence)
       paradyn_headers="-I$paradyn_include ${BOOST_CPPFLAGS}"
       LX_HEADER_SUBST(stackwalk, [walker.h],   SW,     [$paradyn_headers])
       LX_HEADER_SUBST(symtabAPI, [Symtab.h],   SYMTAB, [$paradyn_headers])
       LX_HEADER_SUBST(common,    [dyntypes.h], COMMON, [$paradyn_headers])

       if [[ "x${SW}${SYMTAB}${COMMON}" != x ]]; then 
         PARADYN_CPPFLAGS="$paradyn_headers"
         AC_SUBST(PARADYN_CPPFLAGS)
       fi

       LX_LIB_SUBST(iberty_pic, cplus_demangle, IBERTY, [$paradyn_lib])
       if [[ "x$have_iberty_pic" = "xno" ]]; then
           LX_LIB_SUBST(iberty, cplus_demangle, IBERTY, [$paradyn_lib])
       fi

       common_libs="-lcommon $XML2_LDFLAGS $DWARF_LDFLAGS $IBERTY_LDFLAGS"
       LX_LIB_SUBST(common, _init, COMMON, [$paradyn_lib], [$common_libs])

       symtab_libs="-lsymtabAPI $COMMON_LDFLAGS"
       LX_LIB_SUBST(symtabAPI, _init, SYMTAB, [$paradyn_lib], [$symtab_libs])

       # By the third lib in the chain of dependencies here, this is kind of nasty.
       # Need this if statement for the case where we have common but not symtab.
       # Consider restructuring this if we get more stackwalk libs.
       if [[ "x$have_symtabAPI" = xyes ]]; then
           stackwalk_libs="-lstackwalk $SYMTAB_LDFLAGS"
       else 
           stackwalk_libs="-lstackwalk $COMMON_LDFLAGS"
       fi
       LX_LIB_SUBST(stackwalk, _init, SW, [$paradyn_lib], [$stackwalk_libs])         

     else
       echo "couldn't find ParaDyn headers in $paradyn_include."
       have_common=no
       have_symtabAPI=no
       have_stackwalk=no
     fi
   else 
     echo "couldn't find ParaDyn tools in $withval."
     have_common=no
     have_symtabAPI=no
     have_stackwalk=no
   fi     

   if test "x$have_common" != xno; then 
     AC_DEFINE([HAVE_COMMON], [1], [Define to 1 if you have dyninst common library.])
     have_common=yes;
   fi
   if test "x$have_symtabAPI" != xno; then
     AC_DEFINE([HAVE_SYMTAB], [1], [Define to 1 if you have dyninst SymtabAPI library.])
     have_symtabAPI=yes;
   fi
   if test "x$have_stackwalk" != xno; then
     AC_DEFINE([HAVE_SW], [1], [Define to 1 if you have StackwalkerAPI library.])
     have_stackwalk=yes;
   fi
  ],
  [echo "path to ParaDyn tools not provided."
   have_common=no
   have_symtabAPI=no
   have_stackwalk=no
  ])
])

