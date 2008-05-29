# $Id$
# Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)

TEMPLATE      = subdirs
SUBDIRS       = src/schat \
                src/schatd

win32:SUBDIRS +=  src/update
