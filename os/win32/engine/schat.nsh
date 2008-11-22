/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

!ifndef SCHAT_NSH_
!define SCHAT_NSH_

!include "engine\translations.nsh"

/*
 * Макрос для сохранения имени модуля внутри ${Body}.
 * Зависит от MUI2.nsh.
 */
!macro SCHAT_MOD_ID _ID
  !insertmacro MUI_UNSET MOD_ID
  !define MOD_ID "${_ID}"
!macroend


!define Meta "!insertmacro SCHAT_META "
!define MetaEnd "!endif"
!macro SCHAT_META _ID
  !ifdef SCHAT_META
    !insertmacro SCHAT_MOD_ID ${_ID}
!macroend


/*
 * Макрос контейнер для определения основной части модуля.
 * Используется парно: ${Body} и ${BodyEnd}.
 * Содержимое будет развёрнуто только при определении SCHAT_SECTIONS.
 */
!define Body "!insertmacro SCHAT_MOD_BODY "
!define BodyEnd "!endif"
!macro SCHAT_MOD_BODY _ID
  !ifdef SCHAT_SECTIONS
    !insertmacro SCHAT_MOD_ID ${_ID}
!macroend


/*
 * Парные макросы для обвёртки стандартной секция для модуля.
 * Используется парно: ${Section} и ${SectionEnd}
 * и должны находиться внутри ${Body}.
 */
!define Section "!insertmacro SCHAT_SECTION "
!macro SCHAT_SECTION
  Section "$(sec_${MOD_ID})" ${MOD_ID}_idx
    SetOverwrite on
!macroend

!define SectionEnd "!insertmacro SCHAT_SECTION_END "
!macro SCHAT_SECTION_END
  SectionEnd
!macroend

!endif /* SCHAT_NSH_ */
