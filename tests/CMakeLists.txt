function(add_test test_name src_name)
  add_executable(${test_name} ${src_name})
  target_link_libraries(${test_name} callpath adept_utils)
  if (CALLPATH_HAVE_MPI)
    target_link_libraries(${test_name} ${MPI_LIBRARIES})
  endif()
endfunction()

function(add_mpi_test test_name src_name)
  if (CALLPATH_HAVE_MPI)
    add_test(${test_name} ${src_name})
    target_link_libraries(${test_name} ${MPI_LIBRARIES} adept_utils)
  endif()
endfunction()

add_test(runtime-test runtime_test.C)
add_mpi_test(pack-test pack_test.C)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${adept_utils_INCLUDE_PATH}
  ${MPI_INCLUDE_PATH})
