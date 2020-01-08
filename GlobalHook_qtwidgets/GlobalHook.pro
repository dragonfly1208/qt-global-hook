#-------------------------------------------------
#
# Project created by QtCreator 2016-12-03T09:55:23
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GlobalHook
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui
LIBS +=   -lUser32
