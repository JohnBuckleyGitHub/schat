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

#include <QtGui>
#include <QtNetwork>

#include "schatwindow.h"
#include "simplechatapp.h"
#include "translation.h"
#include "version.h"

#ifndef SCHAT_NO_SINGLEAPP
 #define SCHAT_PREFFIX "SimpleChat",
#else
 #define SCHAT_PREFFIX
#endif

/*!
 * Конструктор класса SimpleChatApp.
 */
SimpleChatApp::SimpleChatApp(int &argc, char **argv)
  : QtSingleApplication(SCHAT_PREFFIX argc, argv),
  m_window(0),
  m_translation(0)
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  setApplicationName(SCHAT_NAME);
  setApplicationVersion(SCHAT_VERSION);
  setOrganizationName(SCHAT_ORGANIZATION);
  setOrganizationDomain(SCHAT_DOMAIN);
  setQuitOnLastWindowClosed(false);
  addLibraryPath(applicationDirPath() + "/plugins");

  QPalette palette = this->palette();
  palette.setColor(QPalette::Inactive, QPalette::Highlight, palette.color(QPalette::Highlight));
  setPalette(palette);

  #if defined(Q_WS_X11)
  setAttribute(Qt::AA_DontShowIconsInMenus, false);
  #endif
}


SimpleChatApp::~SimpleChatApp()
{
  if (m_window)
    delete m_window;
}


/*!
 * Returns true if another instance of this application is running; otherwise false.
 */
bool SimpleChatApp::isRunning()
{
  #if defined(SCHAT_NO_SINGLEAPP) || defined(SCHAT_DEVEL_MODE)
  return false;
  #else
  QStringList args = arguments();
  args.removeFirst();

  if (args.isEmpty())
    return sendMessage("");

  QString message = args.join(", ");

  if (args.contains("-exit")) {
    sendMessage(message);
    return true;
  }
  else
    return sendMessage(message);
  #endif
}


/*!
 * Загрузка переводов, создание главного окна и запуск цикла событий.
 */
int SimpleChatApp::run()
{
  m_window = new SChatWindow;
  QStringList args = arguments();
  args.removeFirst();

  if (args.contains("-hide"))
    m_window->hide();
  else
    m_window->showChat();

  #ifndef SCHAT_NO_SINGLEAPP
  connect(this, SIGNAL(messageReceived(const QString &)), m_window, SLOT(handleMessage(const QString &)));
  #endif
  return exec();
}


QIcon SimpleChatApp::iconFromTheme(const QString &name)
{
  #if defined(Q_WS_X11) && QT_VERSION >= 0x040600
  return QIcon::fromTheme(name, QIcon(":/images/" + name + ".png"));
  #else
  return QIcon(":/images/"+ name + ".png");
  #endif
}


SimpleChatApp *SimpleChatApp::instance()
{
  return (static_cast<SimpleChatApp *>(QCoreApplication::instance()));
}


Translation* SimpleChatApp::translation()
{
  if (!m_translation)
    m_translation = new Translation(this);

  return m_translation;
}



#if !defined(SCHAT_NO_STYLE)
QString SimpleChatApp::defaultStyle()
{
  QStringList styles = QStyleFactory::keys();
  Q_UNUSED(styles);

  #if defined(Q_WS_X11)
  if (styles.contains("Oxygen"))
    return "Oxygen";

  if (styles.contains("GTK+"))
    return "GTK+";
  #endif

  #if defined(Q_WS_WIN)
  if (styles.contains("WindowsVista"))
    return "WindowsVista";
  #endif

  return "Plastique";
}
#endif
