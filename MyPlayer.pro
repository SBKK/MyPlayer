#-------------------------------------------------
#
# Project created by QtCreator 2017-08-05T21:48:48
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyPlayer
TEMPLATE = app


SOURCES += src\main.cpp\
        src\mainwindow.cpp \
    src\avmanager.cpp \
    src\packetqueue.cpp \
    src\readthread.cpp \
    src\videothread.cpp \
    src\framequeue.cpp \
    src\audiothread.cpp \
    src/myopenglwidget.cpp \
    src/avclock.cpp \
    src/aveventthread.cpp

HEADERS  += src\mainwindow.h \
    src\avmanager.h \
    src\packetqueue.h \
    src\global.h \
    src\readthread.h \
    src\videothread.h \
    src\framequeue.h \
    src\audiothread.h \
    src/myopenglwidget.h \
    src/avclock.h \
    src/aveventthread.h

FORMS    += src\mainwindow.ui


INCLUDEPATH += $$PWD\include
LIBS += -L$$PWD\lib\SDL2 -lSDL2
LIBS += -L$$PWD\lib\FFmpeg -lavcodec -lavfilter -lavformat -lavutil -lswresample -lswscale
LIBS += -lpthread

win32:LIBS += -lOpengl32 \
                -lglu32 \
                -lglut
