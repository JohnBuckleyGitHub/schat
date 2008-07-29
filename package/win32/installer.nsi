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
!include "Sections.nsh"
!include "include\common.nsh"
!include "include\sections.nsh"
!include "include\schat.nsh"
!include "include\translations.nsh"
!include "include\page-options.nsh"

!ifdef INSTALLER_CORE
  !define SCHAT_OUT_FILENAME "schat-core-${SCHAT_VERSION}.exe"
!else
  !define SCHAT_OUT_FILENAME "schat-${SCHAT_VERSION}.exe"
!endif

Name "${SCHAT_NAME}"
Caption "${SCHAT_NAME} ${SCHAT_VERSION}"

!ifdef INSTALLER_CORE
  OutFile "out\updates\win32\${SCHAT_OUT_FILENAME}"
!else
  OutFile "out\${SCHAT_OUT_FILENAME}"
!endif

InstallDir "${SCHAT_INSTALLDIR}"
InstallDirRegKey HKCU "${SCHAT_REGKEY}" ""
RequestExecutionLevel user
InstType "$(STR1)"
;SetCompressor /SOLID lzma
;SetCompressorDictSize 10
SetCompress off
BrandingText "${SCHAT_COPYRIGHT}"

VIProductVersion "${SCHAT_VERSION}"
VIAddVersionKey  "CompanyName"      "IMPOMEZIA"
VIAddVersionKey  "FileDescription"  "${SCHAT_NAME} Installer"
VIAddVersionKey  "FileVersion"      "${SCHAT_VERSION}"
VIAddVersionKey  "LegalCopyright"   "${SCHAT_COPYRIGHT}"
VIAddVersionKey  "OriginalFilename" "${SCHAT_OUT_FILENAME}"
VIAddVersionKey  "ProductName"      "${SCHAT_NAME}"
VIAddVersionKey  "ProductVersion"   "${SCHAT_VERSION}"

ReserveFile "contrib\plugins\FindProcDLL.dll"
ReserveFile "contrib\plugins\KillProcDLL.dll"

!define MUI_FINISHPAGE_RUN "$INSTDIR\schat.exe"

!insertmacro MUI_RESERVEFILE_LANGDLL
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
Page custom SettingsPage SettingsPageLeave
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "English"

/**
 * Секции
 */
!insertmacro SECTION_CORE
!insertmacro SECTION_SERVER
!insertmacro SECTION_OPTIONS
!insertmacro SECTION_DESC
!insertmacro SECTION_UNINSTALL


/**
 * Завершающие действия
 */
Section
  WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "Version"     "${SCHAT_VERSION}"
  
  !ifdef SECTION_SERVER
  ${If} ${SectionIsSelected} ${SecServer}
    WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "Server" 1
  ${Else}
    WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "Server" 0
  ${EndIf}
  !endif
  
  WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "Desktop"     "$settings.Desktop"
  WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "QuickLaunch" "$settings.QuickLaunch"
  WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "AllPrograms" "$settings.AllPrograms"
  WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "AutoStart"   "$settings.AutoStart"
  
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "DisplayName"     "${SCHAT_NAME} ${SCHAT_VERSION}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "UnInstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "DisplayIcon"     "$INSTDIR\schat.exe"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "Publisher"       "IMPOMEZIA"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "URLInfoAbout"    "${SCHAT_WEB_SITE}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "HelpLink"        "${SCHAT_WEB_SITE}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "URLUpdateInfo"   "${SCHAT_WEB_SITE}"
  WriteRegStr HKLM "${SCHAT_UNINST_KEY}" "DisplayVersion"  "${SCHAT_VERSION}"
SectionEnd


/**
 * Инициализация
 */
!insertmacro GetParameters
!insertmacro GetOptionsS
!insertmacro GetParent

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
  call findRunningChat
  
  !insertmacro UPDATE_CMD
    
  ${Option} "Desktop"     0 $settings.Desktop
  ${Option} "QuickLaunch" 1 $settings.QuickLaunch
  ${Option} "AllPrograms" 1 $settings.AllPrograms
  ${Option} "AutoStart"   1 $settings.AutoStart
  
  ${SectionState} "Server" 0 ${SecServer}
FunctionEnd


Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
  call un.findRunningChat
FunctionEnd


!insertmacro findRunningChat
!insertmacro un.findRunningChat
!insertmacro SETTINGS_PAGE
