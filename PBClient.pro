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
CONFIG += c++11


TEMPLATE = app

LIBS = -lcurl -lboost_system -lboost_random -lboost_thread -lssl -lcrypto

SOURCES += main.cpp \
    core.cpp

HEADERS += \
    core.h
