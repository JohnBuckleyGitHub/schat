@echo off
mkdir in\doc\
mkdir in\plugins\
mkdir in\translations\

copy ..\..\out\schat2.exe in\
copy ..\..\out\schatd2.exe in\
copy ..\..\out\schat.dll in\
copy ..\..\out\schat-client.dll in\
copy ..\..\out\schatd.dll in\
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

pause