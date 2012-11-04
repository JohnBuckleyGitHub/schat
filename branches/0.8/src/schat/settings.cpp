/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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
#include <QtNetwork>

#include "abstractprofile.h"
#include "networkreader.h"
#include "schatwindow.h"
#include "settings.h"
#include "simplechatapp.h"
#include "translation.h"
#include "version.h"
#include "widget/networkwidget.h"
#include "widget/nickedit.h"

/*!
 * \brief Конструктор класса Settings.
 */
Settings::Settings(bool unixLike, QObject *parent)
  : AbstractSettings(unixLike ? SCHAT_UNIX_CONFIG("schat.conf") : QApplication::applicationDirPath() + "/schat.conf", parent),
    m_profile(new AbstractProfile(this)),
    m_initRichTextCSS(false),
    m_unixLike(unixLike),
    m_updateTimer(new QTimer(this))
{
  QString defaultConf = unixLike ? SCHAT_UNIX_SHARE("default.conf") : QApplication::applicationDirPath() + "/default.conf";
  if (QFile::exists(defaultConf))
    m_default = new QSettings(defaultConf, QSettings::IniFormat, this);
  else
    m_default = 0;
}


/*!
 * Возвращает константную ссылку на стандартный CSS стиль для Rich Text движка.
 */
const QString& Settings::richTextCSS()
{
   if (!m_initRichTextCSS) {
     QFile file(":/css/richtext.css");
     if (file.open(QFile::ReadOnly)) {
       m_richTextCSS = QLatin1String(file.readAll());
       file.close();
     }
     m_initRichTextCSS = true;
   }

   return m_richTextCSS;
}


int Settings::save(const QString &key, bool value)
{
  if (getBool(key) == value)
    return 0;

  setBool(key, value);
  return 1;
}


int Settings::save(const QString &key, const QString &value)
{
  if (getString(key) == value)
    return 0;

  setString(key, value);
  return 1;
}


int Settings::save(const QString &key, int value)
{
  if (getInt(key) == value)
    return 0;

  setInt(key, value);
  return 1;
}


/*!
 * Возвращает список путей.
 */
