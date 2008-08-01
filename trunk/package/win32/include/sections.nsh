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
 
!ifndef SECTIONS_NSH_
!define SECTIONS_NSH_

!macro RUNTIME_FILES
  !ifndef INSTALLER_CORE
    SetOutPath "$INSTDIR"
    File "${SCHAT_QTDIR}\bin\QtCore4.dll"
    File "${SCHAT_QTDIR}\bin\QtGui4.dll"
    File "${SCHAT_QTDIR}\bin\QtNetwork4.dll"
    !ifdef VC90
      File "${VC90_REDIST_DIR}\msvcr90.dll"
    !endif    
  !endif
!macroend

 
!macro  SECTION_CORE
!define SECTION_CORE
Section "$(STR1000)" SecCore

  SetOutPath "$INSTDIR"
  SectionIn RO
  
  File "..\..\out\release\schat.exe"
  File "..\..\out\release\update.exe"
  
  !ifdef VC90
    File "contrib\Microsoft.VC90.CRT.manifest"
  !endif

  !insertmacro RUNTIME_FILES
  
  SetOutPath "$INSTDIR\networks"
  File "..\..\data\networks\*.xml"
  
  SetOutPath "$INSTDIR\doc"
  File "..\..\data\doc\*.html"
  File "..\..\data\doc\*.example"
  
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
  
  ${If} $mode == "normal"
    Delete "$DESKTOP\${SCHAT_NAME}.lnk"
    Delete "$QUICKLAUNCH\${SCHAT_NAME}.lnk"
    Delete "${SCHAT_PROGRAMGROUP}\*.lnk"
    RMDir  "${SCHAT_PROGRAMGROUP}"
    
    Delete "$INSTDIR\uninstall.exe"
    Delete "$INSTDIR\uninstall.ini"
    Delete "$INSTDIR\schat.exe"
    Delete "$INSTDIR\schatd.exe"
    Delete "$INSTDIR\update.exe"
    Delete "$INSTDIR\QtCore4.dll"
    Delete "$INSTDIR\QtGui4.dll"
    Delete "$INSTDIR\QtNetwork4.dll"
    Delete "$INSTDIR\networks\*.xml"
    Delete "$INSTDIR\doc\*.html"
    Delete "$INSTDIR\doc\*.example"
    
    !ifdef VC90
      Delete "$INSTDIR\msvcr90.dll"
      Delete "$INSTDIR\Microsoft.VC90.CRT.manifest"
    !endif
    
    RMDir "$INSTDIR\networks"
    RMDir "$INSTDIR\doc"
    RMDir "$INSTDIR"
    
    DeleteRegKey HKLM "${SCHAT_UNINST_KEY}"
    DeleteRegKey HKCU "${SCHAT_REGKEY}"
  ${EndIf}
  
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