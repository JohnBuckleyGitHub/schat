# $Id$
# IMPOMEZIA Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
QT += network
CONFIG += console
HEADERS = \
    version.h \
    daemon.h \
    daemonservice.h \
    protocol.h \
    profile.h    
    
SOURCES = \
    main.cpp \
    daemon.cpp \
    daemonservice.cpp \
    profile.cpp   
    
DEPENDPATH  += . \
               ../common
                
INCLUDEPATH += . \
               ../common

CONFIG(debug, debug|release) { 
    RCC_DIR = ../../tmp/daemon/debug/rcc
    MOC_DIR = ../../tmp/daemon/debug/moc
    OBJECTS_DIR = ../../tmp/daemon/debug/obj
    DESTDIR = ../../out/debug
}
CONFIG(release, debug|release) { 
    RCC_DIR = ../../tmp/daemon/release/rcc
    MOC_DIR = ../../tmp/daemon/release/moc
    OBJECTS_DIR = ../../tmp/daemon/release/obj
    DESTDIR = ../../out/release
}
win32:RC_FILE = daemon.rc
#DEFINES += SCHAT_DEBUG
#DEFINES += SCHAT_PRIVATE_LOG
