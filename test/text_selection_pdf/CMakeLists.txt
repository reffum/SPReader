cmake_minimum_required(VERSION 3.16)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Gui Widgets Pdf PdfWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Pdf PdfWidgets)

add_executable(text_selection_pdf
	main.cpp
	${CMAKE_SOURCE_DIR}/src/widgets/PdfView.h
	${CMAKE_SOURCE_DIR}/src/widgets/PdfView.cpp
)

target_include_directories(text_selection_pdf PRIVATE
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(text_selection_pdf PRIVATE
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Pdf
	Qt${QT_VERSION_MAJOR}::PdfWidgets
)
