cmake_minimum_required(VERSION 3.5)

project(Data_logger VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 COMPONENTS SerialPort REQUIRED)
find_package(Qt6 COMPONENTS Charts REQUIRED)

find_package(Qt6 COMPONENTS Core Gui REQUIRED)
add_subdirectory(${PROJECT_SOURCE_DIR}/Libs/QXlsx/QXlsx)

include_directories(Includes/)

set(PROJECT_SOURCES
        Includes/mainwindow.h
        Includes/comport.hpp
        Includes/all.hpp
		Includes/threaddisplaytimer.hpp
		Includes/threadreader.hpp
		Includes/windownext.hpp
		Includes/windowchart.hpp
		
        Srcs/comport.cpp
        Srcs/main.cpp
        Srcs/mainwindow.cpp
		Srcs/threaddisplaytimer.cpp
		Srcs/threadreader.cpp
		Srcs/windownext.cpp
		Srcs/windowchart.cpp
		
        mainwindow.ui
        ResourceFiles.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_executable(Data_logger
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
		add_library(Data_logger SHARED
            ${PROJECT_SOURCES}
        )
    else()
		add_executable(Data_logger
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Data_logger PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Data_logger PRIVATE Qt6::Core Qt6::SerialPort)
target_link_libraries(Data_logger PRIVATE Qt6::Charts)
target_link_libraries(Data_logger PRIVATE QXlsx::QXlsx)

set_target_properties(Data_logger PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Data_logger
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(Data_logger)
endif()
