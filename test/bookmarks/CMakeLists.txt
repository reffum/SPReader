cmake_minimum_required(VERSION 3.16)

project(BookmarksTest LANGUAGES CXX)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Gui Widgets PdfWidgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets PdfWidgets Concurrent)

set(CMAKE_AUTOMOC ON)

add_executable(bookmarks_app
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/models/BookmarksModel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/widgets/BookmarksTreeView.cpp
    main.cpp
)

target_link_libraries(bookmarks_app PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(bookmarks_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)
