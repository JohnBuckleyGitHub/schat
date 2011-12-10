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

#include <QFile>
#include <QSslConfiguration>
#include <QSslKey>
#include <QStringList>
#include <QUuid>

#include "DataBase.h"
#include "DateTime.h"
#include "debugstream.h"
#include "FileLocations.h"
#include "net/packets/auth.h"
#include "net/Protocol.h"
#include "net/ServerData.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "Normalize.h"
#include "plugins/StorageHooks.h"
#include "ServerUser.h"
#include "Settings.h"
#include "Storage.h"

Storage *Storage::m_self = 0;

Storage::Storage(QObject *parent)
  : QObject(parent)
  , m_allowSlaves(false)
{
  m_self = this;

  Normalize::init();

  m_hooks = new StorageHooks();
  m_serverData = new ServerData();
  m_locations = new FileLocations(this);

  // Инициализация настроек по умолчанию.
  m_settings = new Settings(m_locations->path(FileLocations::ConfigFile), this);
  m_settings->setDefault("Certificate", QLatin1String("server.crt"));
  m_settings->setDefault("Kernel",      QString());
  m_settings->setDefault("Listen",      QStringList("0.0.0.0:7667"));
  m_settings->setDefault("LogLevel",    2);
  m_settings->setDefault("MainChannel", 1);
  m_settings->setDefault("PrivateId",   QString(SimpleID::encode(SimpleID::uniqueId())));
  m_settings->setDefault("PrivateKey",  QLatin1String("server.key"));
  m_settings->setDefault("ServerName",  QString());

  m_log = new NodeLog;
  m_db = new DataBase(this);
}


Storage::~Storage()
{
  delete m_hooks;
  delete m_log;
  delete m_serverData;
}


/*!
 * Запуск сервера, функция производит инициализацию состояния и объектов сервера.
 */
int Storage::start()
{
  m_log->open(m_locations->file(FileLocations::LogPath, m_locations->path(FileLocations::BaseName) + ".log"), static_cast<NodeLog::Level>(m_settings->value("LogLevel").toInt()));

  setDefaultSslConf();

  m_serverData->setPrivateId(m_settings->value(QLatin1String("PrivateId")).toString().toUtf8());
  m_serverData->setName(m_settings->value(QLatin1String("ServerName")).toString());

  m_db->start();

  qint64 key = m_settings->value(QLatin1String("MainChannel")).toLongLong();
  if (key > 0) {
    ChatChannel channel = DataBase::channel(key);
    if (channel) {
      m_serverData->setChannelId(channel->id());
      SCHAT_LOG_DEBUG(<< "Main channel:" << channel->name() << "id:" << SimpleID::encode(channel->id()));
    }
  }

  return 0;
}


void Storage::addSlave(const QByteArray &id)
{
  if (m_slaves.contains(id))
    return;

  m_slaves.append(id);
}


/*!
 * Проверка на принадлежность пользователей одному вторичному серверу.
 * Если идентификаторы сокетов совпадают, значит, пользователи находятся на одном вторичном сервере.
 *
 * \return true если пользователи находятся на одном вторичном сервере или один или оба не являются валидными.
 * \bug Нарушена совместимость с вторичным сервером.
 */
bool Storage::isSameSlave(const QByteArray &id1, const QByteArray &id2)
{
  ChatUser user1 = user(id1);
  if (!user1)
    return true;

  ChatUser user2 = user(id2);
  if (!user2)
    return true;

//  if (isAllowSlaves() && user1->socketId() == user2->socketId())
//    return true;

  return false;
}


/*!
 * Получение пользователя по идентификатору.
 *
 * \param id      Идентификатор пользователя.
 * \param offline true в случае если пользователь не найден, то будет произведена попытка получить его из базы.
 */
