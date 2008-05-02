/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>
#include <unistd.h>

#include "server.h"

int main(int argc, char *argv[])
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    
  QCoreApplication app(argc, argv);
  Server server;
  
  if (!server.start())
    return 3;
  
  // Создаём PID-файл
  QFile pidfile(QCoreApplication::instance()->applicationDirPath() + "/schatd.pid");
  if (pidfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&pidfile);
    out << getpid();
    pidfile.close();
  }

  return app.exec();
}
