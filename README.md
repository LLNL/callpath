Callpath
==========================================
by Todd Gamblin, tgamblin@llnl.gov

This is the callpath library, a tool for representing callpath information
consistently in distributed-memory parallel applications.

See the `LICENSE` file for license and distribution information.

Directory structure
------------------------------------------
    callpath/
      build/     # contains sample cmake build line scripts
      cmake/     # Extra build files an find modules
      doc/       # target directory for doxygen
      scripts/   # scripts to be installed to $prefix/bin
      src/       # Callpath library source code
      tests/     # Test programs for callpath library

Building
------------------------------------------
To build, you need to use cmake.  We recommend that you build out of source.
That is, you should make a directory to build in first.  e.g., to build with
backtrace:

    mkdir $SYS_TYPE && cd $SYS_TYPE
    cmake \
        -D CMAKE_INSTALL_PREFIX=/path/to/install \
        -D CALLPATH_WALKER=backtrace  \
        -D CMAKE_BUILD_TYPE=RelWithDebInfo \
        ..
    make -j8
    make -j8 install

To build with Dyninst you need to tell CMake where dyninst and its deps live, e.g.:

    cmake \
        -D CMAKE_INSTALL_PREFIX=/path/to/install \
        -D CMAKE_BUILD_TYPE=RelWithDebInfo \
        -D CALLPATH_WALKER=dyninst \
        -D Dyninst_DIR=/usr/global/tools/dyninst/stackwalker/$SYS_TYPE/current \
        -D DWARF_DIR=/usr/global/tools/dyninst/libdwarf/$SYS_TYPE/dwarf-20111030 \
        ..

To build on Blue Gene machines, you will need to use a toolchain file.  We include
some sample toolchain files in `cmake/Toolchain`:

    cmake \
        -D CMAKE_TOOLCHAIN_FILE=../cmake/Toolchain/BlueGeneQ-gnu.cmake \
        -D CMAKE_INSTALL_PREFIX=/path/to/install \
        -D CMAKE_BUILD_TYPE=RelWithDebInfo \
        -D CALLPATH_WALKER=dyninst \
        -D Dyninst_DIR=/usr/global/tools/dyninst/stackwalker/$SYS_TYPE/current \
        -D DWARF_DIR=/usr/global/tools/dyninst/libdwarf/$SYS_TYPE/dwarf-20111030 \
        ..

You can find some more sample CMake command lines in the scripts in the `build`
directory.

Directory structure
------------------------------------------
    callpath/
      build/     # contains sample cmake build line scripts
      cmake/     # Extra build files an find modules
      doc/       # target directory for doxygen
      scripts/   # scripts to be installed to $prefix/bin
      src/       # Callpath library source code
      tests/     # Test programs for callpath library
