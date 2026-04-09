# Project Configuration
TARGET = 2Fast4U
TEMPLATE = app
CONFIG += c++17 console
CONFIG -= app_bundle

# Qt Modules
QT += core gui widgets serialport multimedia

# Compiler Definitions
DEFINES += WITH_MINIAUDIO

# On garde les hacks Windows confinés à Windows
win32: DEFINES += NOMINMAX

# Include Directories (Pointing to the sibling SoLoud folder)
INCLUDEPATH += $$PWD/../soloud/include

# Static Library Linking
CONFIG(debug, debug|release) {
    # On pointe directement vers le dossier soloud généré par le Shadow Build
    LIBS += -L$$OUT_PWD/../soloud -lsoloud

    win32: PRE_TARGETDEPS += $$OUT_PWD/../soloud/soloud.lib
    unix:!macx: PRE_TARGETDEPS +=$$OUT_PWD/../soloud/libsoloud.a
} else {
    # Même logique pour le mode release
    LIBS += -L$$OUT_PWD/../soloud -lsoloud

    win32: PRE_TARGETDEPS +=$$OUT_PWD/../soloud/soloud.lib
    unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../soloud/libsoloud.a
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
    spritemanager.h \
    menuwidget.h \
    optionswidget.h \
    startwidget.h \
    pausewidget.h \
    endwidget.h \
    hoverbutton.h

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
    spritemanager.cpp \
    menuwidget.cpp \
    optionswidget.cpp \
    startwidget.cpp \
    pausewidget.cpp \
    endwidget.cpp \
    hoverbutton.cpp

# UI and Resources
FORMS += mainwindow.ui
RESOURCES += resources.qrc
RESOURCES += sounds.qrc