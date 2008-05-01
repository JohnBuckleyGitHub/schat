/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */
 

/**
* Выводим `MessageBox` если чат запущен.
*/
!macro _findRunningChat
    Push $0
  newcheck:
    FindWindow $0 "QWidget" "${SCHAT_NAME}" 0
    IntCmp $0 0 done
    MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "$(STR400)" IDRETRY newcheck
    Quit
  done:
    Pop $0
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
