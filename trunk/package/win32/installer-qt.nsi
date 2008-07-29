/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "include\common.nsh"
!include "include\schat.nsh"
!include "include\sections.nsh"

!define SCHAT_OUT_FILENAME "schat-runtime-${SCHAT_VERSION}.exe"

Name "${SCHAT_NAME}"
Caption "${SCHAT_NAME} ${SCHAT_VERSION}"
OutFile "out\updates\win32\${SCHAT_OUT_FILENAME}"
InstallDir "${SCHAT_INSTALLDIR}"
InstallDirRegKey HKCU "${SCHAT_REGKEY}" ""
RequestExecutionLevel user
SetCompressor /SOLID lzma
SetCompressorDictSize 8
;SetCompress off
BrandingText "${SCHAT_COPYRIGHT}"

VIProductVersion "${SCHAT_VERSION}"
VIAddVersionKey  "CompanyName"      "IMPOMEZIA"
VIAddVersionKey  "FileDescription"  "${SCHAT_NAME} Installer"
VIAddVersionKey  "FileVersion"      "${SCHAT_VERSION}"
VIAddVersionKey  "LegalCopyright"   "${SCHAT_COPYRIGHT}"
VIAddVersionKey  "OriginalFilename" "${SCHAT_OUT_FILENAME}"
VIAddVersionKey  "ProductName"      "${SCHAT_NAME}"
VIAddVersionKey  "ProductVersion"   "${SCHAT_VERSION}"

!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "English"

Section
  !insertmacro RUNTIME_FILES
SectionEnd

!insertmacro GetParameters
!insertmacro GetOptionsS
!insertmacro GetParent

Function .onInit
  !insertmacro UPDATE_CMD
FunctionEnd
