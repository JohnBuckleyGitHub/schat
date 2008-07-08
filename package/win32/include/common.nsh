/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

!define SCHAT_NAME       "IMPOMEZIA Simple Chat"
!define SCHAT_NAME_SHORT "Simple Chat"
!define SCHAT_VERSION    "0.0.5.197"
!define SCHAT_REGKEY     "Software\IMPOMEZIA\${SCHAT_NAME_SHORT}"
!define SCHAT_QTDIR      "C:\qt\440"
!define SCHAT_WEB_SITE   "http://impomezia.net.ru/"
!define SCHAT_COPYRIGHT  "Copyright © 2008 IMPOMEZIA"
!define SCHAT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SCHAT_NAME_SHORT}"
!define SCHAT_INSTALLDIR "$LOCALAPPDATA\IMPOMEZIA\${SCHAT_NAME_SHORT}"
!define VC90
!ifdef VC90
  !define VC90_REDIST_DIR "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT"
!endif

!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_FINISHPAGE_LINK            "${SCHAT_WEB_SITE}"
!define MUI_FINISHPAGE_LINK_LOCATION   "${SCHAT_WEB_SITE}"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP         "contrib\header.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_ICON                       "contrib\install.ico"
!define MUI_UNICON                     "contrib\uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP   "contrib\wizard.bmp"
!define MUI_LANGDLL_REGISTRY_ROOT      "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY       "${SCHAT_REGKEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "language"
!define MUI_LANGDLL_ALWAYSSHOW
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TITLE_3LINES