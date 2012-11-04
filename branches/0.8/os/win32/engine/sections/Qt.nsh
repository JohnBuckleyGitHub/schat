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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

${Name} Qt

${Body}
${Section}
  SectionIn RO

  SetOutPath "$INSTDIR"
  File "${SCHAT_QT_BINDIR}\QtCore4.dll"
  File "${SCHAT_QT_BINDIR}\QtGui4.dll"
  File "${SCHAT_QT_BINDIR}\QtNetwork4.dll"
  File "${SCHAT_QT_BINDIR}\QtWebKit4.dll"
  File "${SCHAT_QT_BINDIR}\libpng15.dll"
  File "${SCHAT_QT_BINDIR}\zlib.dll"

  !if ${SCHAT_VC90} == 1
    File "${VC90_REDIST_DIR}\msvcp90.dll"
    File "${VC90_REDIST_DIR}\msvcr90.dll"
    File "contrib\Microsoft.VC90.CRT.manifest"
  !endif
  
  !if ${SCHAT_VC100} == 1
    File "${VC100_REDIST_DIR}\msvcp100.dll"
    File "${VC100_REDIST_DIR}\msvcr100.dll"
    Delete "$INSTDIR\msvcp90.dll"
    Delete "$INSTDIR\msvcr90.dll"
    Delete "$INSTDIR\Microsoft.VC90.CRT.manifest"
  !endif

  SetOutPath "$INSTDIR\plugins\imageformats"
  File "${SCHAT_QTDIR}\plugins\imageformats\qgif4.dll"
  File "${SCHAT_QTDIR}\plugins\imageformats\qico4.dll"
  File "${SCHAT_QTDIR}\plugins\imageformats\qjpeg4.dll"
${SectionEnd}
${BodyEnd}


${Uninstall}
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\QtWebKit4.dll"
  Delete "$INSTDIR\libpng15.dll"
  Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\msvcp90.dll"
  Delete "$INSTDIR\msvcr90.dll"
  Delete "$INSTDIR\msvcp100.dll"
  Delete "$INSTDIR\msvcr100.dll"
  Delete "$INSTDIR\Microsoft.VC90.CRT.manifest"
  Delete "$INSTDIR\plugins\imageformats\qgif4.dll"
  Delete "$INSTDIR\plugins\imageformats\qico4.dll"
  Delete "$INSTDIR\plugins\imageformats\qjpeg4.dll"

  RMDir "$INSTDIR\plugins\imageformats"
  RMDir "$INSTDIR\plugins"
${UninstallEnd}
