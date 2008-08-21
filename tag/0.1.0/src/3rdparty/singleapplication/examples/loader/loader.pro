TEMPLATE	= app
LANGUAGE	= C++

TARGET		= loader

include(../../config.pri)

DEPENDPATH	+= .
INCLUDEPATH	+= .

HEADERS		+= mainwindow.h

SOURCES		+= main.cpp \
		mainwindow.cpp

win32:LIBS	+= -luser32

include(../../singleapplication.pri)
