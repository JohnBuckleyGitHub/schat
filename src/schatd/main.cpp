/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>

#include "server.h"

int main(int argc, char *argv[])
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    
  QCoreApplication app(argc, argv);
  Server server;
  
  if (!server.listen(QHostAddress::Any, 7666)) {
    qDebug() << server.errorString();
    return 1;
  }

//  QPushButton quitButton(QObject::tr("&Quit"));
//  quitButton.setWindowTitle(QObject::tr("Trip Server"));
//  QObject::connect(&quitButton, SIGNAL(clicked()), &app, SLOT(quit()));
////  quitButton.setStyle(new QPlastiqueStyle);
//  quitButton.show();
  return app.exec();
}
