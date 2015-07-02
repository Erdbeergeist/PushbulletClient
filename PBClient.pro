#-------------------------------------------------
#
# Project created by QtCreator 2015-07-01T11:32:36
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = PBClient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS = -lcurl -lboost_system


SOURCES += main.cpp \
    core.cpp

HEADERS += \
    core.h
