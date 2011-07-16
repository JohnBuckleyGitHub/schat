/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QTextCodec>

#include "ChatApp.h"
#include "ChatCore.h"
#include "version.h"
#include "ui/ChatWindow.h"


ChatApp::ChatApp(int &argc, char **argv)
  : QApplication(argc, argv)
{
  setApplicationName(SCHAT_NAME);
  setApplicationVersion(SCHAT_VERSION);
  setOrganizationName(SCHAT_ORGANIZATION);
  setOrganizationDomain(SCHAT_DOMAIN);
  setQuitOnLastWindowClosed(false);
  addLibraryPath(applicationDirPath() + "/plugins/qt");
  addLibraryPath(applicationDirPath() + "/plugins/client");

  #if defined(Q_WS_X11)
  setAttribute(Qt::AA_DontShowIconsInMenus, false);
  #endif

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  m_core = new ChatCore(this);

  m_window = new ChatWindow();
  m_window->showChat();
}


ChatApp::~ChatApp()
{
  delete m_window;
}
