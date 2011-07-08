function(add_test test_name src_name)
  add_executable(${test_name} ${src_name})
  target_link_libraries(${test_name} callpath adept-utils)
endfunction() 

function(add_mpi_test test_name src_name)
  if (CALLPATH_HAVE_MPI)
    add_test(${test_name} ${src_name})
    target_link_libraries(${test_name} ${MPI_LIBRARIES} adept-utils)
  endif()
endfunction() 

add_test(runtime-test runtime_test.cpp)
add_mpi_test(pack-test pack_test.cpp)

#???runtime_test_LDFLAGS = $(SW_LDFLAGS) $(SW_RPATH)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${adept-utils_INCLUDE_PATH}
  ${MPI_INCLUDE_PATH})
