/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#if defined(Q_WS_WIN)
# include <QFileInfo>
# include <QProcess>
#endif

#include "ChatApp.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "Path.h"
#include "sglobal.h"
#include "ui/ChatWindow.h"
#include "version.h"

ChatApp::ChatApp(int &argc, char **argv)
  : QApplication(argc, argv)
  , m_core(0)
  , m_window(0)
{
  setApplicationName(SCHAT_NAME);
  setApplicationVersion(SCHAT_VERSION);
  setOrganizationName(SCHAT_ORGANIZATION);
  setOrganizationDomain(SCHAT_DOMAIN);
  setQuitOnLastWindowClosed(false);

# if defined(Q_WS_X11)
  setAttribute(Qt::AA_DontShowIconsInMenus, false);
# endif

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  Path::init();

  QPalette palette = this->palette();
  palette.setColor(QPalette::Inactive, QPalette::Highlight, palette.color(QPalette::Highlight));
  setPalette(palette);
}


ChatApp::~ChatApp()
{
  if (m_window)
    delete m_window;
}


void ChatApp::start()
{
  m_core = new ChatCore(this);
  if (!ChatCore::settings()->value("Labs/DisableUI").toBool()) {
    m_window = new ChatWindow();
    connect(m_window, SIGNAL(restartRequest()), SLOT(restart()));
    m_window->showChat();
  }
}


void ChatApp::stop()
{
  if (m_window)
    delete m_window;

  if (m_core)
    delete m_core;

  m_core = 0;
  m_window = 0;
}


#if defined(Q_OS_WIN)
bool ChatApp::selfUpdate()
{
  if (!SCHAT_REVISION)
    return false;

  if (QFileInfo(applicationFilePath()).baseName() != LS("schat2"))
    return false;

  QString appPath = QApplication::applicationDirPath();
  QSettings s(appPath + LS("/schat2.conf"), QSettings::IniFormat);
  s.setIniCodec("UTF-8");
  s.beginGroup(LS("Update"));
  if (s.value(LS("Ready"), false) != true)
    return false;

  s.setValue(LS("Ready"), false);
  QString version = s.value(LS("Version"), QString()).toString();
  if (version.isEmpty())
    return false;

  int revision = s.value(LS("Revision"), 0).toInt();
  if (revision < 1)
    return false;

  QString file = appPath + LS("/.schat2/schat2-") + version + LS(".") + QString::number(revision) + LS(".exe");
  if (!QFile::exists(file))
    return false;

  QProcess::startDetached(LC('"') + file + LC('"'), QStringList(LS("-update")), appPath);
  return true;
}
#endif


void ChatApp::restart()
{
  stop();

# if defined(Q_OS_WIN)
  if (selfUpdate()) {
    quit();
    return;
  }
# endif

  start();
}
