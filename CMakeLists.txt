# CMakeLists.txt – QtRibbonWidget
#
# Builds the QtRibbonWidget shared library and, optionally, the demo
# application.  Generates CMake package-config files so other CMake
# projects can consume the library with:
#
#   find_package(QtRibbonWidget REQUIRED)
#   target_link_libraries(myapp PRIVATE QtRibbonWidget::QtRibbonWidget)
#
# Typical build / install sequence:
#   mkdir build && cd build
#   cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
#   cmake --build .
#   cmake --install .
#
# To also build the demo application:
#   cmake .. -DBUILD_DEMO=ON

cmake_minimum_required(VERSION 3.12)

project(QtRibbonWidget
  VERSION   1.0.0
  LANGUAGES CXX
)

# ── C++ standard ────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Qt auto-processing (MOC / UIC / RCC) ───────────────────────────────────
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# UIC looks for .ui files relative to the source; tell it where they are.
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}")

# ── Find Qt5 ────────────────────────────────────────────────────────────────
find_package(Qt5 5.9 REQUIRED COMPONENTS Core Gui Widgets)

# ── Install directories (follow GNU conventions) ────────────────────────────
include(GNUInstallDirs)

# ── Library sources & headers ────────────────────────────────────────────────
#
# Only the three ribbon widget classes form the public API.
# main.cpp and mainwindow.{h,cpp} belong to the demo application.

set(QTRIBBONWIDGET_PUBLIC_HEADERS
  ribbon.h
  ribbonbuttongroup.h
  ribbontabbar.h
  ribbontabcontent.h
  ribbontoolbutton.h
)

set(QTRIBBONWIDGET_SOURCES
  ribbon.cpp
  ribbonbuttongroup.cpp
  ribbontabbar.cpp
  ribbontabcontent.cpp
  ribbontoolbutton.cpp
  # .ui files are listed so AUTOUIC generates the required ui_*.h headers.
  ribbonbuttongroup.ui
  ribbontabcontent.ui
  resource.qrc
)

# ── Shared library ───────────────────────────────────────────────────────────
add_library(QtRibbonWidget SHARED ${QTRIBBONWIDGET_SOURCES})

add_library(QtRibbonWidget::QtRibbonWidget ALIAS QtRibbonWidget)

# Versioned .so names: libQtRibbonWidget.so.1  →  libQtRibbonWidget.so.1.0.0
set_target_properties(QtRibbonWidget PROPERTIES
  VERSION       ${PROJECT_VERSION}
  SOVERSION     ${PROJECT_VERSION_MAJOR}
  PUBLIC_HEADER "${QTRIBBONWIDGET_PUBLIC_HEADERS}"
  EXPORT_NAME   QtRibbonWidget
)

