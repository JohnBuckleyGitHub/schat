/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>

#include "settings.h"
#include "schatwindow.h"


/** [public]
 * 
 */
Settings::Settings(Profile *p, QObject *parent)
  : QObject(parent)
{
  profile = p;
  chat    = static_cast<SChatWindow *>(parent);
}


/** [public]
 * 
 */
void Settings::read()
{
  QSettings s(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);
  
  // Позиция и размер окна
  QPoint pos = s.value("Pos", QPoint(-999, -999)).toPoint();
  QSize size = s.value("Size", QSize(680, 460)).toSize();
  chat->resize(size);
  if (pos.x() != -999 && pos.y() != -999)
    chat->move(pos);
  
  chat->restoreSplitter((s.value("Splitter").toByteArray()));  // Состояние сплитера
  hideWelcome = s.value("HideWelcome", false).toBool();        // Показ окна для выбора ника и сервера при старте
  firstRun    = s.value("FirstRun", true).toBool();            // Первый запуск
  server      = s.value("Server", "192.168.5.130").toString(); // Адрес сервера
  serverPort  = quint16(s.value("ServerPort", 7666).toUInt()); // Порт сервера
  
  s.beginGroup("Profile");
  profile->setNick(s.value("Nick", QDir::home().dirName()).toString()); // Ник
  profile->setFullName(s.value("Name", "").toString());                 // Настоящие имя
  profile->setSex(quint8(s.value("Sex", 0).toUInt()));                  // Пол
}


/** [public]
 * 
 */
void Settings::write()
{
  QSettings s(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);
  
  s.setValue("Size", chat->size());
  s.setValue("Pos", chat->pos());
  s.setValue("Splitter", chat->saveSplitter());
  s.setValue("HideWelcome", hideWelcome);
  s.setValue("FirstRun", false);
  s.setValue("Server", server);
  s.setValue("ServerPort", serverPort);
  
  s.beginGroup("Profile");
  s.setValue("Nick", profile->nick());
  s.setValue("Name", profile->fullName());
  s.setValue("Sex", profile->sex());
}
