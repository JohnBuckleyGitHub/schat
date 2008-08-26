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

#include <QtGui>

#include "abstractprofile.h"
#include "icondefreader.h"
#include "networkreader.h"
#include "schatwindow.h"
#include "settings.h"
#include "version.h"

/*!
 * \class Settings
 * \brief Класс читает и записывает настройки клиента.
 */

/*!
 * \brief Конструктор класса Settings.
 */
Settings::Settings(const QString &filename, AbstractProfile *profile, QObject *parent)
  : AbstractSettings(filename, parent), m_profile(profile)
{
  needCreateNetworkList = true;
}


QString Settings::smileFile(const QString &smile)
{
  if (m_emoticons.isEmpty() || m_emoticonsFiles.isEmpty())
    return "";

  if (m_emoticons.contains(smile)) {
    int index = m_emoticons.value(smile);
    if (index < m_emoticonsFiles.size()) {
      QString file = m_emoticonsPath + "/" + m_emoticonsFiles.at(index);
      if (!QFile::exists(file))
        return "";

      return file;
    }
    else
      return "";
  }

  return "";
}


/*!
 * \brief Возвращает список всех смайликов которые были найдены в строке.
 */
QStringList Settings::smiles(const QString &text) const
{
  QStringList out;
  
  if (!m_emoticons.isEmpty()) {
    QMapIterator <QString, int> i(m_emoticons);
    while (i.hasNext()) {
      i.next();
      QString key = i.key();
  
      if (text.contains(key))
        out << key;
    }
  }

  return out;
}


/*!
 * \brief Создаёт карту смайликов.
 */
void Settings::createEmoticonsMap()
{
  m_emoticonsPath = qApp->applicationDirPath() + "/emoticons/" + getString("EmoticonTheme");
  bool err = true;

  if (QFile::exists(m_emoticonsPath + "/icondef.xml")) {
    IconDefReader reader(&m_emoticons, &m_emoticonsFiles);
    if (reader.readFile(m_emoticonsPath + "/icondef.xml"))
      err = false;
  }

  if (err) {
    m_emoticons.clear();
    m_emoticonsFiles.clear();
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

  m_pos  = m_settings->value("Pos", QPoint(-999, -999)).toPoint();
  m_size = m_settings->value("Size", QSize(680, 460)).toSize();
  m_splitter = m_settings->value("Splitter").toByteArray();

  readBool("HideWelcome", false);
  readBool("FirstRun", true);
  readBool("EmoticonsRequireSpaces", true);
  readInt("AnimatedEmoticonsInMessage", 2);
  readString("Style", "Plastique");
  readString("EmoticonTheme", "kolobok");
  qApp->setStyle(getString("Style"));

  network.fromConfig(m_settings->value("Network", "AchimNet.xml").toString());
  createServerList();

  m_settings->beginGroup("Profile");
  m_profile->setNick(m_settings->value("Nick", QDir::home().dirName()).toString());
  m_profile->setFullName(m_settings->value("Name", "").toString());
  m_profile->setGender(m_settings->value("Gender", "male").toString());
  m_profile->setByeMsg(m_settings->value("Bye", "IMPOMEZIA Simple Chat").toString());
  m_settings->endGroup();

  createEmoticonsMap();

#ifdef SCHAT_UPDATE
  int interval = m_settings->value("Updates/CheckInterval", 60).toInt();
  if (interval < 5)
    interval = 5;
  else if (interval > 1440)
    interval = 1440;

  setInt("Updates/CheckInterval", interval);
  readBool("Updates/AutoClean", true);
  readBool("Updates/AutoDownload", true);
  readString("Updates/Url", "http://192.168.5.1/schat/updates/update.xml");
#endif
}


/** [public]
 * 
 */
void Settings::write()
{
  QSettings s(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);

  m_settings->setValue("Size", m_size);
  m_settings->setValue("Pos", m_pos);
  m_settings->setValue("Splitter", m_splitter);
  m_settings->setValue("FirstRun", false);

  writeBool("HideWelcome");
  writeBool("EmoticonsRequireSpaces");
  writeInt("AnimatedEmoticonsInMessage");
  writeString("Style");
  writeString("EmoticonsTheme");

  m_settings->setValue("Network", network.config());
  saveRecentServers();

  m_settings->beginGroup("Profile");
  m_settings->setValue("Nick", m_profile->nick());
  m_settings->setValue("Name", m_profile->fullName());
  m_settings->setValue("Gender", m_profile->gender());
  m_settings->setValue("Bye", m_profile->byeMsg());
  m_settings->endGroup();
  
#ifdef SCHAT_UPDATE
  writeBool("Updates/AutoClean");
  writeInt("Updates/CheckInterval");
  s.beginGroup("Updates");
//  s.setValue("CheckInterval", updateCheckInterval);
//  s.setValue("AutoClean", updateAutoClean);
  s.endGroup();
#endif
}


/** [private]
 * Создаёт список сетей и одиночных серверов. 
 */
void Settings::createServerList()
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

  QStringList recent = m_settings->value("RecentServers", "empty").toStringList();

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
void Settings::saveRecentServers()
{
  QStringList list;

  for (int row = 0; row < networksModel.rowCount(); ++row) {
    QStandardItem *item = networksModel.item(row);
    if (item->data(Qt::UserRole).type() == QVariant::Int)
      list << (item->text() + ':' + item->data(Qt::UserRole).toString());
  }

  if (list.isEmpty())
    list << "empty";

  m_settings->setValue("RecentServers", list);
}
