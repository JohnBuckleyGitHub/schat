/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

${Name} Core

${Body}
${Section}
  SectionIn RO

  SetOutPath "$INSTDIR"
  File "${SCHAT_SOURCE}\schat2.exe"
  File "${SCHAT_SOURCE}\schat.dll"
  File "${SCHAT_SOURCE}\schat-client.dll"
  File "license.txt"

  SetOutPath "$INSTDIR\doc"
  File "${SCHAT_SOURCE}\doc\ChangeLog.html"
  
  SetOutPath "$INSTDIR\translations"
  File "${SCHAT_SOURCE}\translations\schat2_en.qm"
  File "${SCHAT_SOURCE}\translations\schat2_ru.qm"
  File "${SCHAT_SOURCE}\translations\qt_ru.qm"
  File "${SCHAT_SOURCE}\translations\ru.png"

  WriteRegStr HKCU "${SCHAT_REGKEY}" "" $INSTDIR
  WriteUninstaller "$INSTDIR\uninstall.exe"
${SectionEnd}
${BodyEnd}

${Uninstall}
  Delete "$INSTDIR\schat2.exe"
  Delete "$INSTDIR\schat.dll"
  Delete "$INSTDIR\schat-client.dll"
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\doc\ChangeLog.html"
  Delete "$INSTDIR\translations\schat2_en.qm"
  Delete "$INSTDIR\translations\schat2_ru.qm"
  Delete "$INSTDIR\translations\qt_ru.qm"
  Delete "$INSTDIR\translations\ru.png"

  RMDir "$INSTDIR\doc"
  RMDir "$INSTDIR\translations"

  DeleteRegKey HKCU "${SCHAT_REGKEY}"
${UninstallEnd}
