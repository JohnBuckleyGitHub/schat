/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

!ifndef mod
  !define mod "!insertmacro SCHAT_MOD "
!endif
!ifmacrondef SCHAT_MOD
  !macro SCHAT_MOD _NAME
    !ifdef ${_NAME} | SCHAT_UNINSTALL
      !include "engine\sections\${_NAME}.nsh"
    !endif
  !macroend
!endif

${mod} Core
${mod} Qt

!ifdef SCHAT_SECTIONS
  !if ${SCHAT_EMOTICON_GRP} == 1
    SectionGroup "$(STR1004)" GroupEmoticons
  !endif
!endif
${mod} Emoticons.Kolobok
${mod} Emoticons.Simple
!ifdef SCHAT_SECTIONS
  !if ${SCHAT_EMOTICON_GRP} == 1
    SectionGroupEnd
  !endif
!endif

${mod} Daemon
${mod} Customize
