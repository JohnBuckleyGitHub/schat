/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
//#include "server.h"
#include "singleapplication.h"
#include "version.h"

bool install();

int main(int argc, char *argv[])
{  
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  
  QApplication app(argc, argv);
  app.setStyle(new QPlastiqueStyle);
  
  if (install())
    return 0;
  else
    QFile::remove(qApp->applicationDirPath() + "/install.exe");
  
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
  
  // Допускаем запуск только одной копии из одной папки
  SingleApplication instance("SimpleChat", app.applicationDirPath(), &app);
  if (instance.isRunning()) {
    QString message = "SimpleChat";
    if (instance.sendMessage(message))
      return 0;
  }
  
  SChatWindow window;
  if (arguments.contains("-hide"))
    window.hide();
  else
    window.show();
  
  QObject::connect(&instance, SIGNAL(messageReceived(const QString &)), &window, SLOT(handleMessage(const QString &)));
  
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
    else if (QFile::exists(appPath + "/install.exe")) {
      QStringList args;
      args << "-install" << "-run";
      if (s.value("AutoClean", true).toBool())
        args << "-clean";
      
      QProcess::startDetached('"' + appPath + "/install.exe\"", args, appPath);
      return true;
    }    
  }
  
  return false;
}
