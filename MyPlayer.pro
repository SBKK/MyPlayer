#-------------------------------------------------
#
# Project created by QtCreator 2017-08-05T21:48:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    avmanager.cpp \
    packetqueue.cpp \
    readthread.cpp \
    videothread.cpp \
    framequeue.cpp \
    audiothread.cpp

HEADERS  += mainwindow.h \
    avmanager.h \
    packetqueue.h \
    global.h \
    readthread.h \
    videothread.h \
    framequeue.h \
    audiothread.h

FORMS    += mainwindow.ui


INCLUDEPATH += $$PWD\SDL_LIB\include
LIBS += -L$$PWD\SDL_LIB\lib -lSDL2
INCLUDEPATH += $$PWD\FFMPEG_LIB\include
LIBS += -L$$PWD\FFMPEG_LIB\lib -lavcodec -lavfilter -lavformat -lavutil -lswresample -lswscale
LIBS += -lpthread
