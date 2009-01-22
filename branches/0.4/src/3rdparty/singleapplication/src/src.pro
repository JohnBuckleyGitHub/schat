#!win32:VERSION	= 1.0

TEMPLATE	= lib
LANGUAGE	= C++

DEFINES		*= SINGLEAPPLICATION_BUILD

include(../config.pri)
include(../singleapplication.pri)

TARGET		= $$qtLibraryTarget($$SINGLEAPPLICATION_LIBNAME)

CONFIG		+= $$SINGLEAPPLICATION_LIBCONFIG
QT		-= gui

include(src.pri)

win32:CLEAN_FILES += $(DLLDESTDIR)/$(QMAKE_TARGET).lib $(DLLDESTDIR)/$(QMAKE_TARGET).dll
else:CLEAN_FILES += $(DESTDIR)/lib$(QMAKE_TARGET)*
