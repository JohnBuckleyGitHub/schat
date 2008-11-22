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

!ifndef DEFAULT_NSH_
!define DEFAULT_NSH_

!define def "!insertmacro DEFAULT "
!macro DEFAULT _PARAM _VALUE
  !ifndef ${_PARAM}
    !define ${_PARAM} "${_VALUE}"
  !endif
!macroend

${def} SCHAT_VERSION    "0.4.0.0"
${def} SCHAT_NAME       "IMPOMEZIA Simple Chat"
${def} SCHAT_NAME_SHORT "Simple Chat"
${def} SCHAT_PREFIX     ""
${def} SCHAT_SUFFIX     ""
${def} SCHAT_OUTDIR     ""
${def} SCHAT_REGKEY     "Software\IMPOMEZIA\${SCHAT_NAME_SHORT}"
${def} SCHAT_QTDIR      "C:\qt\443"
${def} SCHAT_WEB_SITE   "http://impomezia.com/"
${def} SCHAT_COPYRIGHT  "Copyright © 2008 IMPOMEZIA"
${def} SCHAT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SCHAT_NAME_SHORT}"
${def} SCHAT_INSTALLDIR "$LOCALAPPDATA\IMPOMEZIA\${SCHAT_NAME_SHORT}"

!endif /* DEFAULT_NSH_ */
