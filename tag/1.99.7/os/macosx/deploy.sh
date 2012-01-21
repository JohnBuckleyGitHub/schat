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

VERSION="1.99.7"
TARGET="SimpleChat2"

cd ../..
qmake -r
make -j3
cd out
mkdir $TARGET.app/Contents/Frameworks/
cp -R libschat*dylib $TARGET.app/Contents/Frameworks/
cp schatd2.app/Contents/MacOS/schatd2 $TARGET.app/Contents/MacOS/
cp plugins/*dylib $TARGET.app/Contents/PlugIns/

macdeployqt $TARGET.app

cp -fR ../res/doc $TARGET.app/Contents/Resources/doc
mkdir $TARGET.app/Contents/Resources/translations
cp -fr ../res/translations/*.png $TARGET.app/Contents/Resources/translations/
cp -fr ../res/translations/*.qm $TARGET.app/Contents/Resources/translations/

DIR="bundle"
mkdir $DIR
cp -fr $TARGET.app $DIR
ln -s /Applications $DIR/Applications

hdiutil create -ov -srcfolder $DIR -format UDBZ -volname "$TARGET $VERSION" "$TARGET-$VERSION.dmg"
hdiutil internet-enable -yes "$TARGET-$VERSION.dmg"

cp "$TARGET-$VERSION.dmg" ../os/macosx/dmg