ChatUser Storage::user(const QByteArray &id, bool offline) const
{
  if (m_users.contains(id))
    return m_users.value(id);

  if (!offline)
    return ChatUser();

  ChatUser user = m_db->user(id);
  if (user && m_users.contains(user->id()))
    return m_users.value(user->id());

  return user;
}


/*!
 * Процедура авторизации анонимного пользователя во время работы.
 */
LoginReply Storage::login(ChatUser user, const QString &name, const QByteArray &password)
{
  Q_UNUSED(user)

  if (!m_serverData->is(ServerData::PasswordAuthSupport))
    return LoginReply(Notice::ServiceUnavailable);

  if (SimpleID::typeOf(password) != SimpleID::PasswordId)
    return LoginReply(Notice::BadRequest);

  QString login = LoginReply::filter(name, m_serverData->name());
  if (login.isEmpty())
    return LoginReply(Notice::BadRequest);

//  Account account = m_db->account(login);
//  if (!account.isValid())
//    return LoginReply(Notice::UserNotExists);
//
//  if (account.password() != password)
//    return LoginReply(Notice::Forbidden);
//
  LoginReply reply(login);
//  user->setAccount(login);
//  user->groups().add("registered");
//  m_db->update(user);

//  if (user->id() != account.userId) {
//    reply.setStatus(Notice::Conflict);
//    return reply;
//  }

  return reply;
}


/*!
 * Создание идентификатора пользователя.
 */
QByteArray Storage::makeUserId(int type, const QByteArray &userId) const
{
  QByteArray prefix;
  if (type == AuthRequest::Anonymous || type == AuthRequest::Cookie || type == AuthRequest::Password)
    prefix = "anonymous:";
  else if (type == AuthRequest::SlaveNode)
    prefix = "slave:";

  return SimpleID::make(prefix + m_serverData->privateId() + userId, SimpleID::UserId);
}


QList<QByteArray> Storage::users(const QByteArray &id)
{
  QList<QByteArray> out;
  ChatUser user = this->user(id);
  if (!user)
    return out;

  QList<QByteArray> channels = user->channels();

  for (int i = 0; i < channels.size(); ++i) {
    ChatChannel channel = this->channel(channels.at(i));
    if (!channel)
      continue;

    foreach (QByteArray id, channel->channels().all()) {
      if (!out.contains(id))
        out.append(id);
    }
  }

  return out;
}


/*!
 * Регистрация пользователя.
 *
 * \param user     Пользователь.
 * \param name     Имя аккаунта пользователя.
 * \param password Пароль.
 * \param data     JSON данные.
 */
RegReply Storage::reg(ChatUser user, const QString &name, const QByteArray &password, const QVariant &data)
{
  if (!m_serverData->is(ServerData::PasswordAuthSupport))
    return RegReply(Notice::ServiceUnavailable);

  if (SimpleID::typeOf(password) != SimpleID::PasswordId)
    return RegReply(Notice::BadRequest);

  QString login = RegReply::filter(name);
  if (login.isEmpty())
    return RegReply(Notice::BadRequest);

  login += '@' + m_serverData->name();

  qint64 result = m_db->reg(user, login, password, data);
  if (result == -2)
    return RegReply(Notice::UserAlreadyExists);

  if (result == -1)
    return RegReply(Notice::InternalError);

  return RegReply(login);
}


void Storage::store(ChatUser user)
{
  m_db->add(user);
}


void Storage::update(ChatUser user)
{
  m_db->update(user);
}


/*!
 * Добавление канала.
 */
bool Storage::add(ChatChannel channel)
{
  if (m_db->add(channel) == -1)
    return false;

  m_cache.add(channel);
  return true;
}


/*!
 * Получение канала по идентификатору канала или идентификатору нормализированного имени либо Сookie.
 *
 * \todo В случае получения пользовательского канала по нормализированному имени и если ник устарел, сбрасывать ник и возвращать пустой канал.
 */
