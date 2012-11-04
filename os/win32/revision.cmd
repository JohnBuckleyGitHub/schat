@echo off
echo ${def} SCHAT_VERSION "%SCHAT_VERSION%.%SCHAT_REVISION%" > "engine\version.nsh"
echo #define SCHAT_REVISION %SCHAT_REVISION% > "..\..\src\common\revision.h"