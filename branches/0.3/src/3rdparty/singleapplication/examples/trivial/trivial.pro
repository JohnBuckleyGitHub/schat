TEMPLATE	= app
LANGUAGE	= C++

TARGET		= trivial

include(../../config.pri)

DEPENDPATH	+= .
INCLUDEPATH	+= .

HEADERS		+= widget.h

SOURCES		+= main.cpp \
		widget.cpp

include(../../singleapplication.pri)
