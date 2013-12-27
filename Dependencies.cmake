
include(FindPkgConfig)

find_package(Qt5Core QUIET)

if (Qt5Core_FOUND)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Widgets REQUIRED)

    if (DESIGNER)
        find_package(Qt5Designer REQUIRED)
    endif (DESIGNER)

    set(QT_LIBRARIES
        ${Qt5Core_LIBRARIES}
        ${Qt5Gui_LIBRARIES}
        ${Qt5Widgets_LIBRARIES}
        ${Qt5Designer})
    set(QT_INCLUDES
        ${Qt5Core_LIBRARIES}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Designer_INCLUDE_DIRS})

    set(QT_USE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ECMQt4To5Porting.cmake")
    include (ECMQt4To5Porting.cmake)

    macro(qt_use_modules)
        qt5_use_modules(${ARGN})
    endmacro()

else (Qt5Core_FOUND)
    set(QT_REQ QtCore QtGui)

    if (DESIGNER)
        set(QT_REQ ${QT_REQ} QtDesigner)
    endif (DESIGNER)

    find_package(Qt4 4.7 REQUIRED ${QT_REQ})

    macro(qt_use_modules)
        qt4_use_modules(${ARGN})
    endmacro()
endif (Qt5Core_FOUND)

include(CheckFunctionExists)
check_function_exists(getopt_long HAVE_GETOPT_LONG)
set(QT_BINARY_DIR "${_qt5Core_install_prefix}/bin")

