#!win32:VERSION = 1.1

TEMPLATE     = lib
LANGUAGE     = C++

DEFINES      *= SINGLEAPPLICATION_BUILD

include(../config.pri)
include(../singleapplication.pri)

TARGET       = $$qtLibraryTarget($$SINGLEAPPLICATION_LIBNAME)

CONFIG       += $$SINGLEAPPLICATION_LIBCONFIG
QT           -= gui

INCLUDEPATH  += $$PWD
DEPENDPATH   += $$PWD

include(src.pri)

win32:CLEAN_FILES += $(DLLDESTDIR)/$(QMAKE_TARGET).dll $(DLLDESTDIR)/$(QMAKE_TARGET).lib
else:CLEAN_FILES += $(DESTDIR)/lib$(QMAKE_TARGET)*
