/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

!define SCHAT_NAME     "Simple Chat" 
!define SCHAT_VERSION  "0.0.1.81"
!define SCHAT_REGKEY   "Software\IMPOMEZIA\${SCHAT_NAME}"
!define SCHAT_QTDIR    "C:\qt\4.4.0-beta1"
!define SCHAT_WEB_SITE "http://impomezia.net.ru/"

!include "MUI2.nsh"
!include "include\sections.nsh"
!include "include\translations.nsh"

Name "${SCHAT_NAME} ${SCHAT_VERSION}"
OutFile "out\schat-${SCHAT_VERSION}.exe"
InstallDir "$LOCALAPPDATA\${SCHAT_NAME}"
InstallDirRegKey HKCU "${SCHAT_REGKEY}" ""
RequestExecutionLevel user
InstType "$(STR1)"
SetCompressor /SOLID lzma
SetCompressorDictSize 12
;SetCompress off

VIProductVersion "${SCHAT_VERSION}"
VIAddVersionKey  "CompanyName"      "IMPOMEZIA"
VIAddVersionKey  "FileDescription"  "${SCHAT_NAME} Installer"
VIAddVersionKey  "FileVersion"      "${SCHAT_VERSION}"
VIAddVersionKey  "LegalCopyright"   "Copyright © 2008 IMPOMEZIA"
VIAddVersionKey  "OriginalFilename" "schat-${SCHAT_VERSION}.exe"
VIAddVersionKey  "ProductName"      "${SCHAT_NAME}"
VIAddVersionKey  "ProductVersion"   "${SCHAT_VERSION}"

!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_FINISHPAGE_LINK            "${SCHAT_WEB_SITE}"
!define MUI_FINISHPAGE_LINK_LOCATION   "${SCHAT_WEB_SITE}"
!define MUI_FINISHPAGE_RUN             "$INSTDIR\schat.exe"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP         "contrib\header.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                       "contrib\install.ico"
!define MUI_UNICON                     "contrib\uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP   "contrib\wizard.bmp"
!define MUI_LANGDLL_REGISTRY_ROOT      "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY       "${SCHAT_REGKEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "language"
!define MUI_LANGDLL_ALWAYSSHOW

!insertmacro MUI_RESERVEFILE_LANGDLL
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
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
!insertmacro SECTION_DESC
!insertmacro SECTION_UNINSTALL


/**
 * Инициализация
 */
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd


Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd
