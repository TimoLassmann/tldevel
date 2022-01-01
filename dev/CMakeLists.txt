cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()


enable_testing()

set(pathword "${CMAKE_SOURCE_DIR}/words.txt")

add_executable(esl_stopwatch_test eslStopWatchTest.c)
target_link_libraries(esl_stopwatch_test PUBLIC tldevel)
target_include_directories(esl_stopwatch_test PUBLIC
  "${PROJECT_SOURCE_DIR}")

add_test(
    NAME esl_stopwatch_test
    COMMAND esl_stopwatch_test
)

add_executable(tlmisc_test tlmisc_test.c)
target_link_libraries(tlmisc_test PUBLIC tldevel)
target_include_directories(tlmisc_test PUBLIC
  "${PROJECT_SOURCE_DIR}")

add_test(
    NAME tlmisc_test
    COMMAND tlmisc_test
)

add_executable(logsum_test tllogsum_test.c)
target_link_libraries(logsum_test PUBLIC tldevel)
target_include_directories(logsum_test PUBLIC
  "${PROJECT_SOURCE_DIR}")

add_test(
    NAME logsum_test
    COMMAND logsum_test
)

add_executable(khash_test khash_test.c)
target_link_libraries(khash_test PUBLIC tldevel)
target_include_directories(khash_test PUBLIC
  "${PROJECT_SOURCE_DIR}")

add_test(
  NAME khash_test
  COMMAND khash_test
  )

add_executable(kstring_test kstring_test.c)
target_link_libraries(kstring_test PUBLIC tldevel)
target_include_directories(kstring_test PUBLIC
"${PROJECT_SOURCE_DIR}")
add_test(
    NAME kstring_test
    COMMAND kstring_test
)

add_executable(tldevel_test tldevel_test.c)
target_link_libraries(tldevel_test PUBLIC tldevel)
target_include_directories(tldevel_test PUBLIC
"${PROJECT_SOURCE_DIR}")
add_test(
    NAME tldevel_test
    COMMAND tldevel_test
)


add_executable(tlalphabet_test tlalphabet_test.c)
target_link_libraries(tlalphabet_test PUBLIC tldevel)
target_include_directories(tlalphabet_test PUBLIC
"${PROJECT_SOURCE_DIR}")
add_test(
    NAME tlalphabet_test
    COMMAND tlalphabet_test
)



if(HDF5_FOUND)
  
  add_executable(hdf5_test tlhdf5wrap_test.c)
  target_link_libraries(hdf5_test PUBLIC tldevel)
  target_include_directories(hdf5_test PUBLIC
    "${PROJECT_SOURCE_DIR}")
  target_link_libraries(hdf5_test PRIVATE HDF5::HDF5)

  add_test(
    NAME hdf5_test
    COMMAND hdf5_test
    )
  
  #ifdef HAVE_HDF5
  
endif()


add_executable(tlrng_test tlrng_test.c ${PROJECT_BINARY_DIR}/CmakeConfig.h)
target_link_libraries(tlrng_test PUBLIC tldevel)
target_include_directories(tlrng_test PUBLIC
  "${PROJECT_SOURCE_DIR}")
add_test(
  NAME tlrng_test
  COMMAND tlrng_test
  )
if(HDF5_FOUND)
  target_link_libraries(tlrng_test PRIVATE HDF5::HDF5)
endif()
