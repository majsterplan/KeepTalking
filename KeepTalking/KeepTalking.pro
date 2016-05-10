#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T23:08:01
#
#-------------------------------------------------

QT       += core

QT       -= gui

QT       += sql

TARGET = KeepTalking
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++11

TEMPLATE = app

HEADERS += \
    server.h \
    commandparser.h \
    commandbuilder.h \
    user.h \
    usersmanager.h \
    conversation.h \
    conversationsmanager.h

SOURCES += main.cpp \
    server.cpp \
    commandparser.cpp \
    commandbuilder.cpp \
    user.cpp \
    usersmanager.cpp \
    conversation.cpp \
    conversationsmanager.cpp

LIBS += -lwsock32
