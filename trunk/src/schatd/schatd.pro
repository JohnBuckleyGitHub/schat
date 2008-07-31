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
CONFIG += console warn_on
HEADERS = \
    abstractprofile.h \
    channellog.h \
    clientservice.h \
    daemon.h \
    daemonservice.h \
    daemonsettings.h \
    log.h \
    network.h \
    networkreader.h \
    profile.h \
    protocol.h \
    userunit.h \
    version.h \
    
SOURCES = \
    abstractprofile.cpp \
    channellog.cpp \
    clientservice.cpp \
    daemon.cpp \
    daemonservice.cpp \
    daemonsettings.cpp \
    log.cpp \
    main.cpp \
    network.cpp \
    networkreader.cpp \
    profile.cpp \
    userunit.cpp \
    
DEPENDPATH  += . \
               ../common \
                
INCLUDEPATH += . \
               ../common \

CONFIG(debug, debug|release) { 
    RCC_DIR = ../../tmp/schatd/debug/rcc
    MOC_DIR = ../../tmp/schatd/debug/moc
    OBJECTS_DIR = ../../tmp/schatd/debug/obj
    DESTDIR = ../../out/debug
}
CONFIG(release, debug|release) { 
    RCC_DIR = ../../tmp/schatd/release/rcc
    MOC_DIR = ../../tmp/schatd/release/moc
    OBJECTS_DIR = ../../tmp/schatd/release/obj
    DESTDIR = ../../out/release
}
win32:RC_FILE = schatd.rc

include(schatd.pri)
