/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QtGui>

#include "abstractprofile.h"
#include "emoticonsreader.h"
#include "icondefreader.h"
#include "networkreader.h"
#include "schatwindow.h"
#include "settings.h"
#include "version.h"

/*!
 * \brief Конструктор класса Settings.
 */
Settings::Settings(const QString &filename, QObject *parent)
  : AbstractSettings(filename, parent)
{
  m_profile = new AbstractProfile(this);
  m_updateTimer = new QTimer(this);

  QString defaultConf = qApp->applicationDirPath() + "/default.conf";
  if (QFile::exists(defaultConf))
    m_default = new QSettings(defaultConf, QSettings::IniFormat, this);
  else
    m_default = 0;
}


/*!
 * \brief Возвращает список всех смайликов которые были найдены в строке.
 */
QList<Emoticons> Settings::emoticons(const QString &text) const
{
  QList<Emoticons> out;

  if (!m_emoticons.isEmpty() && !text.isEmpty()) {
    QMapIterator <QString, QStringList> i(m_emoticons);
    while (i.hasNext()) {
      i.next();
      QStringList list = i.value();
      foreach (QString name, list) {
        if (text.contains(name)) {
          out << Emoticons(name, i.key());
        }
      }
    }
  }

  return out;
}


QStandardItem* Settings::findItem(const QStandardItemModel *model, const QString &text, Qt::MatchFlags flags, int column)
{
  QList<QStandardItem *> items;

  items = model->findItems(text, flags, column);
  if (items.size() > 0)
    return items[0];
  else
    return 0;
}


/*!
 * \brief Создаёт карту смайликов.
 */
void Settings::createEmoticonsMap()
{
  QString emoticonsPath = qApp->applicationDirPath() + "/emoticons/" + getString("EmoticonTheme");
  bool err = true;

  if (QFile::exists(emoticonsPath + "/icondef.xml")) {
    IconDefReader reader(&m_emoticons);
    if (reader.readFile(emoticonsPath + "/icondef.xml")) {
      err = false;
      if (reader.refresh())
        setInt("EmoticonsRefreshTime", reader.refresh());
      else
        setInt("EmoticonsRefreshTime", 50);
    }
  }
  else if (QFile::exists(emoticonsPath + "/emoticons.xml")) {
    EmoticonsReader reader(&m_emoticons);
    if (reader.readFile(emoticonsPath + "/emoticons.xml"))
      err = false;
  }

  if (err) {
    m_emoticons.clear();
    setBool("UseEmoticons", false);
  }
}


void Settings::notify(int notify)
{
  bool readyToInstall = false;

  switch (notify) {
    case UpdateError:
    case UpdateAvailable:
    case UpdateNoAvailable:
    #ifndef SCHAT_NO_UPDATE
      case UpdateReady:
    #endif
      if (m_update)
        m_update->deleteLater();

      #ifndef SCHAT_NO_UPDATE
        if (notify == UpdateReady)
          readyToInstall = true;
      #endif

      setBool("Updates/ReadyToInstall", readyToInstall);
      break;

    case UpdateSettingsChanged:
      if (getBool("Updates/Enable")) {
        m_updateTimer->setInterval(getInt("Updates/CheckInterval") * 60 * 1000);
        if (!m_updateTimer->isActive())
          m_updateTimer->start();
      }
      else if (m_updateTimer->isActive())
        m_updateTimer->stop();
      break;

    default:
      break;
  }

  emit changed(notify);
}


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


/*!
 * Чтение настроек.
 */
void Settings::read()
{
  m_pos  = m_settings->value("Pos", QPoint(-999, -999)).toPoint();
  m_size = m_settings->value("Size", QSize(640, 430)).toSize();
  m_splitter = m_settings->value("Splitter").toByteArray();

  setBool("HideWelcome",            false);
  setBool("FirstRun",               true);
  setBool("EmoticonsRequireSpaces", true);
  setBool("UseEmoticons",           true);
  setBool("UseAnimatedEmoticons",   false);
  setBool("Log",                    true);
  setBool("LogPrivate",             true);
  setBool("Sound",                  true);
  setInt("EmoticonsRefreshTime",    50);
  setString("Style",                "Plastique");
  setString("EmoticonTheme",        "Kolobok");
  setString("Network",              "SimpleNet.xml");
  setList("RecentServers",          QStringList());

  setBool("Updates/Enable",         true);
  setBool("Updates/CheckOnStartup", true);
  setInt("Updates/CheckInterval",   60);
  setInt("Updates/LevelQt",         UpdateLevelQt);
  setInt("Updates/LevelCore",       UpdateLevelCore);
  setString("Updates/LastVersion",  QApplication::applicationVersion());
  setList("Updates/Mirrors", QStringList() << "http://192.168.5.1/schat/mirror/mirror.xml"); /// \todo Адрес должен быть в интернете.

  #ifndef SCHAT_NO_UPDATE
    setInt("Updates/DownloadSize",    0);
    setBool("Updates/AutoClean",      true);
    setBool("Updates/AutoDownload",   false);
  #endif

  if (m_default)
    AbstractSettings::read(m_default);

  AbstractSettings::read();

  normalizeInterval();

  qApp->setStyle(getString("Style"));

  network.fromConfig(getString("Network"));
  createServerList();

  m_settings->beginGroup("Profile");
  m_profile->setNick(m_settings->value("Nick",     QDir::home().dirName()).toString());
  m_profile->setFullName(m_settings->value("Name", "").toString());
  m_profile->setGender(m_settings->value("Gender", "male").toString());
  m_profile->setByeMsg(m_settings->value("Bye",    "IMPOMEZIA Simple Chat").toString());
  m_settings->endGroup();

  createEmoticonsMap();

  m_updateTimer->setInterval(getInt("Updates/CheckInterval") * 60 * 1000);
  if (getBool("Updates/Enable"))
    m_updateTimer->start();
  connect(m_updateTimer, SIGNAL(timeout()), SLOT(updatesCheck()));
}


