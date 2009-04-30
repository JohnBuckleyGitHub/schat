/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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
#include "version.h"

/*!
 * Конструктор класса SimpleChatApp.
 */
SimpleChatApp::SimpleChatApp(int &argc, char **argv)
  : QtSingleApplication("SimpleChat", argc, argv),
  m_window(0)
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  setApplicationName(SCHAT_NAME);
  setApplicationVersion(SCHAT_VERSION);
  setOrganizationName(SCHAT_ORGANIZATION);
  setOrganizationDomain(SCHAT_DOMAIN);
  setQuitOnLastWindowClosed(false);
  addLibraryPath(applicationDirPath() + "/plugins");

  #ifndef Q_OS_WINCE
    setStyle(new QPlastiqueStyle);
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
  QTranslator qtTranslator;
  qtTranslator.load("qt_ru", ":/translations");
  installTranslator(&qtTranslator);

  QTranslator translator;
  translator.load("schat_ru", ":/translations");
  installTranslator(&translator);

  m_window = new SChatWindow;
  QStringList args = arguments();
  args.removeFirst();

  if (args.contains("-hide"))
    m_window->hide();
  else
    m_window->show();

  #ifndef SCHAT_NO_SINGLEAPP
  connect(this, SIGNAL(messageReceived(const QString &)), m_window, SLOT(handleMessage(const QString &)));
  #endif
  return exec();
}


SimpleChatApp *SimpleChatApp::instance()
{
  return (static_cast<SimpleChatApp *>(QCoreApplication::instance()));
}
