include(ALPSEnableTests)

set (test_src
    sum_single
    )

foreach(test ${test_src})
    alps_add_gtest(${test})
endforeach(test)

#add tests for MPI
if(ALPS_HAVE_MPI)

set (test_src_mpi
    sum_mpi
    )
foreach(test ${test_src_mpi})
    alps_add_gtest(${test})
endforeach(test)

endif(ALPS_HAVE_MPI)
