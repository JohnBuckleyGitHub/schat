# $Id$
# Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.com)

TEMPLATE      = subdirs
SUBDIRS       = \
    src/schatd \
    src/3rdparty/singleapplication \
    src/schat \
    src/schatd-ui

win32:SUBDIRS +=  src/update
