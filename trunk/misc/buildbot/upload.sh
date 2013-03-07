#!/bin/bash
set -e

SCHAT_VERSION=$1
SCHAT_REVISION=$2

UPLOAD_BASE="/var/www/download.schat.me/htdocs/schat2/$SCHAT_VERSION"
SNAPSHOTS="/var/www/download.schat.me/htdocs/schat2/snapshots/$SCHAT_VERSION/r$SCHAT_REVISION"

if [ ! -d $UPLOAD_BASE ]
then
  mkdir $UPLOAD_BASE
fi

cp -f $SNAPSHOTS/schat2-src-$SCHAT_VERSION-dev.tar.bz2          $UPLOAD_BASE/schat2-src-$SCHAT_VERSION.tar.bz2
cp -f $SNAPSHOTS/schat2_$SCHAT_VERSION-1~lucid-dev_amd64.deb    $UPLOAD_BASE/schat2_$SCHAT_VERSION-1~lucid_amd64.deb
cp -f $SNAPSHOTS/schat2_$SCHAT_VERSION-1~lucid-dev_i386.deb     $UPLOAD_BASE/schat2_$SCHAT_VERSION-1~lucid_i386.deb
cp -f $SNAPSHOTS/schatd2_$SCHAT_VERSION-1~lucid-dev_amd64.deb   $UPLOAD_BASE/schatd2_$SCHAT_VERSION-1~lucid_amd64.deb
cp -f $SNAPSHOTS/schatd2_$SCHAT_VERSION-1~lucid-dev_i386.deb    $UPLOAD_BASE/schatd2_$SCHAT_VERSION-1~lucid_i386.deb
cp -f $SNAPSHOTS/schat2_$SCHAT_VERSION-1~precise-dev_amd64.deb  $UPLOAD_BASE/schat2_$SCHAT_VERSION-1~precise_amd64.deb
cp -f $SNAPSHOTS/schat2_$SCHAT_VERSION-1~precise-dev_i386.deb   $UPLOAD_BASE/schat2_$SCHAT_VERSION-1~precise_i386.deb
cp -f $SNAPSHOTS/schatd2_$SCHAT_VERSION-1~precise-dev_amd64.deb $UPLOAD_BASE/schatd2_$SCHAT_VERSION-1~precise_amd64.deb
cp -f $SNAPSHOTS/schatd2_$SCHAT_VERSION-1~precise-dev_i386.deb  $UPLOAD_BASE/schatd2_$SCHAT_VERSION-1~precise_i386.deb
cp -f $SNAPSHOTS/SimpleChat2-$SCHAT_VERSION-dev.dmg             $UPLOAD_BASE/SimpleChat2-$SCHAT_VERSION.dmg
cp -f $SNAPSHOTS/schat2-$SCHAT_VERSION-dev.exe                  $UPLOAD_BASE/schat2-$SCHAT_VERSION.exe
cp -f $SNAPSHOTS/schat2-$SCHAT_VERSION-dev.msi                  $UPLOAD_BASE/schat2-$SCHAT_VERSION.msi
cp -f $SNAPSHOTS/schat2-server-$SCHAT_VERSION-dev.exe           $UPLOAD_BASE/schat2-server-$SCHAT_VERSION.exe

s3cmd put --recursive --acl-public --guess-mime-type $UPLOAD_BASE s3://impomezia/schat/

googlecode_upload.py --summary "Source Code"                --project schat --labels Type-Source,OpSys-All,2.0                 $UPLOAD_BASE/schat2-src-$SCHAT_VERSION.tar.bz2
googlecode_upload.py --summary "Mac OS X Disk Image"        --project schat --labels Type-Package,OpSys-OSX,2.0,64bit          $UPLOAD_BASE/SimpleChat2-$SCHAT_VERSION.dmg
googlecode_upload.py --summary "Chat Server"                --project schat --labels Type-Installer,OpSys-Windows,2.0          $UPLOAD_BASE/schat2-server-$SCHAT_VERSION.exe
googlecode_upload.py --summary "Chat Client. MSI Installer" --project schat --labels Type-Installer,OpSys-Windows,2.0          $UPLOAD_BASE/schat2-$SCHAT_VERSION.msi
googlecode_upload.py --summary "Chat Client"                --project schat --labels Type-Installer,OpSys-Windows,2.0,Featured $UPLOAD_BASE/schat2-$SCHAT_VERSION.exe
