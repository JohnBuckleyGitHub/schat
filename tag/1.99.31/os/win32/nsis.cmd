@echo off
cd ../../out
for %%I in (*.exe *.dll) do SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode %%~fI

cd plugins
for %%I in (*.dll) do SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode %%~fI

cd ../..
nmake install
if ERRORLEVEL 1 exit 1

cd os/win32
makensis setup.nsi
if ERRORLEVEL 1 exit 1

makensis server.nsi
if ERRORLEVEL 1 exit 1

cd out
for %%I in (*.exe) do SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode %%~fI

cd ..
exit 0