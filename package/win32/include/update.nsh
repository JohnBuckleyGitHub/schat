/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright пїЅ 2008 IMPOMEZIA (http://impomezia.net.ru)
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

!ifndef UPDATE_NSH_
!define UPDATE_NSH_

!include "FileFunc.nsh"

Var mode
Var cmdparams

/**
 * Инициализация
 */
!macro UPDATE_ENGINE_INIT
  ${un.GetParameters} $cmdparams
  ${un.GetOptionsS} $cmdparams "-update" $R0
  ${Unless} ${Errors}
    StrCpy $mode "update"
    Banner::show /NOUNLOAD /set 76 "${SCHAT_NAME} ${SCHAT_VERSION}" "Processing Update..."
    Sleep 4000
    Banner::destroy
  ${Else}
    StrCpy $mode "normal"
  ${EndUnless}
!macroend


/**
 * Запуск
 */
!macro UPDATE_ENGINE_EXEC

!macroend


/**
 * Функции
 */
!macro UPDATE_ENGINE_FUNCTIONS
  !insertmacro un.GetParameters
  !insertmacro un.GetOptionsS
!macroend

!endif /* UPDATE_NSH_ */
