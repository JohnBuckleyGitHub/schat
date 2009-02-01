TEMPLATE     = app
LANGUAGE     = C++

TARGET       = console

include(../../config.pri)
QT           -= gui
CONFIG       *= console

DEPENDPATH   += .
INCLUDEPATH  += .

HEADERS      += worker.h

SOURCES      += main.cpp \
             worker.cpp

include(../../singleapplication.pri)
