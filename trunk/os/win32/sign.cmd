@echo off
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/schat2.exe
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/schatd2.exe
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/schat-client.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/schat.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/schatd.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/Cache.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/GenericNode.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/RawFeeds.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/Messages.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/History.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/Profile.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/Emoticons.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/GeoIP.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/Update.dll
SignTool.exe sign /f %SCHAT_SIGN_FILE% /p %SCHAT_SIGN_PASSWORD% /t http://timestamp.comodoca.com/authenticode ../../out/plugins/Idle.dll
