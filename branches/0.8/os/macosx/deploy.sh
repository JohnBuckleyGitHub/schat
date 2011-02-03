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

SCHAT_VERSION="0.8.0.1427"
TARGET="SimpleChat"

cd ../..
qmake -r
make -j 3
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

echo ""
echo -e "\x1b[32m***\x1b[0m Please create volume named \x1b[1mSimpleChat\x1b[0m and store as file \x1b[1m~/Desktop/schat.dmg\x1b[0m"
read -p "Press Enter key to continue..."

VOLUME="/Volumes/$TARGET"
cp -fr $TARGET.app $VOLUME/
ln -s /Applications $VOLUME/Applications
rm -fr $VOLUME/.fseventsd
mkdir $VOLUME/.background
cp ../../os/macosx/mac.png $VOLUME/.background/
cp ../../os/macosx/DS_Store $VOLUME/.DS_Store

echo ""
echo -e "\x1b[32m***\x1b[0m Please unmount volume \x1b[1mSimpleChat\x1b[0m"
read -p "Press Enter key to continue..."

cd ../../os/macosx/dmg
hdiutil convert ~/Desktop/schat.dmg -format UDBZ -o "schat-$SCHAT_VERSION-x86_64.dmg"
hdiutil internet-enable -yes "schat-$SCHAT_VERSION-x86_64.dmg"
rm -fr ~/Desktop/schat.dmg