target_link_libraries(QtRibbonWidget
  PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# Consumers of the installed library only need the install-time include dir.
# During the build itself, the binary dir is also needed for generated ui_*.h.
target_include_directories(QtRibbonWidget
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/QtRibbonWidget>
)

# Suppress deprecation warnings (mirrors QT_DEPRECATED_WARNINGS from the .pro)
target_compile_definitions(QtRibbonWidget PRIVATE QT_DEPRECATED_WARNINGS)

# ── Install: library + public headers ────────────────────────────────────────
install(TARGETS QtRibbonWidget
  EXPORT        QtRibbonWidgetTargets
  LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QtRibbonWidget
)

# ── Install: icons (optional resource – useful for applications that want
#             to reuse the bundled SVG icons without embedding them) ──────────
install(DIRECTORY icons/
  DESTINATION ${CMAKE_INSTALL_DATADIR}/QtRibbonWidget/icons
)

# ── Install: themes (source QSS files) ───────────────────────────────────────
install(DIRECTORY themes/
  DESTINATION ${CMAKE_INSTALL_DATADIR}/QtRibbonWidget/themes
)

# ── CMake package-config files ───────────────────────────────────────────────
#
# Generates:
#   <prefix>/lib/cmake/QtRibbonWidget/QtRibbonWidgetConfig.cmake
#   <prefix>/lib/cmake/QtRibbonWidget/QtRibbonWidgetConfigVersion.cmake
#   <prefix>/lib/cmake/QtRibbonWidget/QtRibbonWidgetTargets.cmake

include(CMakePackageConfigHelpers)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtRibbonWidgetConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/QtRibbonWidgetConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QtRibbonWidget"
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/QtRibbonWidgetConfigVersion.cmake"
  VERSION     ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(EXPORT QtRibbonWidgetTargets
  FILE      QtRibbonWidgetTargets.cmake
  NAMESPACE QtRibbonWidget::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QtRibbonWidget"
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/QtRibbonWidgetConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/QtRibbonWidgetConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QtRibbonWidget"
)

# ── Demo application (opt-in) ────────────────────────────────────────────────
option(BUILD_DEMO "Build the QtRibbonWidget demo application" ON)

if(BUILD_DEMO)
  add_executable(QtRibbonDemo
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )

  target_link_libraries(QtRibbonDemo PRIVATE QtRibbonWidget::QtRibbonWidget)

  install(TARGETS QtRibbonDemo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endif()

# ── Python bindings (opt-in) ────────────────────────────────────────────────
option(BUILD_PYTHON "Build Python bindings using SIP and PyQt5" OFF)

if(BUILD_PYTHON)
  find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
  
  find_program(SIP_EXECUTABLE NAMES sip)
  if(NOT SIP_EXECUTABLE)
    message(FATAL_ERROR "sip executable not found. Required for BUILD_PYTHON.")
  endif()

  # Get PyQt5 SIP directory
  set(PYQT5_SIP_DIR "/usr/lib/python3/dist-packages/PyQt5/bindings")
  if(NOT EXISTS "${PYQT5_SIP_DIR}/QtCore/QtCoremod.sip")
    # Fallback for older systems
    set(PYQT5_SIP_DIR "/usr/share/sip/PyQt5")
  endif()

  # Get SIP include directory
  set(SIP_INCLUDE_DIR "/usr/include/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")

  set(SIP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/python/QtRibbonWidget.sip")
  set(SIP_OUT "${CMAKE_CURRENT_BINARY_DIR}/sip")
  file(MAKE_DIRECTORY ${SIP_OUT})

  # Manual SIP 4 call with correct flags
  add_custom_command(
    OUTPUT "${SIP_OUT}/sipQtRibbonWidgetcmodule.cpp"
           "${SIP_OUT}/sipQtRibbonWidgetRibbon.cpp"
           "${SIP_OUT}/sipQtRibbonWidgetRibbonButtonGroup.cpp"
    COMMAND ${SIP_EXECUTABLE}
            -c ${SIP_OUT}
            -I ${PYQT5_SIP_DIR}
            -n PyQt5.sip
            -t Qt_5_15_0
            -t WS_X11
            ${SIP_SRC}
    DEPENDS ${SIP_SRC} python/ribbon.sip python/ribbonbuttongroup.sip
    COMMENT "Generating SIP bindings for QtRibbonWidget"
  )

  set(SIP_SOURCES
    "${SIP_OUT}/sipQtRibbonWidgetcmodule.cpp"
    "${SIP_OUT}/sipQtRibbonWidgetRibbon.cpp"
    "${SIP_OUT}/sipQtRibbonWidgetRibbonButtonGroup.cpp"
  )
  set_source_files_properties(${SIP_SOURCES} PROPERTIES GENERATED TRUE)

  add_library(PyQt5_QtRibbonWidget MODULE ${SIP_SOURCES})
  # Disable AUTOMOC for SIP generated files
  set_target_properties(PyQt5_QtRibbonWidget PROPERTIES AUTOMOC OFF)
  
  target_link_libraries(PyQt5_QtRibbonWidget PRIVATE QtRibbonWidget Python3::Python)
  target_include_directories(PyQt5_QtRibbonWidget PRIVATE ${SIP_OUT} ${SIP_INCLUDE_DIR} ${Python3_INCLUDE_DIRS})
  
  set_target_properties(PyQt5_QtRibbonWidget PROPERTIES
    PREFIX ""
    OUTPUT_NAME "QtRibbonWidget"
  )

  # Install the python module
  execute_process(
    COMMAND ${Python3_INTERPRETER} -c "import sysconfig; print(sysconfig.get_path('platlib'))"
    OUTPUT_VARIABLE PYTHON_INSTALL_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  
  install(TARGETS PyQt5_QtRibbonWidget
    DESTINATION lib
  )
  
  install(FILES python/QtRibbonWidgetPy.py
    DESTINATION ${CMAKE_INSTALL_DATADIR}/QtRibbonWidget/python
    RENAME "demo.py"
  )
endif()

# ── Summary ──────────────────────────────────────────────────────────────────
message(STATUS "")
message(STATUS "QtRibbonWidget ${PROJECT_VERSION}")
message(STATUS "  Install prefix : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Library dir    : ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "  Include dir    : ${CMAKE_INSTALL_INCLUDEDIR}/QtRibbonWidget")
message(STATUS "  Data dir       : ${CMAKE_INSTALL_DATADIR}/QtRibbonWidget")
message(STATUS "  Build demo     : ${BUILD_DEMO}")
message(STATUS "")
