/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

!define SCHAT_NAME    "Simple Chat" 
!define SCHAT_VERSION "0.0.1.79"
!define SCHAT_REGKEY  "Software\IMPOMEZIA\${SCHAT_NAME}"
!define SCHAT_QTDIR   "C:\qt\4.4.0-beta1"

!include "MUI2.nsh"

Name "${SCHAT_NAME} ${SCHAT_VERSION}"
OutFile "out\schat-${SCHAT_VERSION}.exe"
InstallDir "$PROGRAMFILES\${SCHAT_NAME}"
InstallDirRegKey HKCU "${SCHAT_REGKEY}" ""
RequestExecutionLevel user
InstType "Recommended"
SetCompressor /SOLID lzma
SetCompressorDictSize 12

!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_WELCOMEFINISHPAGE_BITMAP "contrib\wizard.bmp"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Russian"


Section "Dummy Section" SecDummy

  SetOutPath "$INSTDIR"
  SectionIn RO
  
  File "..\..\out\release\schat.exe"
  File "..\..\out\release\schatd.exe"
  File "${SCHAT_QTDIR}\bin\QtCore4.dll"
  File "${SCHAT_QTDIR}\bin\QtGui4.dll"
  File "${SCHAT_QTDIR}\bin\QtNetwork4.dll"
  
  WriteRegStr HKCU "${SCHAT_REGKEY}" "" $INSTDIR
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\Modern UI Test"

SectionEnd
