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

${Name} Daemon
${State} 0

${Body}
${Section}
  SetOutPath "$INSTDIR"
  File "${SCHAT_BINDIR}\schatd.exe"
  File "${SCHAT_BINDIR}\schatd-ui.exe"
  
  SetOverwrite ifnewer
  File "${SCHAT_DATADIR}\motd.html"
  File "${SCHAT_DATADIR}\normalize.xml"
  SetOverwrite on

  SetOutPath "$INSTDIR\translations"
  File "${SCHAT_DATADIR}\translations\schatd_en.qm"
  File "${SCHAT_DATADIR}\translations\schatd_ru.qm"
  File "${SCHAT_DATADIR}\translations\schatd_uk.qm"
  File "${SCHAT_DATADIR}\translations\qt_ru.qm"
  File "${SCHAT_DATADIR}\translations\qt_uk.qm"
  File "${SCHAT_DATADIR}\translations\schatd-ui_ru.qm"
  File "${SCHAT_DATADIR}\translations\schatd-ui_uk.qm"
${SectionEnd}
${BodyEnd}

${Uninstall}

  Delete "$INSTDIR\translations\schatd_en.qm"
  Delete "$INSTDIR\translations\schatd_ru.qm"
  Delete "$INSTDIR\translations\schatd_uk.qm"
  Delete "$INSTDIR\translations\qt_ru.qm"
  Delete "$INSTDIR\translations\qt_uk.qm"
  Delete "$INSTDIR\translations\schatd-ui_ru.qm"
  Delete "$INSTDIR\translations\schatd-ui_uk.qm"
  RMDir "$INSTDIR\translations"

  Delete "$INSTDIR\schatd.exe"
  Delete "$INSTDIR\schatd-ui.exe"
  Delete "$INSTDIR\motd.html"
  Delete "$INSTDIR\normalize.xml"
${UninstallEnd}
