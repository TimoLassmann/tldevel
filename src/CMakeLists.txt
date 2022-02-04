
set(HEADER_LIST
  "${tldevel_SOURCE_DIR}/include/tldevel.h"
  "${tldevel_SOURCE_DIR}/include/tlrng.h"
  "${tldevel_SOURCE_DIR}/include/tlalloc.h"
  "${tldevel_SOURCE_DIR}/include/tllogsum.h"
  "${tldevel_SOURCE_DIR}/include/tlbitvec.h"
  "${tldevel_SOURCE_DIR}/include/tlrbtree.h"
  "${tldevel_SOURCE_DIR}/include/tlcheckpoint.h"
  "${tldevel_SOURCE_DIR}/include/tlmisc.h"
  "${tldevel_SOURCE_DIR}/include/tlalphabet.h"
  "${tldevel_SOURCE_DIR}/include/kstring.h"
  "${tldevel_SOURCE_DIR}/include/khash.h"
  "${tldevel_SOURCE_DIR}/include/ksa.h" 
  "${tldevel_SOURCE_DIR}/include/esl_stopwatch.h" )

configure_file("${tldevel_SOURCE_DIR}/include/Config.h.in" "${tldevel_SOURCE_DIR}/include/Config.h")

set(tllib_SOURCES
  tldevel.c
  tlrng.c
  tlalloc.c
  tllogsum.c
  tlbitvec.c
  tlrbtree.c
  tlcheckpoint.c
  tlmisc.c
  tlalphabet.c
  kstring.c
  ksa.c
  esl_stopwatch.c
)

if(HTSlib_FOUND)
  list(APPEND HEADER_LIST
    "${tldevel_SOURCE_DIR}/include/tlseqio.h"
    "${tldevel_SOURCE_DIR}/include/tlseqbuffer.h")
  list(APPEND tllib_SOURCES  tlseqbuffer.c)
  option(USE_MYMATH "Use tutorial provided math implementation" ON)
else ()
  option(USE_MYMATH "Use tutorial provided math implementation" OFF)
  message(WARNING "htslib was not found!\nTrying to fall back on hand-crafted code depending on zlib.")
  find_package(ZLIB REQUIRED)
  
  if(ZLIB_FOUND)
    list(APPEND HEADER_LIST
      "${tldevel_SOURCE_DIR}/include/tlseqio.h"
      "${tldevel_SOURCE_DIR}/include/tlseqbuffer.h")
    list(APPEND tllib_SOURCES tlseqio.c tlseqbuffer.c)   
  else()
    message(WARNING "Neither zlib or htslib was found.\n\ntldevel will be compiled without seqio support!\n")
  endif()
  
endif()
set(tllib_LIBS)

if(HDF5_FOUND)
  list(APPEND HEADER_LIST
    "${tldevel_SOURCE_DIR}/include/tlrng_hdf5.h"
    "${tldevel_SOURCE_DIR}/include/tlhdf5wrap.h"
    "${tldevel_SOURCE_DIR}/include/tlhdf5wrap_types.h")
  
  list(APPEND tllib_SOURCES tlhdf5wrap.c tlrng_hdf5.c)
  #list(APPEND tllib_LIBS PRIVATE HDF5::HDF5)

  # target_include_directories(my_target INTERFACE ${HDF5_INCLUDE_DIRS})
  #   target_compile_definitions(my_target INTERFACE ${HDF5_DEFINITIONS})
  #   target_link_libraries(my_target INTERFACE ${HDF5_LIBRARIES})

endif()

add_library(tldevel STATIC ${tllib_SOURCES} ${HEADER_LIST})

set_target_properties(
  tldevel PROPERTIES
  SOVERSION ${tldevel_VERSION}
  OUTPUT_NAME tldevel
  C_STANDARD 11
  C_STANDARD_REQUIRED ON
  C_EXTENSIONS ON)

target_include_directories(tldevel PUBLIC ${tldevel_SOURCE_DIR}/include ${HDF5_INCLUDE_DIRS} )
target_compile_definitions(tldevel INTERFACE ${HDF5_DEFINITIONS})
target_link_libraries(tldevel PRIVATE ${HDF5_LIBRARIES})
