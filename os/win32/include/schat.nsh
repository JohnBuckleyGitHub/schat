/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.com)
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

!AddPluginDir "contrib\plugins"

/**
* Выводим `MessageBox` если чат запущен.
*/
!macro _findRunningChat
  ${Unless} ${Silent}
    newcheck:
    FindProcDLL::FindProc "schat.exe"
    Pop $R0
    ${If} $R0 == 1 
      MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "$(STR400)" IDRETRY newcheck
      Quit
    ${EndIf}
  ${Else}
     !insertmacro KILL_ALL "schat.exe"
     !insertmacro KILL_ALL "schatd.exe"
     !insertmacro KILL_ALL "schatd-ui.exe"
  ${EndUnless}
!macroend

!macro findRunningChat
Function findRunningChat
  !insertmacro _findRunningChat
FunctionEnd
!macroend

!macro un.findRunningChat
Function un.findRunningChat
  !insertmacro _findRunningChat
FunctionEnd
!macroend


/**
 * Управляет выбором секции
 */
!define SectionState "!insertmacro SectionState"
!macro SectionState _KEY _DEF _SEC
  Push $0
  ClearErrors
  ReadIniStr $0 "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "${_KEY}"
  ${Unless} ${Errors}
    ${If} $0 != 1
    ${AndIf} $0 != 0
      StrCpy $0 ${_DEF}
    ${EndIf}
  ${Else}
    StrCpy $0 ${_DEF}
  ${EndUnless}
  
  ${If} $0 == 1
    !insertmacro SelectSection ${_SEC}
  ${Else}
    !insertmacro UnselectSection ${_SEC}
  ${EndIf}
  
  Pop $0
!macroend


/**
 * Завершает все процессы с указаным именем
 */
!macro KILL_ALL _NAME
  Push $0
  StrCpy $R0 1
  ${While} $R0 == 1
    KillProcDLL::KillProc "${_NAME}"
    Pop $R0
    FindProcDLL::FindProc "${_NAME}"
    Pop $R0
  ${EndWhile}
  Pop $R0 
!macroend


/**
 * Обработка ключа командной строки "-update"
 */
!macro UPDATE_CMD
  ${GetParameters} $R0
  ${GetOptionsS} $R0 "-update" $R0
  ${Unless} ${Errors}
    ${GetParent} "$EXEDIR" $R0
    StrCpy $INSTDIR $R0
  ${EndUnless}
!macroend


!macro SAVE_SECTION _SEC _KEY
  ${If} ${SectionIsSelected} ${_SEC}
    WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "${_KEY}" 1
  ${Else}
    WriteINIStr "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "${_KEY}" 0
  ${EndIf}
!macroend
