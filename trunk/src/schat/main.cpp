/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "schatwindow.h"
#include "server.h"

int main(int argc, char *argv[])
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  
  QApplication app(argc, argv);
  app.setStyle(new QPlastiqueStyle);
  SChatWindow window;
  window.show();
  
  Server *server = new Server;
  if (!server->listen(QHostAddress::Any, 7666)) {
    delete server;
  }
  
  return app.exec();
}
