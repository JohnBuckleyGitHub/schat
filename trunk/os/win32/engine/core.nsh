/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

!include "MUI2.nsh"
!include "engine\default.nsh"
!include "engine\schat.nsh"

!define SCHAT_META
!include "engine\sections.nsh"

BrandingText "${SCHAT_COPYRIGHT}"
Caption "${SCHAT_NAME} ${SCHAT_VERSION}"
InstallDir "${SCHAT_INSTALLDIR}"
InstallDirRegKey HKCU "${SCHAT_REGKEY}" ""
Name "${SCHAT_NAME}"
OutFile "${SCHAT_OUTDIR}schat-${SCHAT_PREFIX}${SCHAT_VERSION}${SCHAT_SUFFIX}.exe"
RequestExecutionLevel user

SetCompressor /SOLID lzma
SetCompressorDictSize 10

VIProductVersion "${SCHAT_VERSION}"
VIAddVersionKey  "CompanyName"      "IMPOMEZIA"
VIAddVersionKey  "FileDescription"  "${SCHAT_NAME} Installer"
VIAddVersionKey  "FileVersion"      "${SCHAT_VERSION}"
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
!define MUI_WELCOMEFINISHPAGE_BITMAP   "contrib\wizard.bmp"
!define MUI_LANGDLL_REGISTRY_ROOT      "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY       "${SCHAT_REGKEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "language"
!define MUI_LANGDLL_ALWAYSSHOW
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TITLE_3LINES

!insertmacro MUI_RESERVEFILE_LANGDLL

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"

!undef SCHAT_META
!define SCHAT_SECTIONS
!include "engine\sections.nsh"

!insertmacro INSERT_TRANSLATIONS
