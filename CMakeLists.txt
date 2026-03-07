cmake_minimum_required(VERSION 3.16)

project(2Fast4U LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

qt_add_executable(2Fast4U
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    gamewidget.h gamewidget.cpp
    resources.qrc
    line.h
    player.h player.cpp
    terrain.h terrain.cpp
    obstacle.h
    serialcontroller.h serialcontroller.cpp
    powertrain.h powertrain.cpp

)

target_link_libraries(2Fast4U PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(2Fast4U PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)

include(GNUInstallDirs)
install(TARGETS 2Fast4U
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
