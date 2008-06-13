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
 * 403 => Ошибка проверки скачаного файла (Критическая ошибка, источник `Update::verifyFile()`).
 */

#include <QtGui>

#include "install.h"
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
  Install *install;
  
  if (arguments.contains("-install")) {
    QSplashScreen *splash = new QSplashScreen(QPixmap(":/images/splash.png"));
    splash->show();
    splash->showMessage(QObject::tr("Идёт обновление..."), Qt::AlignRight | Qt::AlignBottom, Qt::white);
    install = new Install;
    QTimer::singleShot(0, install, SLOT(execute()));
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
      
  return app.exec();
}
