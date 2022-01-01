cmake_minimum_required(VERSION 3.10)

project(tldevel LANGUAGES C VERSION 1.3)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")

# set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
# set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

# target_compile_options(tutorial_compiler_flags INTERFACE
#   "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
#   "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
#   )

# if(APPLE)
#   add_compile_options(-dead_strip)
# elseif(UNIX)
#   add_compile_options(-Wl,--gc-sections -Wl,--as-needed)  
# endif()


find_package(HTSlib)
  
find_package(HDF5 REQUIRED COMPONENTS C HL)

configure_file(CmakeConfig.h.in CmakeConfig.h)

set(tllib_SOURCES
  tldevel.h
  tldevel.c
  tlrng.h
  tlrng.c
  tlalloc.h
  tlalloc.c
  tllogsum.h
  tllogsum.c
  tlbitvec.h
  tlbitvec.c
  tlrbtree.h
  tlrbtree.c
  tlcheckpoint.h
  tlcheckpoint.c
  tlmisc.h
  tlmisc.c
  tlalphabet.h
  tlalphabet.c
  kstring.h
  kstring.c
  khash.h
  ksa.h 
  ksa.c
  esl_stopwatch.h
  esl_stopwatch.c
)

if(HTSlib_FOUND)
  list(APPEND tllib_SOURCES tlseqio.h tlseqbuffer.h tlseqbuffer.c)
else ()
  message(WARNING "htslib was not found!\nTrying to fall back on hand-crafted code depending on zlib.")
  find_package(ZLIB REQUIRED)
  
  if(ZLIB_FOUND)
    list(APPEND tllib_SOURCES tlseqio.h tlseqbuffer.h tlseqio.c tlseqbuffer.c)   
  else()
    message(WARNING "Neither zlib or htslib was found.\n\ntldevel will be compiled without seqio support!\n")
  endif()
  
endif()
set(tllib_LIBS)

if(HDF5_FOUND)  
  list(APPEND tllib_SOURCES tlhdf5wrap.h tlhdf5wrap.c)
  list(APPEND tllib_LIBS PRIVATE HDF5::HDF5)
endif()
  

add_library(tldevel STATIC ${tllib_SOURCES})
#add_dependencies(htslib zlib)

target_link_libraries(tldevel ${tllib_LIBS})

target_include_directories(tldevel PUBLIC   "${PROJECT_BINARY_DIR}"  )
