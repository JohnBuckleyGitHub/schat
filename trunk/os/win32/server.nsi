/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

!define SCHAT_NAME "Simple Chat Server"
!define SCHAT_NAME_SHORT "${SCHAT_NAME}"

!include "MUI2.nsh"
!include "engine\default.nsh"
!include "engine\revision.nsh"

BrandingText "${SCHAT_COPYRIGHT}"
Caption "${SCHAT_NAME} ${SCHAT_VERSION}"
InstallDir "${SCHAT_INSTALLDIR}"
InstallDirRegKey HKCU "${SCHAT_REGKEY}" ""
Name "${SCHAT_NAME}"
OutFile "${SCHAT_OUTDIR}schat2-server-${SCHAT_PREFIX}${SCHAT_VERSION}${SCHAT_SUFFIX}.exe"
RequestExecutionLevel user

SetCompressor /SOLID lzma
SetCompressorDictSize 10

VIProductVersion "${SCHAT_VERSION}.${SCHAT_REVISION}"
VIAddVersionKey  "CompanyName"      "IMPOMEZIA"
VIAddVersionKey  "FileDescription"  "${SCHAT_NAME} Installer"
VIAddVersionKey  "FileVersion"      "${SCHAT_VERSION}.${SCHAT_REVISION}"
VIAddVersionKey  "LegalCopyright"   "${SCHAT_COPYRIGHT}"
VIAddVersionKey  "OriginalFilename" "${SCHAT_OUT_FILENAME}"
VIAddVersionKey  "ProductName"      "${SCHAT_NAME}"
VIAddVersionKey  "ProductVersion"   "${SCHAT_VERSION}"

!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_FINISHPAGE_LINK            "${SCHAT_WEB_SITE}"
!define MUI_FINISHPAGE_LINK_LOCATION   "${SCHAT_WEB_SITE}"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP         "contrib\header.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                       "contrib\install.ico"
!define MUI_UNICON                     "contrib\install.ico"

!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section

  SetOutPath "$INSTDIR"
  File "${SCHAT_SOURCE}\schatd2.exe"
  File "${SCHAT_SOURCE}\schatd.dll"
  File "license.txt"

  File "${SCHAT_SOURCE}\server.crt"
  File "${SCHAT_SOURCE}\server.key"

  File "${SCHAT_SOURCE}\libeay32.dll"
  File "${SCHAT_SOURCE}\QtCore4.dll"
  File "${SCHAT_SOURCE}\QtNetwork4.dll"
  File "${SCHAT_SOURCE}\QtSql4.dll"
  File "${SCHAT_SOURCE}\ssleay32.dll"
  File "${SCHAT_SOURCE}\zlib1.dll"

  !if ${SCHAT_VC100} == 1
    File "${SCHAT_SOURCE}\msvcp100.dll"
    File "${SCHAT_SOURCE}\msvcr100.dll"
  !endif
  
  SetOutPath "$INSTDIR\plugins"
  File "${SCHAT_SOURCE}\plugins\GenericNode.dll"
  File "${SCHAT_SOURCE}\plugins\Messages.dll"
  File "${SCHAT_SOURCE}\plugins\GeoIP.dll"

  SetOutPath "$INSTDIR\plugins\qt\sqldrivers"
  File "${SCHAT_SOURCE}\plugins\qt\sqldrivers\qsqlite4.dll"

  SetOutPath "$INSTDIR\doc"
  File "${SCHAT_SOURCE}\doc\ChangeLog.html"
  File "${SCHAT_SOURCE}\doc\ChangeLog.GenericNode.html"
  File "${SCHAT_SOURCE}\doc\ChangeLog.Messages.html"
  File "${SCHAT_SOURCE}\doc\ChangeLog.GeoIP.html"

  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "DisplayName"     "${SCHAT_NAME}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "UnInstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "DisplayIcon"     "$INSTDIR\schatd2.exe"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "Publisher"       "IMPOMEZIA"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "URLInfoAbout"    "${SCHAT_WEB_SITE}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "HelpLink"        "${SCHAT_WEB_SITE}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "URLUpdateInfo"   "${SCHAT_WEB_SITE}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "DisplayVersion"  "${SCHAT_VERSION}"
  
  WriteRegStr HKCU "${SCHAT_REGKEY}" "" $INSTDIR
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

Section "Uninstall"

  Delete "$INSTDIR\msvcp100.dll"
  Delete "$INSTDIR\msvcr100.dll"

  Delete "$INSTDIR\libeay32.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\QtSql4.dll"
  Delete "$INSTDIR\ssleay32.dll"
  Delete "$INSTDIR\zlib1.dll"

  Delete "$INSTDIR\server.crt"
  Delete "$INSTDIR\server.key"

  Delete "$INSTDIR\schatd2.exe"
  Delete "$INSTDIR\schatd.dll"
  Delete "$INSTDIR\license.txt"

  Delete "$INSTDIR\plugins\qt\sqldrivers\qsqlite4.dll"
  Delete "$INSTDIR\plugins\GenericNode.dll"
  Delete "$INSTDIR\plugins\Messages.dll"
  Delete "$INSTDIR\plugins\GeoIP.dll"

  Delete "$INSTDIR\doc\ChangeLog.html"
  Delete "$INSTDIR\doc\ChangeLog.GenericNode.html"
  Delete "$INSTDIR\doc\ChangeLog.Messages.html"
  Delete "$INSTDIR\doc\ChangeLog.GeoIP.html"

  RMDir "$INSTDIR\plugins\qt\sqldrivers"
  RMDir "$INSTDIR\plugins\qt"
  RMDir "$INSTDIR\doc"
  RMDir "$INSTDIR\plugins"

  Delete "$INSTDIR\uninstall.exe"
  DeleteRegKey HKLM "${SCHAT_UNINST_KEY}"
  RMDir "$INSTDIR"
SectionEnd
