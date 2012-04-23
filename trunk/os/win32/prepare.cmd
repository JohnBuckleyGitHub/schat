@echo off
mkdir in\doc\
mkdir in\plugins\
mkdir in\plugins\qt\imageformats
mkdir in\plugins\qt\sqldrivers
mkdir in\translations\

copy %QTDIR%\bin\libeay32.dll in\
copy %QTDIR%\bin\libpng15.dll in\
copy %QTDIR%\bin\msvcp100.dll in\
copy %QTDIR%\bin\msvcr100.dll in\
copy %QTDIR%\bin\QtCore4.dll in\
copy %QTDIR%\bin\QtGui4.dll in\
copy %QTDIR%\bin\QtNetwork4.dll in\
copy %QTDIR%\bin\QtSql4.dll in\
copy %QTDIR%\bin\QtWebKit4.dll in\
copy %QTDIR%\bin\ssleay32.dll in\
copy %QTDIR%\bin\zlib1.dll in\

copy %QTDIR%\plugins\imageformats\qgif4.dll in\plugins\qt\imageformats\
copy %QTDIR%\plugins\imageformats\qico4.dll in\plugins\qt\imageformats\
copy %QTDIR%\plugins\imageformats\qjpeg4.dll in\plugins\qt\imageformats\
copy %QTDIR%\plugins\sqldrivers\qsqlite4.dll in\plugins\qt\sqldrivers\

copy %QTDIR%\translations\qt_ru.qm in\translations\

copy ..\..\out\schat2.exe in\
copy ..\..\out\schatd2.exe in\
copy ..\..\out\schat.dll in\
copy ..\..\out\schat-client.dll in\
copy ..\..\out\schatd.dll in\

copy ..\..\misc\tools\server.crt in\
copy ..\..\misc\tools\server.key in\


copy ..\..\out\plugins\Cache.dll in\plugins\
copy ..\..\out\plugins\GenericNode.dll in\plugins\
copy ..\..\out\plugins\History.dll in\plugins\
copy ..\..\out\plugins\Messages.dll in\plugins\
copy ..\..\out\plugins\RawFeeds.dll in\plugins\
copy ..\..\out\plugins\Profile.dll in\plugins\
copy ..\..\out\plugins\GeoIP.dll in\plugins\

copy ..\..\res\doc\*.html in\doc\

copy ..\..\res\translations\ru.png in\translations\
copy ..\..\res\translations\schat2_en.qm in\translations\
copy ..\..\res\translations\schat2_ru.qm in\translations\
copy ..\..\res\translations\profile_en.qm in\translations\
copy ..\..\res\translations\profile_ru.qm in\translations\