/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

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
  needCreateNetworkList = true;
}


/** [public]
 * 
 */
void Settings::notify(int notify)
{
  switch (notify) {
    case ServerChanged:
      emit serverChanged();
      break;
      
    default:
      break;
  }  
}


/** [public]
 * 
 */
void Settings::notify(int notify, int index)
{
  switch (notify) {
    case NetworksModelIndexChanged:
      emit networksModelIndexChanged(index);
      break;
      
    default:
      break;
  }  
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
  
  chat->restoreSplitter((s.value("Splitter").toByteArray()));        // Состояние сплитера
  hideWelcome = s.value("HideWelcome", false).toBool();              // Показ окна для выбора ника и сервера при старте
  firstRun    = s.value("FirstRun", true).toBool();                  // Первый запуск
  style       = s.value("Style", "Plastique").toString();            // Внешний вид
  network.fromConfig(s.value("Network", "AchimNet.xml").toString());
  qApp->setStyle(style);
  
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
  s.setValue("Style", style);
  s.setValue("Network", network.config());
  saveRecentServers(s);
  
  s.beginGroup("Profile");
  s.setValue("Nick", profile->nick());
  s.setValue("Name", profile->fullName());
  s.setValue("Sex", profile->sex());
}


/** [private]
 * Функция проходится по модели `networksModel` и выделяет из неё одиночные серверы,
 * для записи в файл настроек (ключ "RecentServers").
 */
void Settings::saveRecentServers(QSettings &s)
{
  QStringList list;
  
  for (int row = 0; row < networksModel.rowCount(); ++row) {
    QStandardItem *item = networksModel.item(row);
    if (item->data(Qt::UserRole).type() == QVariant::Int)
      list << (item->text() + ':' + item->data(Qt::UserRole).toString());
  }
  
  if (list.isEmpty())
    list << "empty";
  
  s.setValue("RecentServers", list);
}

