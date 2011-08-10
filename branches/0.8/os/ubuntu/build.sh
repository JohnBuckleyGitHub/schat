#!/bin/bash
# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

function build() {
  local target=$1
  mkdir src/$target/debian
  cp -fr os/ubuntu/$target/* src/$target/debian
  cd src/$target
  cat debian/changelog.in | sed "s/##REVISION##/`svnversion -n`/g" | sed "s/##RDATE##/`date -R`/g" | sed "s/##DIST##/`lsb_release -cs`/g" > debian/changelog
  dpkg-buildpackage
  cd ../..
}

cd ../..
patch -p0 -N -i os/ubuntu/ubuntu.patch
qmake -r

build "schatd"
build "schat"

cp -f src/*.deb os/ubuntu/deb
