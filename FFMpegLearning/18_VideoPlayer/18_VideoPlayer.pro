QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customslider.cpp \
    main.cpp \
    mainwindow.cpp \
    qmutexcond.cpp \
    videoplayer.cpp \
    videoplayer_audio.cpp \
    videoplayer_video.cpp \
    videowidget.cpp

HEADERS += \
    customslider.h \
    mainwindow.h \
    qmutexcond.h \
    videoplayer.h \
    videowidget.h

FORMS += \
    mainwindow.ui

win32 {
        FFMPEG_HOME=C:/msys64/usr/local/ffmpeg
#FFMPEG_HOME=C:/Users/pc/Downloads/Compressed/ffmpeg-4.3.2-2021-02-27-full_build-shared

        SDL_HOME=D:/SDL2-devel-2.0.22-mingw/SDL2-2.0.22/x86_64-w64-mingw32
}

message($${FFMPEG_HOME})
#message ($${SDL_HOME})
INCLUDEPATH +=$${FFMPEG_HOME}/include


LIBS+= -L $${FFMPEG_HOME}/lib/  \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lpostproc \
        -lswscale \
        -lswresample

INCLUDEPATH +=$${SDL_HOME}/include
LIBS+= -L$${SDL_HOME}/lib \
       -lSDL2
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
