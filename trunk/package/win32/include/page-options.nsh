/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
  
  ${NSD_CreateCheckbox} 10 42 90% 18 "$(STR102)"
  Pop $QuickLaunchCheckBox
  ${NSD_Check} $QuickLaunchCheckBox
  
  ${NSD_CreateCheckbox} 10 64 90% 18 "$(STR103)"
  Pop $AllProgramsCheckBox
  ${NSD_Check} $AllProgramsCheckBox
  
  ${NSD_CreateCheckbox} 10 94 90% 18 "$(STR104)"
  Pop $AutostartCheckBox
  ${NSD_Check} $AutoStartCheckBox
  
  nsDialogs::Show

FunctionEnd

Function SettingsPageLeave
  ${NSD_GetState} $DesktopCheckBox     $settings.Desktop
  ${NSD_GetState} $QuickLaunchCheckBox $settings.QuickLaunch
  ${NSD_GetState} $AllProgramsCheckBox $settings.AllPrograms
  ${NSD_GetState} $AutoStartCheckBox   $settings.AutoStart
FunctionEnd
!macroend

!macro SECTION_OPTIONS
Section
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
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "${SCHAT_NAME}" "$INSTDIR\schat.exe"
  ${EndIf}  
SectionEnd
!macroend

!endif /* PAGEOPTIONS_NSH_ */