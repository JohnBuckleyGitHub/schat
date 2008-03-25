TEMPLATE = app
TARGET = 
DEPENDPATH  += . \
               ..\common \
               ..\schatd
               
INCLUDEPATH += . \
               ..\common \
               ..\schatd
                

QT += network
#CONFIG += console

# Input
HEADERS += clientsocket.h \
           schatwindow.h \
           protocol.h \
           server.h \
           serversocket.h
FORMS += schatwindow.ui
SOURCES += clientsocket.cpp \
           main.cpp \
           schatwindow.cpp \
           server.cpp \
           serversocket.cpp

CONFIG(debug, debug|release) { 
    RCC_DIR = ../../tmp/schat/debug/rcc
    UI_DIR = ../../tmp/schat/debug/ui
    MOC_DIR = ../../tmp/schat/debug/moc
    OBJECTS_DIR = ../../tmp/schat/debug/obj
    DESTDIR = ../../out/debug
}
CONFIG(release, debug|release) { 
    RCC_DIR = ../../tmp/schat/release/rcc
    UI_DIR = ../../tmp/schat/release/ui
    MOC_DIR = ../../tmp/schat/release/moc
    OBJECTS_DIR = ../../tmp/schat/release/obj
    DESTDIR = ../../out/release
}