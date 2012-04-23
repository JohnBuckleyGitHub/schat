@echo off
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode out\schat2-%SCHAT_VERSION%.exe
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode out\schat2-server-%SCHAT_VERSION%.exe