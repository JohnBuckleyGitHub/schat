!AddPluginDir '..\build\ReadTagDLL\MinSizeRel'

Name "ReadTagDLL.dll test"
RequestExecutionLevel user
OutFile "ReadTagTest.exe"
InstallDir "$LOCALAPPDATA\IMPOMEZIA\ReadTag Test"
ShowInstDetails show

Function .onInit
FunctionEnd

Section
  SetOutPath "$INSTDIR"
  File "test.nsi"
;  MessageBox MB_OK $INSTDIR
  ReadTagDLL::extract "$INSTDIR\default.conf"
SectionEnd