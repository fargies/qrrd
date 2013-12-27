
set(QRRD_SRCS
    RRA.cpp
    DS.cpp
    RRDFile.cpp
    RRDPlotter.cpp
    RRDWidget.cpp
    )

set(QRRD_HEADERS

    )

set(QRRD_PUBHDRS
    RRA.hpp
    DS.hpp
    RRDFile.hpp
    RRDPlotter.hpp
    RRDWidget.hpp
    )

include(${QT_USE_FILE})

add_library(QRRD
    ${QRRD_SRCS} ${QRRD_HDRS} ${QRRD_PUBHDRS})
qt_use_modules(QRRD Core Gui)
target_link_libraries(QRRD rrd)
set_target_properties(QRRD PROPERTIES
    PUBLIC_HEADER "${QRRD_PUBHDRS}")
install(TARGETS QRRD
    ARCHIVE DESTINATION lib COMPONENT Devel
    LIBRARY DESTINATION lib COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include/${PROJECT_NAME} COMPONENT Devel)
