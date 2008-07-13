# $Id$
# Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)

TEMPLATE      = subdirs
SUBDIRS       = \
    src/schatd \
    src/3rdparty/singleapplication \
    src/schat

win32:SUBDIRS +=  src/update
