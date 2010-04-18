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

${Name} Core

${Body}
${Section}
  SectionIn RO

  SetOutPath "$INSTDIR"
  File "${SCHAT_BINDIR}\schat.exe"
  File "license.txt"

  !if ${SCHAT_DEFAULT_CONF} == 1
    File /nonfatal "default.conf"
  !endif

  SetOutPath "$INSTDIR\networks"
  File "${SCHAT_DATADIR}\networks\*.xml"

  SetOutPath "$INSTDIR\doc"
  File "${SCHAT_DATADIR}\doc\*.html"
  
  SetOutPath "$INSTDIR\sounds"
  File "${SCHAT_DATADIR}\sounds\*.wav"

  SetOutPath "$INSTDIR\styles"

  WriteRegStr HKCU "${SCHAT_REGKEY}" "" $INSTDIR
  WriteUninstaller "$INSTDIR\uninstall.exe"
${SectionEnd}
${BodyEnd}

${Uninstall}
  Delete "$INSTDIR\schat.exe"
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\networks\*.xml"
  Delete "$INSTDIR\doc\*.html"
  Delete "$INSTDIR\networks\*.xml"
  Delete "$INSTDIR\sounds\*.wav"

  RMDir "$INSTDIR\doc"
  RMDir "$INSTDIR\networks"
  RMDir "$INSTDIR\sounds"
  RMDir "$INSTDIR\styles"

  DeleteRegKey HKCU "${SCHAT_REGKEY}"
${UninstallEnd}
