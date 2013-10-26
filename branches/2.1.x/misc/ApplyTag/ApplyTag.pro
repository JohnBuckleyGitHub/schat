TEMPLATE = app
TARGET = ApplyTag

CONFIG += console

QT = core 

HEADERS += \
  src/ApplyTag.h \

SOURCES += \
  ApplyTag.cpp \
  ApplyTagTool.cpp \
 
VPATH += \
    . \
    src \

INCLUDEPATH += \
    . \
    src \
