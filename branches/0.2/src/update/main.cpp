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

/**
 * КОДЫ ВОЗВРВТА
 * 0   => Обновления успешно скачаны/проверены.
 * 100 => Уведомление об отсутствии необходимости обновления (источник `Update::createQueue(const QString &)`).
 * 400 => Не удалось создать папку для обновлений (Критическая ошибка, источник `Update::execute()`).
 * 401 => Не удалось прочитать "update.xml" (Критическая ошибка, источник `Update::createQueue(const QString &)`).
 * 403 => Ошибка проверки скачаного файла (Критическая ошибка, источник `Update::verifyFile()`).
 */

#include <QtCore>

#include "update.h"

int main(int argc, char **argv)
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  
  QCoreApplication app(argc, argv);
  QStringList arguments = app.arguments();
  arguments.takeFirst();
  
  Update *update = 0;
  
  if (arguments.contains("-get")) {
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
