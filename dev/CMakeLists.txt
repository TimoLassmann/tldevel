

add_executable(esl_stopwatch_test eslStopWatchTest.c)
target_link_libraries(esl_stopwatch_test PRIVATE  tldevel)
add_test(
    NAME esl_stopwatch_test
    COMMAND esl_stopwatch_test
)

add_executable(tlmisc_test tlmisc_test.c)
target_link_libraries(tlmisc_test PRIVATE tldevel)
add_test(
    NAME tlmisc_test
    COMMAND tlmisc_test
)

add_executable(logsum_test tllogsum_test.c)
target_link_libraries(logsum_test PRIVATE tldevel)
add_test(
    NAME logsum_test
    COMMAND logsum_test
)

add_executable(khash_test khash_test.c)
target_link_libraries(khash_test PRIVATE tldevel)
add_test(
  NAME khash_test
  COMMAND khash_test
  )

add_executable(kstring_test kstring_test.c)
target_link_libraries(kstring_test PRIVATE tldevel)
add_test(
    NAME kstring_test
    COMMAND kstring_test
)

add_executable(tldevel_test tldevel_test.c)
target_link_libraries(tldevel_test PRIVATE tldevel)
add_test(
    NAME tldevel_test
    COMMAND tldevel_test
)


add_executable(tlalphabet_test tlalphabet_test.c)
target_link_libraries(tlalphabet_test PRIVATE tldevel)
add_test(
    NAME tlalphabet_test
    COMMAND tlalphabet_test
)



if(HDF5_FOUND)  
  add_executable(hdf5_test tlhdf5wrap_test.c)
  target_link_libraries(hdf5_test PRIVATE tldevel)
  target_link_libraries(hdf5_test PRIVATE ${HDF5_C_LIBRARIES})
  target_include_directories(hdf5_test PUBLIC ${HDF5_C_INCLUDE_DIRS})
  
  add_test(
    NAME hdf5_test
    COMMAND hdf5_test
    )
endif()


add_executable(tlrng_test tlrng_test.c)
target_link_libraries(tlrng_test PRIVATE tldevel)
add_test(
  NAME tlrng_test
  COMMAND tlrng_test
  )
if(HDF5_FOUND)
  target_link_libraries(tlrng_test PRIVATE ${HDF5_C_LIBRARIES})
  target_include_directories(tlrng_test PUBLIC ${HDF5_C_INCLUDE_DIRS})
endif()
