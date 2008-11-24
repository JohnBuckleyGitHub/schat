/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright � 2008 IMPOMEZIA <schat@impomezia.com>
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
!include "engine\page-options.nsh"
!include "engine\update.nsh"

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

!if ${SCHAT_CHECK_RUN} == 1
 ReserveFile "contrib\plugins\FindProcDLL.dll"
 ReserveFile "contrib\plugins\KillProcDLL.dll"
!endif

!if ${SCHAT_FINISH_RUN} != ""
  !define MUI_FINISHPAGE_RUN "${SCHAT_FINISH_RUN}"
!endif

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
!insertmacro OPTIONS_PAGE
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"


/*
 * �������� ������.
 */
!undef SCHAT_META
!define SCHAT_SECTIONS
!include "engine\sections.nsh"
!insertmacro SECTION_OPTIONS


/*
 * ����������� ��������.
 */
!undef SCHAT_SECTIONS
!define SCHAT_POST
Section
  WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "Version" "${SCHAT_VERSION}"

  !include "engine\sections.nsh"

  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "DisplayName"     "${SCHAT_NAME} ${SCHAT_VERSION}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "UnInstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "DisplayIcon"     "$INSTDIR\schat.exe"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "Publisher"       "IMPOMEZIA"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "URLInfoAbout"    "${SCHAT_WEB_SITE}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "HelpLink"        "${SCHAT_WEB_SITE}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "URLUpdateInfo"   "${SCHAT_WEB_SITE}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "DisplayVersion"  "${SCHAT_VERSION}"
SectionEnd


/*
 * ��������.
 */
!undef SCHAT_POST
!define SCHAT_UNINSTALL
Section "Uninstall"
  Delete "$DESKTOP\${SCHAT_NAME}.lnk"
  Delete "$QUICKLAUNCH\${SCHAT_NAME}.lnk"
  Delete "${SCHAT_PROGRAMGROUP}\*.lnk"
  RMDir  "${SCHAT_PROGRAMGROUP}"

  !include "engine\sections.nsh"

  Delete "$INSTDIR\uninstall.ini"
  DeleteRegKey HKLM "${SCHAT_UNINST_KEY}"
  RMDir "$INSTDIR"
SectionEnd


/*
 * �������� ������.
 */
!undef SCHAT_UNINSTALL
!define SCHAT_DESC
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !if ${SCHAT_EMOTICON_GRP} == 1
    !insertmacro MUI_DESCRIPTION_TEXT ${GroupEmoticons} $(STR1004)
  !endif

  !include "engine\sections.nsh"
!insertmacro MUI_FUNCTION_DESCRIPTION_END


/*
 * �������������.
 */
!undef SCHAT_DESC
!define SCHAT_INIT
!insertmacro UPDATE_ENGINE_FUNCTIONS
!insertmacro GetParameters
!insertmacro GetOptionsS
!insertmacro GetParent

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
  !insertmacro UPDATE_CMD

  !insertmacro OPTIONS_PAGE_INIT
  !include "engine\sections.nsh"
FunctionEnd


/*
 * ������������� ��������������.
 */
Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
  !insertmacro UPDATE_ENGINE_INIT
FunctionEnd

!insertmacro OPTIONS_PAGE_FUNC
!insertmacro INSERT_TRANSLATIONS
!insertmacro FIND_RUNNING
!insertmacro UN_FIND_RUNNING
