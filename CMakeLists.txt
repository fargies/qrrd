cmake_minimum_required(VERSION 2.8)

project(libQRRD CXX C)

include(GitVersion.cmake)

set(VER_MAJOR 1)
set(VER_MINOR 0)
set(VER_PATCH "${GIT_COUNT}")
set(VERSION "${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}")
message(STATUS "Configuring version: ${VERSION}")
file(WRITE ${CMAKE_BINARY_DIR}/.version ${VERSION})

set(CMAKE_AUTOMOC True)

option(TESTS "Compile and run unit tests" OFF)
option(DESIGNER "Generate QtDesigner plugins" OFF)

include(Dependencies.cmake)

if (TESTS)
    include(FindPkgConfig)
    pkg_check_modules(CPPUNIT cppunit)
    if (NOT CPPUNIT_FOUND)
        find_library(CPPUNIT_LIBRARIES
             NAMES cppunit
             PATHS ${CPPUNIT_LIB})
        set(CPPUNIT_INCLUDE_DIRS ${CPPUNIT_INCLUDE_DIRS})
        if (NOT CPPUNIT_LIBRARIES)
            message(SEND_ERROR "CppUnit library not found")
        endif (NOT CPPUNIT_LIBRARIES)
    endif(NOT CPPUNIT_FOUND)
    enable_testing()
endif (TESTS)

configure_file("config.h.in" "config.h")
configure_file("version.h.in" "version.h")

configure_file("libQRRD.pc.in" "libQRRD.pc" @ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wno-long-long")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Woverloaded-virtual -Wunused-parameter -Wuninitialized")

install(FILES
    ${CMAKE_BINARY_DIR}/libQRRD.pc
    DESTINATION lib/pkgconfig COMPONENT Devel)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(doc)
add_subdirectory(designer)

