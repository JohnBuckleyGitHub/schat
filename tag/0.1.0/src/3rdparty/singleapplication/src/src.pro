#!win32:VERSION	= 0.6

TEMPLATE	= lib
LANGUAGE	= C++

DEFINES		*= SINGLEAPPLICATION_BUILD

include(../config.pri)
include(../singleapplication.pri)

TARGET		= $$SINGLEAPPLICATION_LIBNAME

CONFIG		+= $$SINGLEAPPLICATION_LIBCONFIG
QT		-= gui

DEPENDPATH	+= .
INCLUDEPATH	+= .

HEADERS		+= singleapplication.h \
		singleapplication_p.h \
		systemsemaphore.h \
		systemsemaphore_p.h

SOURCES		+= singleapplication.cpp \
		systemsemaphore.cpp

win32:SOURCES	+= systemsemaphore_win.cpp
unix:SOURCES	+= systemsemaphore_unix.cpp

win32:CLEAN_FILES += $(DLLDESTDIR)/$(QMAKE_TARGET).lib $(DLLDESTDIR)/$(QMAKE_TARGET).dll
else:CLEAN_FILES += $(DESTDIR)/lib$(QMAKE_TARGET)*
