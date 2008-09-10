# $Id$
# IMPOMEZIA Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.com)
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

TEMPLATE = app
TARGET = 
DEPENDPATH  += . \
               ../common \
                            
INCLUDEPATH += . \
               ../common \
               
QT += network

RESOURCES += ../../data/schatd-ui.qrc

HEADERS += \

SOURCES += \
    main.cpp \

CONFIG(debug, debug|release) { 
    RCC_DIR = ../../tmp/schatd-ui/debug/rcc
    MOC_DIR = ../../tmp/schatd-ui/debug/moc
    OBJECTS_DIR = ../../tmp/schatd-ui/debug/obj
    DESTDIR = ../../out/debug
}

CONFIG(release, debug|release) { 
    RCC_DIR = ../../tmp/schatd-ui/release/rcc
    MOC_DIR = ../../tmp/schatd-ui/release/moc
    OBJECTS_DIR = ../../tmp/schatd-ui/release/obj
    DESTDIR = ../../out/release
}

win32 {
    RC_FILE = schatd-ui.rc
}

INCLUDEPATH += ../3rdparty/singleapplication/include
SINGLEAPPLICATION_LIBCONFIG = staticlib
win32-msvc*:LIBS += ../3rdparty/singleapplication/bin/singleapplication.lib
else:LIBS += -L ../3rdparty/singleapplication/bin -lsingleapplication

include(schatd-ui.pri)
