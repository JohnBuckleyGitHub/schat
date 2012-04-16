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
  mkdir debian
  cp -fr ../os/ubuntu/$target/* debian
  cp -f $target.pro src.pro
  cat debian/changelog.in | sed "s/##RDATE##/`date -R`/g" | sed "s/##DIST##/`lsb_release -cs`/g" > debian/changelog
  dpkg-buildpackage
  rm -Rf debian
}

cd ../../src

build "schatd2"
build "schat2"

cp -f ../*.deb ../os/ubuntu/deb
