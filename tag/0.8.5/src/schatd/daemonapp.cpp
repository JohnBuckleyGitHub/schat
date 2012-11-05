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

#include "daemon.h"
#include "daemonapp.h"

/*!
 * Конструктор класса DaemonApp.
 */
DaemonApp::DaemonApp(int argc, char **argv)
  : QtService<QCoreApplication>(argc, argv, "Simple Chat Daemon")
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  setServiceDescription("IMPOMEZIA Simple Chat Daemon");
  setStartupType(QtServiceController::AutoStartup);
}


void DaemonApp::processCommand(int code)
{
  if (m_daemon)
    m_daemon->reload(code);
}


/*!
 * Запуск сервера.
 */
void DaemonApp::start()
{
  QCoreApplication *app = application();
  m_daemon = new Daemon(app);

  if (!m_daemon->start()) {
    logMessage("Failed to start Simple Chat Daemon, see logs", QtServiceBase::Error);
    app->exit(3);
  }
}


/*!
 * Остановка сервиса.
 */
void DaemonApp::stop()
{
  application()->quit();
}
