/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

!ifndef PAGEOPTIONS_NSH_
!define PAGEOPTIONS_NSH_

!define SCHAT_PROGRAMGROUP "$SMPROGRAMS\${SCHAT_NAME}"

var DesktopCheckBox
var QuickLaunchCheckBox
var AllProgramsCheckBox
var AutostartCheckBox
var settings.Desktop
var settings.QuickLaunch
var settings.AllPrograms
var settings.AutoStart

!macro SETTINGS_PAGE
Function SettingsPage
  !insertmacro MUI_HEADER_TEXT "$(STR200)" "$(STR201)"
  
  nsDialogs::Create /NOUNLOAD 1018
  Pop $0
  
  ${NSD_CreateGroupBox} 0 0 100% 88 "$(STR100)"
  Pop $0
  
  ${NSD_CreateCheckbox} 10 20 90% 18 "$(STR101)"
  Pop $DesktopCheckBox
  ${NSD_SetState} $DesktopCheckBox $settings.Desktop
  
  ${NSD_CreateCheckbox} 10 42 90% 18 "$(STR102)"
  Pop $QuickLaunchCheckBox
  ${NSD_SetState} $QuickLaunchCheckBox $settings.QuickLaunch
  
  ${NSD_CreateCheckbox} 10 64 90% 18 "$(STR103)"
  Pop $AllProgramsCheckBox
  ${NSD_SetState} $AllProgramsCheckBox $settings.AllPrograms
  
  ${NSD_CreateCheckbox} 10 94 90% 18 "$(STR104)"
  Pop $AutostartCheckBox
  ${NSD_SetState} $AutoStartCheckBox $settings.AutoStart
  
  nsDialogs::Show

FunctionEnd

Function SettingsPageLeave
  ${NSD_GetState} $DesktopCheckBox     $settings.Desktop
  ${NSD_GetState} $QuickLaunchCheckBox $settings.QuickLaunch
  ${NSD_GetState} $AllProgramsCheckBox $settings.AllPrograms
  ${NSD_GetState} $AutoStartCheckBox   $settings.AutoStart
FunctionEnd
!macroend


/**
 * Результирующая обработка опций
 */
!macro SECTION_OPTIONS
Section
  SetOutPath "$INSTDIR"
  
  ${If} $settings.Desktop == ${BST_CHECKED}
    CreateShortCut "$DESKTOP\${SCHAT_NAME}.lnk" "$INSTDIR\schat.exe" "" "" "" "" "" "${SCHAT_NAME} ${SCHAT_VERSION}"
  ${EndIf}
  
  ${If} $settings.QuickLaunch == ${BST_CHECKED}
    CreateShortCut "$QUICKLAUNCH\${SCHAT_NAME}.lnk" "$INSTDIR\schat.exe" "" "" "" "" "" "${SCHAT_NAME} ${SCHAT_VERSION}"
  ${EndIf}
  
  ${If} $settings.AllPrograms == ${BST_CHECKED}
    IfFileExists    "${SCHAT_PROGRAMGROUP}\*.*" +2
    CreateDirectory "${SCHAT_PROGRAMGROUP}"
    CreateShortCut  "${SCHAT_PROGRAMGROUP}\$(STR300).lnk" "$INSTDIR\uninstall.exe" "" "" "" "" "" "${SCHAT_NAME} ${SCHAT_VERSION}"
    CreateShortCut  "${SCHAT_PROGRAMGROUP}\${SCHAT_NAME}.lnk" "$INSTDIR\schat.exe" "" "" "" "" "" "${SCHAT_NAME} ${SCHAT_VERSION}"
  ${EndIf}
  
  ${If} $settings.AutoStart == ${BST_CHECKED}
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${SCHAT_NAME}" "$INSTDIR\schat.exe -hide"
  ${EndIf}  
SectionEnd
!macroend


/**
 * Управляет состоянием опций в странице `SettingsPage`
 */
!define Option "!insertmacro Option"
!macro Option _KEY _DEF _VAR
  Push $0
  ClearErrors
  ReadIniStr $0 "$INSTDIR\uninstall.ini" "${SCHAT_NAME}" "${_KEY}"
  ${Unless} ${Errors}
    ${If} $0 == ${BST_CHECKED}
      StrCpy ${_VAR} $0
    ${Else}
      StrCpy ${_VAR} ${BST_UNCHECKED}
    ${EndIf}
  ${Else}
    StrCpy ${_VAR} ${_DEF}
  ${EndUnless}
  Pop $0
!macroend

!endif /* PAGEOPTIONS_NSH_ */