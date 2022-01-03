cmake_minimum_required(VERSION 3.1)

project(tldevel LANGUAGES C VERSION "1.3.0")


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
  else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(GNUInstallDirs)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")    
endif()

option(BUILD_SHARED_LIBS "Build all libraries as shared" OFF)



list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(HTSlib)
find_package(HDF5 COMPONENTS C HL)

add_subdirectory(src)


#add_subdirectory(dev)

include(CTest)
add_subdirectory(dev)


# Install ################## #################################################
install(TARGETS tldevel
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

file(GLOB HEADER_FILES  ${PROJECT_SOURCE_DIR}/include/*.h)

install(FILES ${HEADER_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tldevel)


# pkg-config
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/tldevel.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/tldevel-1.pc
    @ONLY)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/tldevel-1.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT Development)
  
# Package Generator  #######################################################
set(CPACK_PACKAGE_VENDOR "Vendor name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Some summary")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.org")
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /GPATH
    /GTAGS
    /GRTAGS
    /.*build.*
    /.dir-locals.el
    /\\\\.DS_Store
)
include (CPack)


