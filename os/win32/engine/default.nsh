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

${def} SCHAT_VERSION      "0.4.0.778"
${def} SCHAT_QT_VERSION   "4.4.3"
${def} SCHAT_NAME         "IMPOMEZIA Simple Chat"
${def} SCHAT_NAME_SHORT   "Simple Chat"
${def} SCHAT_PREFIX       ""
${def} SCHAT_SUFFIX       ""
${def} SCHAT_OUTDIR       "out/"
${def} SCHAT_FINISH_RUN   "$INSTDIR\schat.exe"
${def} SCHAT_REGKEY       "Software\IMPOMEZIA\${SCHAT_NAME_SHORT}"
${def} SCHAT_WEB_SITE     "http://impomezia.com/"
${def} SCHAT_COPYRIGHT    "Copyright © 2008 IMPOMEZIA"
${def} SCHAT_UNINST_KEY   "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SCHAT_NAME_SHORT}"
${def} SCHAT_INSTALLDIR   "$LOCALAPPDATA\IMPOMEZIA\${SCHAT_NAME_SHORT}"
${def} SCHAT_PAGE_OPTIONS 1
${def} SCHAT_EMOTICON_GRP 1
${def} SCHAT_CHECK_RUN    1
${def} SCHAT_VC90         1

${def} SCHAT_QTDIR      "C:\qt\443"
${def} SCHAT_BINDIR     "..\..\out\release"
${def} SCHAT_DATADIR    "..\..\data"
${def} VC90_REDIST_DIR  "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT"

!endif /* DEFAULT_NSH_ */
