#!/bin/bash
# $Id: ppa.sh 3255 2012-11-03 17:36:32Z IMPOMEZIA $
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

set -e

PPA_LEVEL=0

PPA_VERSION=$SCHAT_VERSION-1~ppa$PPA_LEVEL
RDATE=`date -R`
SCHAT_SOURCE=schat_$SCHAT_VERSION
PPA_SOURCE=schat_$PPA_VERSION

function upload() {
  local DIST=$1
  cat ../../$WORKDIR/os/ubuntu/debian/changelog.ppa | sed "s/##RDATE##/$RDATE/g" | sed "s/##DIST##/$DIST/g" | sed "s/##SCHAT_VERSION##/$PPA_VERSION/g" > debian/changelog
  debuild -S -sa
  dput ppa:impomezia/stable ../$PPA_SOURCE~${DIST}_source.changes
}

cd ../../
WORKDIR=${PWD##*/}

cd ..
rm -fr ppa
mkdir ppa
cp -fR $WORKDIR ppa/$SCHAT_SOURCE
cp -fR $WORKDIR ppa/$PPA_SOURCE
find ppa/$SCHAT_SOURCE -name .svn -exec rm -rf {} \; > /dev/null 2>&1 || true

cd ppa
tar -cJf $SCHAT_SOURCE.orig.tar.xz $SCHAT_SOURCE 
rm -fr $SCHAT_SOURCE
cp -fr ../$WORKDIR/os/ubuntu/debian $PPA_SOURCE

cd $PPA_SOURCE
mkdir debian/source
mkdir debian/patches

echo > debian/source/format "3.0 (quilt)"
echo > debian/patches/series "ubuntu.patch"
cp ../../$WORKDIR/os/ubuntu/ubuntu.patch debian/patches

rm debian/changelog.in
rm debian/changelog.ppa

upload "raring"
upload "quantal"
upload "precise"
upload "oneiric"
upload "natty"
upload "maverick"
upload "lucid"

