cmake_minimum_required(VERSION 3.16)

project(SPReader VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 or Qt5 components
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets PdfWidgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets PdfWidgets Concurrent)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DJVULIBRE REQUIRED ddjvuapi)

# Automatic generation of MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find UI and form wrapper tools
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(SPReader
    src/main.cpp
    src/MainWindow.cpp
    src/SettingsManager.h
    src/SettingsManager.cpp
        src/models/PdfContentsModel.h
        src/models/PdfContentsModel.cpp
        src/data/DjvuDocument.h
        src/data/DjvuDocument.cpp
        src/models/DjvuContentsModel.h
        src/models/DjvuContentsModel.cpp
        src/widgets/ContentsTreeView.cpp
        src/widgets/ContentsTreeView.h
        src/widgets/DjvuView.h
        src/widgets/DjvuView.cpp
    resources.qrc
)

qt6_add_ui(SPReader
    SOURCES ui/mainwindow.ui
    INCLUDE_PREFIX ui/
)

target_link_libraries(SPReader PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PdfWidgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    ${DJVULIBRE_LIBRARIES}
)

target_compile_definitions(SPReader PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
)

target_include_directories(SPReader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${DJVULIBRE_INCLUDE_DIRS}
)

install(TARGETS SPReader DESTINATION bin)
install(FILES resources/spreader.desktop DESTINATION share/applications)
install(FILES resources/icon.svg DESTINATION share/icons/hicolor/scalable/apps RENAME spreader.svg)
