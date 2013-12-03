#-------------------------------------------------
#
# Project created by QtCreator 2013-11-30T17:56:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = Project5
TEMPLATE = app


SOURCES +=      main.cpp\
                mainwindow.cpp \
		svoip.cpp \
		camerasampler.cpp \
                startermanager.cpp \
                soundsampler.cpp \
                settingsmanager.cpp \
                contactsmanager.cpp \
                basesampler.cpp \
                graphview.cpp \
    contact.cpp

HEADERS  +=     mainwindow.h \
                svoip.h \
                camerasampler.h \
                startermanager.h \
                soundsampler.h \
                settingsmanager.h \
                contactsmanager.h \
                basesampler.h \
                graphview.h \
    contact.h

FORMS    +=
