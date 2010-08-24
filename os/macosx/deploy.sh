#!/bin/bash
# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

SCHAT_VERSION="0.7.3.1322"
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
ln -s /Applications Applications

cp -fr ../../data/doc $TARGET.app/Contents/MacOS/doc
cp -fr ../../data/emoticons $TARGET.app/Contents/MacOS/emoticons
cp -fr ../../data/networks $TARGET.app/Contents/MacOS/networks
cp -fr ../../data/sounds $TARGET.app/Contents/MacOS/sounds 
find $TARGET.app/Contents/MacOS -name .svn -exec rm -rf {} \; > /dev/null 2>&1

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

