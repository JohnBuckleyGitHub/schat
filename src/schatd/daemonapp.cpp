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

#include <QtCore>

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


/*!
 * Запуск сервера.
 */
void DaemonApp::start()
{
  QCoreApplication *app = application();
  QTranslator *translator = new QTranslator(app);
  translator->load("schatd_ru", ":/translations");
  app->installTranslator(translator);

  Daemon *daemon = new Daemon(app);

  if (!daemon->start()) {
    logMessage("Failed to start Simple Chat Daemon, see logs", QtServiceBase::Error);
    app->exit(3);
  }

  QFile pidfile(app->applicationDirPath() + "/schatd.pid");
  if (pidfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&pidfile);
    out << app->applicationPid();
    pidfile.close();
  }
}


/*!
 * Остановка сервиса.
 */
void DaemonApp::stop()
{
  QFile::remove(application()->applicationDirPath() + "/schatd.pid");
  application()->quit();
}
