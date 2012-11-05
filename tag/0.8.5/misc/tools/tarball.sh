#!/bin/bash
# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2012 IMPOMEZIA <schat@impomezia.com>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see <http://www.gnu.org/licenses/>.

FNAME="schat-0.8.4.2277"
svn co http://schat.googlecode.com/svn/branches/0.8/ schat
rm -fr $FNAME
cp -fR schat $FNAME
find $FNAME -name .svn -exec rm -rf {} \; > /dev/null 2>&1
tar -cjf $FNAME.tar.bz2 $FNAME 

