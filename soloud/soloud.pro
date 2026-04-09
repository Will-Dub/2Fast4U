TARGET = soloud
TEMPLATE = lib
CONFIG += staticlib c++17
CONFIG -= qt

DEFINES += WITH_MINIAUDIO NOMINMAX

INCLUDEPATH += $$PWD/include

win32-msvc* {
    QMAKE_CFLAGS += /TP
} else {
    QMAKE_CFLAGS += -x c++
}

# List your files explicitly. NO GLOBBING.
SOURCES += \
    $$PWD/src/core/soloud.cpp \
    $$PWD/src/core/soloud_audiosource.cpp \
    $$PWD/src/core/soloud_bus.cpp \
    $$PWD/src/core/soloud_core_3d.cpp \
    $$PWD/src/core/soloud_core_basicops.cpp \
    $$PWD/src/core/soloud_core_faderops.cpp \
    $$PWD/src/core/soloud_core_filterops.cpp \
    $$PWD/src/core/soloud_core_getters.cpp \
    $$PWD/src/core/soloud_core_setters.cpp \
    $$PWD/src/core/soloud_core_voicegroup.cpp \
    $$PWD/src/core/soloud_core_voiceops.cpp \
    $$PWD/src/core/soloud_fader.cpp \
    $$PWD/src/core/soloud_fft.cpp \
    $$PWD/src/core/soloud_fft_lut.cpp \
    $$PWD/src/core/soloud_file.cpp \
    $$PWD/src/core/soloud_filter.cpp \
    $$PWD/src/core/soloud_misc.cpp \
    $$PWD/src/core/soloud_queue.cpp \
    $$PWD/src/core/soloud_thread.cpp \
    $$PWD/src/audiosource/wav/soloud_wav.cpp \
    $$PWD/src/audiosource/wav/soloud_wavstream.cpp \
    $$PWD/src/audiosource/wav/dr_impl.cpp \
    $$PWD/src/audiosource/wav/stb_vorbis.cpp \
    $$PWD/src/backend/miniaudio/soloud_miniaudio.cpp