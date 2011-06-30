function(add_test runtime-test)
  add_test(${test_name} ${src_name})
  target_link_libraries(${test_name} callpath)
endfunction() 

function(add_mpi_test pack-test)
  if (CALLPATH_HAVE_MPI)
    add_test(${test_name} ${src_name})
    target_link_libraries(${test_name} ${MPI_LIBRARIES})
  endif()
endfunction() 

add_test(runtime-test runtime_test.C)
add_mpi_test(pack-test pack_test.C)

#???runtime_test_LDFLAGS = $(SW_LDFLAGS) $(SW_RPATH)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${MPI_INCLUDE_PATH})
