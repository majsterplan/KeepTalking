#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T23:08:01
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = KeepTalking
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    server.h

SOURCES += main.cpp \
    server.cpp

LIBS += -lwsock32
