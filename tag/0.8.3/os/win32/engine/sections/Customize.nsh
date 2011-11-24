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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

${Name} Customize
${State} 1

${Body}
${Section}
  SetOutPath "$INSTDIR\custom\contrib\plugins"
  File "contrib\plugins\FindProcDLL.dll"
  File "contrib\plugins\KillProcDLL.dll"
  File "contrib\plugins\plugins.txt"

  SetOutPath "$INSTDIR\custom\contrib"
  File "contrib\header.bmp"
  File "contrib\wizard.bmp"
  File "contrib\install.ico"
  File "contrib\Microsoft.VC90.CRT.manifest"

  SetOutPath "$INSTDIR\custom\engine\sections"
  File "engine\sections\*.nsh"

  SetOutPath "$INSTDIR\custom\engine\translations"
  File "engine\translations\*.nsh"

  SetOutPath "$INSTDIR\custom\engine"
  File "engine\*.nsh"

  SetOutPath "$INSTDIR\custom\out"
  SetOutPath "$INSTDIR\custom"
  File "custom\setup-runtime.nsi"
  File "custom\setup-core.nsi"
  File "custom\setup.nsi"
  File "license.txt"

  SetOutPath "$INSTDIR"
  File "${SCHAT_BINDIR}\customize.exe"
${SectionEnd}
${BodyEnd}


${Uninstall}
  Delete "$INSTDIR\custom\contrib\plugins\FindProcDLL.dll"
  Delete "$INSTDIR\custom\contrib\plugins\KillProcDLL.dll"
  Delete "$INSTDIR\custom\contrib\plugins\plugins.txt"

  Delete "$INSTDIR\custom\contrib\header.bmp"
  Delete "$INSTDIR\custom\contrib\wizard.bmp"
  Delete "$INSTDIR\custom\contrib\install.ico"
  Delete "$INSTDIR\custom\contrib\Microsoft.VC90.CRT.manifest"

  Delete "$INSTDIR\custom\engine\sections\*.nsh"
  Delete "$INSTDIR\custom\engine\translations\*.nsh"
  Delete "$INSTDIR\custom\engine\*.nsh"

  Delete "$INSTDIR\custom\setup-runtime.nsi"
  Delete "$INSTDIR\custom\setup-core.nsi"
  Delete "$INSTDIR\custom\setup.nsi"
  Delete "$INSTDIR\custom\license.txt"

  Delete "$INSTDIR\customize.exe"

  RMDir "$INSTDIR\custom\contrib\plugins"
  RMDir "$INSTDIR\custom\contrib"
  RMDir "$INSTDIR\custom\engine\sections"
  RMDir "$INSTDIR\custom\engine\translations"
  RMDir "$INSTDIR\custom\out"
  RMDir "$INSTDIR\custom\"
  RMDir "$INSTDIR"
${UninstallEnd}
