/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "abstractprofile.h"
#include "networkreader.h"
#include "schatwindow.h"
#include "settings.h"
#include "version.h"


/** [public]
 * 
 */
Settings::Settings(AbstractProfile *p, QObject *parent)
  : QObject(parent)
{
  m_profile = p;
  chat    = static_cast<SChatWindow *>(parent);
  needCreateNetworkList = true;
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
  QPoint pos = s.value("Pos", QPoint(-999, -999)).toPoint();            // Позиция окна
  QSize size = s.value("Size", QSize(680, 460)).toSize();               // Размер окна
  chat->resize(size);
  if (pos.x() != -999 && pos.y() != -999)
    chat->move(pos);
  
  chat->restoreSplitter((s.value("Splitter").toByteArray()));           // Состояние сплитера
  hideWelcome = s.value("HideWelcome", false).toBool();                 // Показ окна для выбора ника и сервера при старте
  firstRun    = s.value("FirstRun", true).toBool();                     // Первый запуск
  style       = s.value("Style", "Plastique").toString();               // Внешний вид
  qApp->setStyle(style);
  network.fromConfig(s.value("Network", "AchimNet.xml").toString());    // Файл сети, или адрес одиночного сервера
  
  createServerList(s);
  
  s.beginGroup("Profile");
  m_profile->setNick(s.value("Nick", QDir::home().dirName()).toString()); // Ник
  m_profile->setFullName(s.value("Name", "").toString());                 // Настоящие имя
  m_profile->setGender(s.value("Gender", "male").toString());             // Пол
  m_profile->setByeMsg(s.value("Bye", "IMPOMEZIA Simple Chat").toString());
  s.endGroup();
  
  #ifdef SCHAT_UPDATE
  s.beginGroup("Updates");
  updateCheckInterval = s.value("CheckInterval", 60).toInt();           // Интервал проверки обновлений (от 5 до 1440 (1 сутки) минут)
  if (updateCheckInterval < 5)
    updateCheckInterval = 5;
  if (updateCheckInterval > 1440)
    updateCheckInterval = 1440;
  
  updateAutoClean = s.value("AutoClean", true).toBool();                // Автоматически удалять обновления
  updateAutoDownload = s.value("AutoDownload", true).toBool();          // Автоматически скачивать обновления
  updateUrl = s.value("Url", "http://192.168.5.130/schat/updates/update.xml").toString();
  #endif
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
  s.setValue("Nick", m_profile->nick());
  s.setValue("Name", m_profile->fullName());
  s.setValue("Gender", m_profile->gender());
  s.setValue("Bye", m_profile->byeMsg());
  s.endGroup();
  
  #ifdef SCHAT_UPDATE
  s.beginGroup("Updates");
  s.setValue("CheckInterval", updateCheckInterval);
  s.setValue("AutoClean", updateAutoClean);
  s.endGroup();
  #endif
}


/** [private]
 * Создаёт список сетей и одиночных серверов. 
 */
void Settings::createServerList(QSettings &s)
{
  QDir directory(qApp->applicationDirPath() + "/networks/");
  directory.setNameFilters(QStringList() << "*.xml");
  QStringList files = directory.entryList(QDir::Files | QDir::NoSymLinks);
  NetworkReader network;
  
  foreach (QString file, files)
    if (network.readFile(qApp->applicationDirPath() + "/networks/" + file)) {
      QStandardItem *item = new QStandardItem(QIcon(":/images/network.png"), network.networkName());
      item->setData(file, Qt::UserRole);
      networksModel.appendRow(item);
    }
  
  QStringList recent = s.value("RecentServers", "empty").toStringList();
  
  if (recent.at(0) != "empty")
    foreach (QString server, recent) {
      QStringList list = server.split(':');
      if (list.size() == 2) {
        QStandardItem *item = new QStandardItem(QIcon(":/images/host.png"), list.at(0));
        item->setData(list.at(1).toInt(), Qt::UserRole);
        networksModel.appendRow(item);
      }
    }
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
