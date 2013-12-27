
link_directories(
    ${CMAKE_BINARY_DIR}/src)
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR})

include(${QT_USE_FILE})

add_executable(rrdviewer rrdviewer.cpp)
target_link_libraries(rrdviewer QRRD)

