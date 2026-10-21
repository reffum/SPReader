cmake_minimum_required(VERSION 3.16)

project(SPReader VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 or Qt5 components
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Gui Widgets PdfWidgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets PdfWidgets Concurrent)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DJVULIBRE REQUIRED ddjvuapi)

set(CMAKE_AUTOUIC OFF)

# Automatic generation of MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find UI and form wrapper tools
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Find the uic executable
get_target_property(_uic_executable Qt6::uic IMPORTED_LOCATION)

# Function or macro to process a UI file
function(generate_ui_header ui_file header_file)
	add_custom_command(
			OUTPUT ${header_file}
			COMMAND ${_uic_executable} -o ${header_file} ${ui_file}
			DEPENDS ${ui_file}
			COMMENT "Generating ${header_file} from ${ui_file}"
			VERBATIM
	)
endfunction()

generate_ui_header("${CMAKE_CURRENT_SOURCE_DIR}/src/ui/mainwindow.ui" "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/ui_mainwindow.h")

add_executable(SPReader
	src/ui/mainwindow.ui
	src/ui/ui_mainwindow.h

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
	src/widgets/DraggableScrollArea.h
	src/widgets/DraggableScrollArea.cpp
	resources.qrc
	src/widgets/PdfView.cpp
	src/widgets/PdfView.h
	src/widgets/DocumentWidget.h
	src/widgets/DocumentWidget.cpp
	src/widgets/PdfWidget.h
	src/widgets/PdfWidget.cpp
	src/constants.h
		src/widgets/DjvuWidget.cpp
		src/widgets/DjvuWidget.h
)


add_subdirectory(test)

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
install(FILES resources/app.svg DESTINATION share/icons/hicolor/scalable/apps RENAME spreader.svg)