/*!
 * Запись настроек.
 */
void Settings::write()
{
  m_ro << "EmoticonsRefreshTime"
       << "Updates/Mirrors"
       << "Updates/LevelQt"
       << "Updates/LevelCore"
       << "Updates/LastVersion"
       << "Updates/DownloadSize";

  setBool("FirstRun", false);
  setString("Network", network.config());
  saveRecentServers();

  AbstractSettings::write();

  m_settings->setValue("Size", m_size);
  m_settings->setValue("Pos", m_pos);
  m_settings->setValue("Splitter", m_splitter);

  m_settings->beginGroup("Profile");
  m_settings->setValue("Nick", m_profile->nick());
  m_settings->setValue("Name", m_profile->fullName());
  m_settings->setValue("Gender", m_profile->gender());
  m_settings->setValue("Bye", m_profile->byeMsg());
  m_settings->endGroup();
}


/*!
 * Запуск обновления.
 */
#ifndef SCHAT_NO_UPDATE
bool Settings::install()
{
  QString appPath = qApp->applicationDirPath();

  QSettings s(appPath + "/schat.conf", QSettings::IniFormat);
  s.beginGroup("Updates");

  if (s.value("ReadyToInstall", false).toBool()) {
    QStringList files = s.value("Files", QStringList()).toStringList();
    if (files.size() == 1)
      if (files.at(0) == "empty")
        files.clear();

    if (files.isEmpty() || !QFile::exists(appPath + "/uninstall.exe"))
      return false;

    QStringList args;
    args << "/S" << "-update" << "-run";
    if (s.value("AutoClean", true).toBool())
      args << "-clean";

    s.setValue("ReadyToInstall", false);

    QProcess::startDetached('"' + appPath + "/uninstall.exe\"", args, appPath);
    return true;
  }

  return false;
}
#endif


/*!
 * Запуск проверки обновлений.
 *
 * \param get \a true Форсированное скачивание обновлений.
 *
 * \return \a true В случае если объект Update создан и запущена проверка.
 */
#ifndef SCHAT_NO_UPDATE
bool Settings::update(bool get)
#else
bool Settings::update(bool)
#endif
{
  if (!m_update) {
    m_update = new Update(this);

    #ifndef SCHAT_NO_UPDATE
      if (get)
        m_update->downloadAll(get);
    #endif

    m_update->execute();
    return true;
  }
  else
    return false;
}


/*!
 * Создаёт список сетей и одиночных серверов.
 */
void Settings::createServerList()
{
  QString networksPath = qApp->applicationDirPath() + "/networks/";

  QDir directory(networksPath);
  directory.setNameFilters(QStringList() << "*.xml");
  QStringList files = directory.entryList(QDir::Files | QDir::NoSymLinks);
  NetworkReader network;

  foreach (QString file, files)
    if (network.readFile(networksPath + file)) {
      if (!findItem(&networksModel, network.networkName())) {
        QStandardItem *item = new QStandardItem(QIcon(":/images/applications-internet.png"), network.networkName());
        item->setData(file, Qt::UserRole);
        networksModel.appendRow(item);
      }
    }

  QStringList recent = getList("RecentServers");

  if (!recent.isEmpty())
    foreach (QString server, recent) {
      QStringList list = server.split(':');
      if (list.size() == 2) {
        QStandardItem *item = new QStandardItem(QIcon(":/images/computer.png"), list.at(0));
        item->setData(list.at(1).toInt(), Qt::UserRole);
        networksModel.appendRow(item);
      }
    }
}


/*!
 * Приводит значение "Updates/CheckInterval" к допустимым пределам.
 */
void Settings::normalizeInterval()
{
  int interval = getInt("Updates/CheckInterval");

  if (interval < 5)
    interval = 5;
  else if (interval > 1440)
    interval = 1440;

  setInt("Updates/CheckInterval", interval);
}


/*!
 * Функция проходится по модели \a networksModel и выделяет из неё одиночные серверы,
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

  setList("RecentServers", list);
}
