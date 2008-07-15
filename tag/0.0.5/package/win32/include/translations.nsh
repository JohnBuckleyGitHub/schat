/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

!ifndef TRANSLATIONS_NSH_
!define TRANSLATIONS_NSH_

!define L "!insertmacro ${CURRENT_LANG}"

!macro L_EN _NAME _TEXT
  LangString ${_NAME} ${LANG_ENGLISH} "${_TEXT}"
!macroend

!macro L_RU _NAME _TEXT
  LangString ${_NAME} ${LANG_RUSSIAN} "${_TEXT}"
!macroend

!macro INSERT_TRANSLATIONS
  !include "include\translations\english.nsh"
  !include "include\translations\russian.nsh"
!macroend

!endif /* TRANSLATIONS_NSH_ */
