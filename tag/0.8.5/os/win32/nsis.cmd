@echo off
cd ../../out/release
for %%I in (*.exe) do SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode %%~fI

cd ../../os/win32

makensis setup.nsi
if ERRORLEVEL 1 exit 1

makensis setup-core.nsi
if ERRORLEVEL 1 exit 1

makensis setup-customize.nsi
if ERRORLEVEL 1 exit 1

makensis setup-runtime.nsi
if ERRORLEVEL 1 exit 1

cd out
for %%I in (*.exe) do SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode %%~fI

exit 0