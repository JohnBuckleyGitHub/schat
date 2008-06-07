/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

/**
 * КОДЫ ВОЗВРВТА
 * 0 => Обновления успешно скачаны/проверены.
 */

#include <QtGui>

#include "update.h"

int main(int argc, char **argv)
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  
  QApplication app(argc, argv);
  app.setStyle(new QPlastiqueStyle);
  QStringList arguments = app.arguments();
  arguments.takeFirst();
  
  Update *update = 0;
  
  if (arguments.contains("-install")) {
    QMessageBox::information(0, "INSTALL", "INSTALL");
  }
  else if (arguments.contains("-get")) {
    QUrl url;
    if (arguments.size() == 2)
      url = QUrl::fromEncoded(arguments.at(1).toLocal8Bit());
    else
      url = DefaultUpdateXmlUrl;
    
    update = new Update(url);
    QTimer::singleShot(0, update, SLOT(execute()));
  }
  else
    return 255;    
      
  app.exec();
}
