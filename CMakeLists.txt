cmake_minimum_required(VERSION 3.16)

project(2Fast4U LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)
find_package(Qt6 COMPONENTS Multimedia REQUIRED)

add_compile_definitions(WITH_MINIAUDIO)
add_compile_definitions(NOMINMAX)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/soloud/include)
file(GLOB SOLOUD_SOURCES
    "soloud/src/core/*.cpp"
    "soloud/src/audiosource/wav/*.cpp"
    "soloud/src/audiosource/wav/*.c"
    "soloud/src/backend/miniaudio/*.cpp"
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

qt_add_executable(2Fast4U
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${SOLOUD_SOURCES}
    gamewidget.h gamewidget.cpp
    resources.qrc
    line.h
    player.h player.cpp
    terrain.h terrain.cpp
    obstacle.h
    serialcontroller.h serialcontroller.cpp
    powertrain.h powertrain.cpp
    virtualshifter.h virtualshifter.cpp
    audiomanager.h audiomanager.cpp
    powertrainaudiocontroller.h powertrainaudiocontroller.cpp

    racemanager.h racemanager.cpp
    spritemanager.h spritemanager.cpp
)

target_link_libraries(2Fast4U PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(2Fast4U PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)
target_link_libraries(2Fast4U PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)

include(GNUInstallDirs)
install(TARGETS 2Fast4U
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