ChatChannel Storage::channel(const QByteArray &id, int type)
{
  ChatChannel channel = m_cache.channel(id);
  if (channel)
    return channel;

  channel = DataBase::channel(id, type);
  m_cache.add(channel);

  return channel;
}


/*!
 * Получение канала по имени.
 *
 * \sa StorageHooks::createdNewChannel().
 */
ChatChannel Storage::channel(const QString &name)
{
  QByteArray normalized = Normalize::toId('#' + name);
  ChatChannel channel = this->channel(normalized);

  if (!channel) {
    channel = ChatChannel(new ServerChannel(makeId(normalized), name));
    m_hooks->createdNewChannel(channel);
    add(channel);
  }

  return channel;
}


/*!
 * Удаление канала.
 */
void Storage::remove(ChatChannel channel)
{
  DataBase::update(channel);

  m_cache.remove(channel->id());
}


/*!
 * Переименование канала.
 */
void Storage::rename(ChatChannel channel, const QString &name)
{
  if (channel->type() != SimpleID::UserId)
    return;

  QByteArray normalized = channel->normalized();
  ChatChannel exist = this->channel(Normalize::toId('~' + name), SimpleID::UserId);
  if (exist && exist->id() != channel->id())
    return;

  channel->setName(name);
  m_cache.rename(channel, normalized);
  update(channel);
}


void Storage::update(ChatChannel channel)
{
  m_db->update(channel);
}


/*!
 * Генерирование новой Cookie.
 */
QByteArray Storage::cookie() const
{
  return SimpleID::randomId(SimpleID::CookieId, m_serverData->privateId());
}


QByteArray Storage::makeId(const QByteArray &normalized) const
{
  return SimpleID::make("channel:" + m_serverData->privateId() + normalized, SimpleID::ChannelId);
}


void Storage::setDefaultSslConf()
{
# if !defined(SCHAT_NO_SSL)
  if (!QSslSocket::supportsSsl())
    return;

  QString crtFile = m_locations->file(FileLocations::ConfigPath, m_settings->value(QLatin1String("Certificate")).toString());
  if (crtFile.isEmpty())
    return;

  QString keyFile = m_locations->file(FileLocations::ConfigPath, m_settings->value(QLatin1String("PrivateKey")).toString());
  if (crtFile.isEmpty())
    return;

  QSslConfiguration conf = QSslConfiguration::defaultConfiguration();

  QFile file(crtFile);
  if (file.open(QIODevice::ReadOnly)) {
    conf.setLocalCertificate(QSslCertificate(&file));
    file.close();
  }
  else {
    SCHAT_LOG_WARN() << "Could not open Certificate file" << file.fileName() << ":" << file.errorString();
  }

  file.setFileName(keyFile);
  if (file.open(QIODevice::ReadOnly)) {
    conf.setPrivateKey(QSslKey(&file, QSsl::Rsa));
    file.close();
  }
  else {
    SCHAT_LOG_WARN() << "Could not open Private Key file" << file.fileName() << ":" << file.errorString();
  }

  QSslConfiguration::setDefaultConfiguration(conf);
# endif
}


/*!
 * Добавление канала в кеш.
 */
void Storage::Cache::add(ChatChannel channel)
{
  if (!channel)
    return;

  m_channels[channel->id()] = channel;
  m_channels[channel->normalized()] = channel;

  if (channel->account())
    m_channels[channel->account()->cookie()] = channel;
}


/*!
 * Удаление канала из кэша.
 */
void Storage::Cache::remove(const QByteArray &id)
{
  ChatChannel channel = this->channel(id);
  if (!channel)
    return;

  m_channels.remove(channel->id());
  m_channels.remove(channel->normalized());

  if (channel->account())
    m_channels.remove(channel->account()->cookie());

  return;
}


void Storage::Cache::rename(ChatChannel channel, const QByteArray &before)
{
  m_channels.remove(before);
  m_channels[channel->normalized()] = channel;
}
