/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

!include "MUI2.nsh"
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
SetCompressor /SOLID lzma
SetCompressorDictSize 12
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
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
  call findRunningChat
  
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
