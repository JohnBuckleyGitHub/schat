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

#include "schatwindow.h"
#include "version.h"

#ifdef SCHAT_STATIC
  Q_IMPORT_PLUGIN(qgif)
#endif

#ifndef DISABLE_SINGLE_APP
  #include "singleapplication.h"
#endif

bool install();

int main(int argc, char *argv[])
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  QApplication app(argc, argv);
  QCoreApplication::addLibraryPath(app.applicationDirPath() + "/plugins");

  app.setStyle(new QPlastiqueStyle);

  if (install())
    return 0;

  QStringList arguments = app.arguments();
  arguments.takeFirst();

  // Требуем поддержку System Tray
  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("I couldn't detect any system tray on this system."));
    return 1;
  }

  #ifndef DISABLE_SINGLE_APP
    QString serverName = QString(QCryptographicHash::hash(app.applicationDirPath().toUtf8(), QCryptographicHash::Md5).toHex());
    SingleApplication instance("SimpleChat", serverName, &app);
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

  QTranslator qtTranslator;
  qtTranslator.load("qt_ru", ":/translations");
  app.installTranslator(&qtTranslator);

  SChatWindow window;
  if (arguments.contains("-hide"))
    window.hide();
  else
    window.show();

  #ifndef DISABLE_SINGLE_APP
    QObject::connect(&instance, SIGNAL(messageReceived(const QString &)), &window, SLOT(handleMessage(const QString &)));
  #endif

  return app.exec();
}


bool install()
{
  QString appPath = qApp->applicationDirPath();
  QSettings s(appPath + "/schat.conf", QSettings::IniFormat);
  s.beginGroup("Updates");

  if (s.value("ReadyToInstall", false).toBool()) {
    int qtLevel   = s.value("LastDownloadedQtLevel", 0).toInt();
    int coreLevel = s.value("LastDownloadedCoreLevel", 0).toInt();

    if (qtLevel <= UpdateLevelQt && coreLevel <= UpdateLevelCore)
      return false;
    else if (QFile::exists(appPath + "/uninstall.exe")) {
      QStringList args;
      args << "-update" << "-run";
      if (s.value("AutoClean", true).toBool())
        args << "-clean";

      QProcess::startDetached('"' + appPath + "/uninstall.exe\"", args, appPath);
      return true;
    }
  }

  return false;
}
