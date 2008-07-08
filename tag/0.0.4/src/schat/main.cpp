/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "schatwindow.h"
#include "server.h"
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
    QFile::remove(qApp->applicationDirPath() + "/schat-install.exe");
  
  QStringList arguments = app.arguments();
  arguments.takeFirst();
  
  QTranslator qtTranslator;
  qtTranslator.load("qt_ru", ":/translations");
  app.installTranslator(&qtTranslator);
  
  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("I couldn't detect any system tray on this system."));
    return 1;
  }
  
  SChatWindow window;
  if (arguments.contains("-hide"))
    window.hide();
  else
    window.show();
  
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
    else if (QFile::exists(appPath + "/schat-install.exe")) {
      QStringList args;
      args << "-install" << "-run";
      if (s.value("AutoClean", true).toBool())
        args << "-clean";
      
      QProcess::startDetached('"' + appPath + "/schat-install.exe\"", args, appPath);
      return true;
    }    
  }
  
  return false;
}
