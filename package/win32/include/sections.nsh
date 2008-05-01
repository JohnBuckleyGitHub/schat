/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

!ifndef SECTIONS_NSH_
!define SECTIONS_NSH_
 
!macro  SECTION_CORE
!define SECTION_CORE
Section "$(STR1000)" SecCore

  SetOutPath "$INSTDIR"
  SectionIn RO
  
  File "..\..\out\release\schat.exe"
  File "${SCHAT_QTDIR}\bin\QtCore4.dll"
  File "${SCHAT_QTDIR}\bin\QtGui4.dll"
  File "${SCHAT_QTDIR}\bin\QtNetwork4.dll"
  
  WriteRegStr HKCU "${SCHAT_REGKEY}" "" $INSTDIR
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd
!macroend


!macro  SECTION_SERVER
!define SECTION_SERVER
Section "$(STR1002)" SecServer

  SetOutPath "$INSTDIR"
  File "..\..\out\release\schatd.exe"

SectionEnd
!macroend


!macro SECTION_UNINSTALL
Section "Uninstall"

  Delete "$DESKTOP\${SCHAT_NAME}.lnk"
  Delete "$QUICKLAUNCH\${SCHAT_NAME}.lnk"
  Delete "${SCHAT_PROGRAMGROUP}\*.lnk"
  RMDir  "${SCHAT_PROGRAMGROUP}"
  
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\schat.exe"
  Delete "$INSTDIR\schatd.exe"
  Delete "$INSTDIR\schat.conf"
  Delete "$INSTDIR\schatd.conf"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"

  RMDir  "$INSTDIR"
  
  DeleteRegKey HKLM "${SCHAT_UNINST_KEY}"
  DeleteRegKey HKCU "${SCHAT_REGKEY}"
  
SectionEnd
!macroend


!macro SECTION_DESC
  
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !ifdef SECTION_CORE
    !insertmacro MUI_DESCRIPTION_TEXT ${SecCore} $(STR1001)
  !endif
  !ifdef SECTION_SERVER
    !insertmacro MUI_DESCRIPTION_TEXT ${SecServer} $(STR1003)
  !endif
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

!insertmacro INSERT_TRANSLATIONS
!macroend

!endif /* SECTIONS_NSH_ */