QStringList Settings::path(Paths type) const
{
  QStringList out;

  switch (type) {
    case EmoticonsPath:
      return path("emoticons");

    case NetworksPath:
      return path("networks");

    case SoundsPath:
      return path("sounds");

    case StylesPath:
      return path("styles");

    case TranslationsPath:
      return path("translations");

    case LogPath:
      if (isUnixLike())
        out << SCHAT_UNIX_CONFIG("log");
      else
        out << QApplication::applicationDirPath() + "/log";
      break;
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

    case EmoticonsChanged:
      if (getBool("UseEmoticons")) {
        if (!m_emoticons)
          m_emoticons = new Emoticons(this);
      }
      else if (m_emoticons)
        m_emoticons->deleteLater();
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
  m_pos  = m_settings->value("Pos", QPoint(-99999, -99999)).toPoint();
  m_size = m_settings->value("Size", QSize(640, 430)).toSize();

  setBool("HideWelcome",            false);
  setBool("FirstRun",               true);
  setBool("EmoticonsRequireSpaces", true);
  setBool("UseEmoticons",           true);
  setBool("Log",                    true);
  setBool("LogPrivate",             true);
  setBool("Sound",                  true);
  setBool("MotdEnable",             true);
  setBool("ExitAwayOnSend",         true);
  setBool("AutoAway",               true);
  setBool("Notification",           true);
  setBool("NotificationPublic",     true);
  setBool("NoNotificationInDnD",    true);
  setBool("PopupAutoClose",         true);
  setBool("NoPopupAutoCloseInAway", true);
  setBool("CompactGenderWidget",    false);
  setBool("ServiceMessages",        true);
  setBool("Maximized",              false);
  setInt("AutoAwayTime",            10);
  setInt("PopupAutoCloseTime",      10);

  #if !defined(SCHAT_NO_STYLE)
  setString("Style",                SimpleChatApp::defaultStyle());
  #endif
  setString("EmoticonTheme",        "Kolobok");
  setString("Translation",          "auto");
  setString("Network",              "SimpleNet.xml");
  setList("RecentServers",          QStringList());
  setList("SplitterSizes",          QStringList() << "0" << "0");
  setList("ToolBarLayout",          schat::DefaultToolBarLayout);

  #ifndef SCHAT_NO_WEBKIT
    setBool("MessageGrouping",      false);
    setString("ChatStyle",          "Default");
    setString("ChatStyleVariant",   "");
  #endif

  #ifdef Q_OS_WINCE
  setBool("BigSendButton",    true);
  setBool("Sound/AlwaysPlay", true);
  #else
  setBool("BigSendButton",    false);
  setBool("Sound/AlwaysPlay", false);
  #endif

  setBool("Proxy/Enable",         false);
  setBool("Proxy/HideAndDisable", false);
  setInt("Proxy/Type",            0);
  setInt("Proxy/Port",            3128);
  setString("Proxy/Host",         "");
  setString("Proxy/UserName",     "");
  setString("Proxy/Password",     "");

  setBool("Updates/Enable",         true);
  setBool("Updates/CheckOnStartup", true);
  setInt("Updates/CheckInterval",   60);
  setInt("Updates/LevelQt",         UpdateLevelQt);
  setInt("Updates/LevelCore",       UpdateLevelCore);
  setString("Updates/LastVersion",  QApplication::applicationVersion());
  setList("Updates/Mirrors", QStringList() << "http://impomezia.com/mirror.xml");

  #ifndef SCHAT_NO_UPDATE
    setInt("Updates/DownloadSize",      0);
    setBool("Updates/AutoClean",        true);
    setBool("Updates/AutoDownload",     false);
  #endif

  setBool("Sound/MessageEnable",        true);
  setBool("Sound/PrivateMessageEnable", true);
  setBool("Sound/MuteInDnD",            true);
  setString("Sound/Message",            "Received.wav");
  setString("Sound/PrivateMessage",     "Received.wav");

  QStringList nameFilter;
  nameFilter << "*.wav";
# if defined(Q_OS_LINUX)
  nameFilter << "*.au";
# elif defined(Q_OS_MAC)
  nameFilter << "*.aiff" << "*.snd" << "*.mp3" << "*.m4a" << "*.m4b" << "*.m4p";
# endif
  setList("Sound/NameFilter", nameFilter);

# if defined(Q_OS_LINUX)
  setBool("Sound/UseExternalCmd",     true);
  setString("Sound/ExternalCmd",      "aplay -q -N %1");
# endif

  setInt("Profile/MaxRecentItems",   16);
  setList("Profile/RecentNicks",     QStringList());
  setList("Profile/RecentRealNames", QStringList());
  setList("Profile/RecentByeMsgs",   QStringList());

  if (m_default)
    AbstractSettings::read(m_default);

  AbstractSettings::read();

  Translation *translation = SimpleChatApp::instance()->translation();
  translation->setSearch(path(TranslationsPath));
  translation->load(getString("Translation"));

  normalizeInterval();
  #if !defined(SCHAT_NO_STYLE)
  QApplication::setStyle(getString("Style"));
  #endif

  if (getBool("UseEmoticons"))
    m_emoticons = new Emoticons(this);

  network.setPaths(path(NetworksPath));
  network.fromConfig(getString("Network"));
  createServerList();

  m_settings->beginGroup("Profile");
  m_profile->setNick(m_settings->value("Nick",     AbstractProfile::defaultNick()).toString());
  m_profile->setFullName(m_settings->value("Name", "").toString());
  m_profile->setGender(m_settings->value("Gender", "male").toString());
  m_profile->setByeMsg(m_settings->value("Bye",    "").toString());
  m_settings->endGroup();

  if (m_profile->nick().isEmpty())
    setBool("FirstRun", true);

  if (getInt("Profile/MaxRecentItems") < 0)
    setInt("Profile/MaxRecentItems", 0);
  NickEdit::modifyRecentList("Profile/RecentNicks",     m_profile->nick(), false);
  NickEdit::modifyRecentList("Profile/RecentRealNames", m_profile->fullName(), false);
  NickEdit::modifyRecentList("Profile/RecentByeMsgs",   m_profile->byeMsg(), false);

  m_updateTimer->setInterval(getInt("Updates/CheckInterval") * 60 * 1000);
  if (getBool("Updates/Enable"))
    m_updateTimer->start();
  connect(m_updateTimer, SIGNAL(timeout()), SLOT(updatesCheck()));
}


/*!
 * Установка глобальных настроек прокси.
 */
void Settings::setApplicationProxy() const
{
  if (!getBool("Proxy/HideAndDisable")) {
    QNetworkProxy proxy;

    if (getBool("Proxy/Enable")) {
      if (getInt("Proxy/Type") == 1)
        proxy.setType(QNetworkProxy::Socks5Proxy);
      else
        proxy.setType(QNetworkProxy::HttpProxy);

      proxy.setHostName(getString("Proxy/Host"));
      proxy.setPort(getInt("Proxy/Port"));
      proxy.setUser(getString("Proxy/UserName"));
      proxy.setPassword(getString("Proxy/Password"));
    }
    else
      proxy.setType(QNetworkProxy::NoProxy);

     QNetworkProxy::setApplicationProxy(proxy);
  }
}


/*!
 * Запись настроек.
 */
void Settings::write()
{
  m_ro << "MotdEnable"
       << "Updates/Mirrors"
       << "Updates/LevelQt"
       << "Updates/LevelCore"
       << "Updates/LastVersion"
       << "Updates/DownloadSize"
       << "Sound/NameFilter"
       << "Proxy/HideAndDisable"
       << "BigSendButton"
       << "CompactGenderWidget"
       << "Profile/MaxRecentItems";

  setBool("FirstRun", false);
  setString("Network", network.config());
  saveRecentServers();

  AbstractSettings::write();

  m_settings->setValue("Size", m_size);
  m_settings->setValue("Pos", m_pos);

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
  #if QT_VERSION >= 0x040500
    s.setIniCodec("UTF-8");
  #endif
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


QStringList Settings::path(const QString &base) const
{
  QStringList out;

  if (isUnixLike()) {
    #if !defined(Q_OS_MAC)
    out << SCHAT_UNIX_CONFIG(base);
    #endif
    out << SCHAT_UNIX_SHARE(base);
  }
  else {
    out << QApplication::applicationDirPath() + "/" + base;
  }

  return out;
}


/*!
 * Создаёт список сетей и одиночных серверов.
 */
void Settings::createServerList()
{
  QStringList networks = SimpleSettings->path(Settings::NetworksPath);
  QStringList nameFilter = QStringList("*.xml");
  QStringList files;

  foreach (QString path, networks) {
    QDir dir(path);
    foreach (QString file, dir.entryList(nameFilter, QDir::Files)) {
      if (!files.contains(file))
        files << path + "/" + file;
    }
  }

  NetworkReader network;

  foreach (QString file, files) {
    if (network.readFile(file)) {
      if (!findItem(&networksModel, network.networkName())) {
        QStandardItem *item = new QStandardItem(QIcon(":/images/network.png"), network.networkName());
        item->setData(file, Qt::UserRole);
        networksModel.appendRow(item);
      }
    }
  }

  QStringList recent = getList("RecentServers");

  if (!recent.isEmpty()) {
    foreach (QString server, recent) {
      QStringList list = server.split(':', QString::SkipEmptyParts);
      if (list.size() > 1) {
        ServerInfo info = NetworkWidget::singleServer(server);
        if (info.port == 7666)
          server = info.address;
        networksModel.appendRow(new QStandardItem(QIcon(":/images/computer.png"), server));
      }
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
    if (item->data(Qt::UserRole).type() != QVariant::String) {
      ServerInfo info = NetworkWidget::singleServer(item->text());
      list << (info.address + ":" + QString::number(info.port));
    }
  }

  setList("RecentServers", list);
}
