/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
    ${EndIf}
  ${Else}
     !insertmacro KILL_ALL "schat.exe"
     !insertmacro KILL_ALL "schatd.exe"
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
