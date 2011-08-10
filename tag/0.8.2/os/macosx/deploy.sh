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

VERSION="0.8.2.1620"
TARGET="SimpleChat"

cd ../..
qmake -r
make -j3
cd out/release
macdeployqt $TARGET.app
macdeployqt schatd-ui.app
macdeployqt schatd.app
cp schatd-ui.app/Contents/MacOS/schatd-ui $TARGET.app/Contents/MacOS/
cp schatd.app/Contents/MacOS/schatd $TARGET.app/Contents/MacOS/

cp -fR ../../data/doc $TARGET.app/Contents/Resources/doc
cp -fR ../../data/emoticons $TARGET.app/Contents/Resources/emoticons
cp -fR ../../data/networks $TARGET.app/Contents/Resources/networks
cp -fR ../../data/sounds $TARGET.app/Contents/Resources/sounds
mkdir $TARGET.app/Contents/Resources/translations
cp -fr ../../data/translations/*.png $TARGET.app/Contents/Resources/translations/
cp -fr ../../data/translations/*.qm $TARGET.app/Contents/Resources/translations/
find $TARGET.app/Contents/Resources -name .svn -exec rm -rf {} \; > /dev/null 2>&1

DIR="bundle"
mkdir $DIR
cp -fr $TARGET.app $DIR
ln -s /Applications $DIR/Applications
rm -fr $DIR/.fseventsd
mkdir $DIR/.background
cp ../../os/macosx/mac.png $DIR/.background/
cp ../../os/macosx/DS_Store $DIR/.DS_Store

hdiutil create -ov -srcfolder $DIR -format UDBZ -volname "$TARGET $VERSION" "$TARGET-$VERSION.dmg"
hdiutil internet-enable -yes "$TARGET-$VERSION.dmg"

cp "$TARGET-$VERSION.dmg" ../../os/macosx/dmg


