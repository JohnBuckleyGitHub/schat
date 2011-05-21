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

#ifdef SCHAT_NO_SINGLEAPP
 #define QtSingleApplication QApplication
 #include <QApplication>
#else
 #include <QtSingleApplication>
#endif

#include <QMessageBox>
#include <QPlastiqueStyle>
#include <QSystemTrayIcon>
#include <QTextCodec>
#include <QTranslator>

#include "daemonui.h"

int main(int argc, char *argv[])
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  QtSingleApplication app(argc, argv);
  app.setApplicationName("IMPOMEZIA Simple Chat Daemon UI");
  app.setQuitOnLastWindowClosed(false);
  app.addLibraryPath(app.applicationDirPath() + "/plugins");

  QStringList args = app.arguments();
  args.removeFirst();

  #ifndef SCHAT_NO_SINGLEAPP
  #ifndef SCHAT_DEVEL_MODE
  if (args.isEmpty() && app.sendMessage(""))
    return 0;

  QString message = args.join(", ");

  if (args.contains("-exit")) {
    app.sendMessage(message);
    return 0;
  }
  if (app.sendMessage(message))
    return 0;
  #endif
  #endif

  DaemonUi ui;
  if (args.contains("-show"))
    ui.show();
  else
    ui.hide();

  #ifndef SCHAT_NO_SINGLEAPP
  QObject::connect(&app, SIGNAL(messageReceived(const QString &)), &ui, SLOT(handleMessage(const QString &)));
  #endif

  return app.exec();
}
