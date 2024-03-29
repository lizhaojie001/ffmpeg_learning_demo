QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


win32 {

        SDL_HOME=D:/SDL2-devel-2.0.22-mingw/SDL2-2.0.22/x86_64-w64-mingw32
}


mac {
    SDL_HOME=/usr/local/Cellar/sdl2/2.0.22
}

INCLUDEPATH +=$${SDL_HOME}/include
LIBS+= -L$${SDL_HOME}/lib \
       -lSDL2
