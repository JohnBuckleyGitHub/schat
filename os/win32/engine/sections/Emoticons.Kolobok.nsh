/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright � 2008 IMPOMEZIA <schat@impomezia.com>
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

${Name} Emoticons.Kolobok
${State} 1

${Body}
${Section}
  SetOutPath "$INSTDIR\emoticons\Kolobok"
  File "${SCHAT_DATADIR}\emoticons\Kolobok\*.png"
  File "${SCHAT_DATADIR}\emoticons\Kolobok\icondef.xml"
${SectionEnd}
${BodyEnd}

${Uninstall}
  Delete "$INSTDIR\emoticons\Kolobok\*.png"
  Delete "$INSTDIR\emoticons\Kolobok\icondef.xml"

  RMDir "$INSTDIR\emoticons\Kolobok"
  RMDir "$INSTDIR\emoticons"
${UninstallEnd}
