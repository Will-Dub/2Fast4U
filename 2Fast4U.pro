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

# Include Directories
INCLUDEPATH += $$PWD/soloud/include

# SoLoud Sources (using qmake's file globbing)
SOURCES += \
    $$PWD/soloud/src/core/soloud.cpp \
    $$PWD/soloud/src/core/soloud_audiosource.cpp \
    $$PWD/soloud/src/core/soloud_bus.cpp \
    $$PWD/soloud/src/core/soloud_core_3d.cpp \
    $$PWD/soloud/src/core/soloud_core_basicops.cpp \
    $$PWD/soloud/src/core/soloud_core_faderops.cpp \
    $$PWD/soloud/src/core/soloud_core_filterops.cpp \
    $$PWD/soloud/src/core/soloud_core_getters.cpp \
    $$PWD/soloud/src/core/soloud_core_setters.cpp \
    $$PWD/soloud/src/core/soloud_core_voicegroup.cpp \
    $$PWD/soloud/src/core/soloud_core_voiceops.cpp \
    $$PWD/soloud/src/core/soloud_fader.cpp \
    $$PWD/soloud/src/core/soloud_fft.cpp \
    $$PWD/soloud/src/core/soloud_fft_lut.cpp \
    $$PWD/soloud/src/core/soloud_file.cpp \
    $$PWD/soloud/src/core/soloud_filter.cpp \
    $$PWD/soloud/src/core/soloud_misc.cpp \
    $$PWD/soloud/src/core/soloud_queue.cpp \
    $$PWD/soloud/src/core/soloud_thread.cpp \
    $$PWD/soloud/src/audiosource/wav/soloud_wav.cpp \
    $$PWD/soloud/src/audiosource/wav/soloud_wavstream.cpp \
    $$PWD/soloud/src/audiosource/wav/dr_impl.cpp \
    $$PWD/soloud/src/audiosource/wav/stb_vorbis.c \
    $$PWD/soloud/src/backend/miniaudio/soloud_miniaudio.cpp

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

# Project Sources
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