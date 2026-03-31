# Project Configuration
TARGET = 2Fast4U
TEMPLATE = app
CONFIG += c++17 console
CONFIG -= app_bundle

# Qt Modules
QT += core gui widgets serialport multimedia

# Compiler Definitions
DEFINES += WITH_MINIAUDIO
DEFINES += NOMINMAX

# Include Directories (Pointing to the sibling SoLoud folder)
INCLUDEPATH += $$PWD/../soloud/include

# Static Library Linking (Dynamically handles Debug vs Release builds)
CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../soloud/debug -lsoloud
    PRE_TARGETDEPS += $$OUT_PWD/../soloud/debug/soloud.lib
} else {
    LIBS += -L$$OUT_PWD/../soloud/release -lsoloud
    PRE_TARGETDEPS += $$OUT_PWD/../soloud/release/soloud.lib
}

# Project Headers
HEADERS += \
    mainwindow.h \
    gamewidget.h \
    line.h \
    player.h \
    terrain.h \
    obstacle.h \
    serialcontroller.h \
    powertrain.h \
    virtualshifter.h \
    audiomanager.h \
    powertrainaudiocontroller.h \
    racemanager.h \
    spritemanager.h

# Project Sources (Game code ONLY. SoLoud is handled by the static library.)
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gamewidget.cpp \
    player.cpp \
    terrain.cpp \
    serialcontroller.cpp \
    powertrain.cpp \
    virtualshifter.cpp \
    audiomanager.cpp \
    powertrainaudiocontroller.cpp \
    racemanager.cpp \
    spritemanager.cpp

# UI and Resources
FORMS += mainwindow.ui
RESOURCES += resources.qrc