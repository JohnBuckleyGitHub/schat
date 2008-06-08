/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

/**
 * КОДЫ ВОЗВРВТА
 * 0   => Обновления успешно скачаны/проверены.
 * 100 => Уведомление об отсутствии необходимости обновления (источник `Update::createQueue(const QString &)`).
 * 400 => Не удалось создать папку для обновлений (Критическая ошибка, источник `Update::execute()`).
 * 401 => Не удалось прочитать "update.xml" (Критическая ошибка, источник `Update::createQueue(const QString &)`).
 * 402 => Не удалось создать копию с именем "schat-install.exe" (Критическая ошибка, источник `Update::finished()`).
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
