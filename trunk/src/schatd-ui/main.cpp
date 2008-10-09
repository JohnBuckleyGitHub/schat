/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>

#include "daemonui.h"

#ifndef SCHAT_NO_SINGLE_APP
  #include "singleapplication.h"
#endif

int main(int argc, char *argv[])
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);
  QCoreApplication::addLibraryPath(app.applicationDirPath() + "/plugins");

  app.setStyle(new QPlastiqueStyle);

  QStringList arguments = app.arguments();
  arguments.takeFirst();

  QTranslator qtTranslator;
  qtTranslator.load("qt_ru", ":/translations");
  app.installTranslator(&qtTranslator);

  // Требуем поддержку System Tray
  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("I couldn't detect any system tray on this system."));
    return 1;
  }

  #ifndef SCHAT_NO_SINGLE_APP
    QString serverName = app.applicationDirPath().toUtf8().toHex();
    SingleApplication instance("SimpleChatDaemonUI", serverName, &app);
    if (instance.isRunning()) {
      QString message;

      if (!arguments.isEmpty())
        message = arguments.join(", ");

      if (instance.sendMessage(message))
        return 0;
    }
  #endif

  if (arguments.contains("-exit"))
    return 0;

  DaemonUi ui;
  if (arguments.contains("-show"))
    ui.show();
  else
    ui.hide();

  #ifndef SCHAT_NO_SINGLE_APP
    QObject::connect(&instance, SIGNAL(messageReceived(const QString &)), &ui, SLOT(handleMessage(const QString &)));
  #endif

  return app.exec();
}
