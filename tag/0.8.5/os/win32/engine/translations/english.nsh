/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

!ifndef ENGLISH_NSH_
!define ENGLISH_NSH_

!insertmacro MUI_UNSET CURRENT_LANG
!define CURRENT_LANG "L_EN"

${L} STR100                 "Shortcuts:"
${L} STR101                 "Desktop"
${L} STR102                 "Quick Launch"
${L} STR103                 "Start Menu"
${L} STR104                 "Add to autostart"
${L} STR105                 "Add server to autostart"
${L} STR200                 "Select additional tasks"
${L} STR300                 "Uninstall"
${L} STR301                 "Management of a server"
${L} STR400                 "An instance of ${SCHAT_NAME} is currently running. Exit ${SCHAT_NAME} and then try again."
${L} STR201                 "What additional tasks needed for executing?"
${L} STR1004                "Emoticons"
${L} sec_Core               "Core Components"
${L} desc_Core              "Core Components"
${L} sec_Qt                 "Qt Open Source Edition ${SCHAT_QT_VERSION}"
${L} desc_Qt                "Qt Open Source Edition ${SCHAT_QT_VERSION}"
${L} sec_Emoticons.Kolobok  "Kolobok"
${L} desc_Emoticons.Kolobok "Kolobok emoticons by Aiwan http://kolobok.us/"
${L} sec_Emoticons.Simple   "Simple Smileys"
${L} desc_Emoticons.Simple  "Simple Smileys by Leo Bolin http://leobolin.net/simplesmileys/"
${L} sec_Daemon             "Standalone Server"
${L} desc_Daemon            "Standalone Server and management tool"
${L} sec_Customize          "${SCHAT_NAME_SHORT} Customize"
${L} desc_Customize         "${SCHAT_NAME} Customize"

!endif /* ENGLISH_NSH_ */
