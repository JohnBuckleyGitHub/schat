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

#include <QCryptographicHash>
#include <QFile>
#include <QSslConfiguration>
#include <QSslKey>
#include <QStringList>
#include <QUuid>

#include "DataBase.h"
#include "debugstream.h"
#include "FileLocations.h"
#include "net/packets/auth.h"
#include "net/Protocol.h"
#include "net/ServerData.h"
#include "net/SimpleID.h"
#include "Settings.h"
#include "ServerUser.h"
#include "Storage.h"

Storage *Storage::m_self = 0;

Storage::Storage(QObject *parent)
  : QObject(parent)
  , m_allowSlaves(false)
{
  m_self = this;

  /// \todo Сделать возможность использования внешнего файла с картой нормализации.
  m_normalize.insert(0x0430, 'a'); // а
  m_normalize.insert(0x0435, 'e'); // е
  m_normalize.insert(0x0451, 'e'); // ё
  m_normalize.insert(0x043C, 'm'); // м
  m_normalize.insert(0x0440, 'p'); // р
  m_normalize.insert(0x0441, 'c'); // с
  m_normalize.insert(0x043E, 'o'); // о
  m_normalize.insert(0x0443, 'y'); // у
  m_normalize.insert(0x0445, 'x'); // х
  m_normalize.insert('l', 'i');

  m_serverData = new ServerData();
  m_locations = new FileLocations(this);

  // Инициализация настроек по умолчанию.
  m_settings = new Settings(m_locations->path(FileLocations::ConfigFile), this);
  m_settings->setDefault(QLatin1String("Certificate"), QLatin1String("server.crt"));
  m_settings->setDefault(QLatin1String("Kernel"),      QString());
  m_settings->setDefault(QLatin1String("Listen"),      QStringList("0.0.0.0:7667"));
  m_settings->setDefault(QLatin1String("MainChannel"), 1);
  m_settings->setDefault(QLatin1String("PrivateId"),   QString(SimpleID::toBase64(SimpleID::uniqueId())));
  m_settings->setDefault(QLatin1String("PrivateKey"),  QLatin1String("server.key"));
  m_settings->setDefault(QLatin1String("ServerName"),  QString());

  m_db = new DataBase(this);
}


int Storage::start()
{
  setDefaultSslConf();

  m_serverData->setPrivateId(m_settings->value(QLatin1String("PrivateId")).toString().toUtf8());
  m_serverData->setName(m_settings->value(QLatin1String("ServerName")).toString());

  m_db->start();

  qint64 key = m_settings->value(QLatin1String("MainChannel")).toLongLong();
  if (key > 0) {
    ChatChannel channel = this->channel(key);
    if (channel)
      m_serverData->setChannelId(channel->id());
  }

  return 0;
}


qint64 Storage::timestamp()
{
  #if QT_VERSION >= 0x040700
  return QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
  #else
  return qint64(QDateTime::currentDateTime().toUTC().toTime_t()) * 1000;
  #endif
}


void Storage::addSlave(const QByteArray &id)
{
  if (m_slaves.contains(id))
    return;

  m_slaves.append(id);
}


Storage::~Storage()
{
  delete m_serverData;
}


/*!
 * Добавление нового пользователя.
 */
bool Storage::add(ChatUser user)
{
  if (m_users.contains(user->id()))
    return false;

  user->setServerNumber(m_serverData->number());

  m_db->add(user);
  m_users.insert(user->id(), user);
  m_nicks.insert(user->normalNick(), user);
  return true;
}


/*!
 * Проверка на принадлежность пользователей одному вторичному серверу.
 * Если идентификаторы сокетов совпадают, значит, пользователи находятся на одном вторичном сервере.
 *
 * \return true если пользователи находятся на одном вторичном сервере или один или оба не являются валидными.
 */
bool Storage::isSameSlave(const QByteArray &id1, const QByteArray &id2)
{
  ChatUser user1 = user(id1);
  if (!user1)
    return true;

  ChatUser user2 = user(id2);
  if (!user2)
    return true;

  if (isAllowSlaves() && user1->socketId() == user2->socketId())
    return true;

  return false;
}


/*!
 * Удаление пользователя.
 * Пользователь удаляется из таблиц m_users, m_nicks и m_sessions,
 */
bool Storage::remove(ChatUser user)
{
  if (user->channelsCount() > 1) {
    QList<QByteArray> channels = user->channels();
    for (int i = 0; i < channels.size(); ++i) {
      removeUserFromChannel(user->id(), channels.at(i));
    }
  }

  m_users.remove(user->id());
  m_nicks.remove(user->normalNick());
  m_db->update(user);

  return true;
}


/*!
 * Удаление пользователя из канала. Если канал не постоянный
 * и в нём больше нет пользователей, то канал также будет удалён.
 *
 * \param userId    Идентификатор пользователя.
 * \param channelId Идентификатор канала.
 */
bool Storage::removeUserFromChannel(const QByteArray &userId, const QByteArray &channelId)
{
  ChatUser user = this->user(userId);
  if (!user)
    return false;

  ChatChannel chan = channel(channelId);
  if (!chan)
    return false;

  bool result = chan->removeUser(userId);
  user->removeChannel(channelId);

  if (chan->userCount() == 0 && !chan->isPermanent())
    removeChannel(chan->id());

  return result;
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

  return m_db->user(id);
}


ChatUser Storage::user(const QString &nick, bool normalize) const
{
  if (!normalize)
    return m_nicks.value(nick);

  return m_nicks.value(this->normalize(nick));
}


LoginReply Storage::login(ChatUser user, const QString &name, const QByteArray &password)
{
  if (m_serverData->name().isEmpty())
    return LoginReply(Notice::ServiceUnavailable);

  if (SimpleID::typeOf(password) != SimpleID::PasswordId)
    return LoginReply(Notice::BadRequest);

  QString login = RegReply::filter(name);
  if (login.isEmpty())
    return LoginReply(Notice::BadRequest);

//  login += '@' + m_serverData->name();
//
//  qint64 result = m_db->reg(user, login, password);
//  if (result == -2)
//    return RegReply(Notice::UserAlreadyExists);

  return LoginReply();
}


/*!
 * Создание идентификатора пользователя.
 */
QByteArray Storage::makeUserId(int type, const QByteArray &userId) const
{
  QString prefix;
  if (type == AuthRequestData::Anonymous || type == AuthRequestData::Cookie)
    prefix = QLatin1String("anonymous:");
  else if (type == AuthRequestData::SlaveNode)
    prefix = QLatin1String("slave:");

  return QCryptographicHash::hash(QString(prefix + m_serverData->privateId() + userId).toLatin1(), QCryptographicHash::Sha1) += SimpleID::UserId;
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

    foreach (QByteArray id, channel->users()) {
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
 */
RegReply Storage::reg(ChatUser user, const QString &name, const QByteArray &password)
{
  if (m_serverData->name().isEmpty())
    return RegReply(Notice::ServiceUnavailable);

  if (SimpleID::typeOf(password) != SimpleID::PasswordId)
    return RegReply(Notice::BadRequest);

  QString login = RegReply::filter(name);
  if (login.isEmpty())
    return RegReply(Notice::BadRequest);

  login += '@' + m_serverData->name();

  qint64 result = m_db->reg(user, login, password);
  if (result == -2)
    return RegReply(Notice::UserAlreadyExists);

  return RegReply(login);
}


/*!
 * Обработка смены ника пользователя.
 */
void Storage::rename(ChatUser user)
{
  if (!m_users.contains(user->id()))
    return;

  ChatChannel channel = m_channels.value(user->channel());
  if (!channel)
    return;

  m_nicks.remove(user->normalNick());
  m_channelNames.remove(QLatin1String("~") + user->normalNick());

  user->setNormalNick(normalize(user->nick()));
  channel->setName(QLatin1String("~") + user->nick());
  channel->setNormalName(QLatin1String("~") + user->normalNick());

  m_nicks[user->normalNick()] = user;
  m_channelNames[channel->normalName()] = channel;
}


void Storage::store(ChatUser user)
{
  m_db->add(user);
}


void Storage::update(ChatUser user)
{
  m_db->update(user);
}


bool Storage::removeChannel(const QByteArray &id)
{
  ChatChannel channel = m_channels.value(id);
  if (!channel)
    return false;

  SCHAT_DEBUG_STREAM(this << "removeChannel()" << channel->name())

  m_channels.remove(id);
  m_channelNames.remove(channel->normalName());
  return true;
}


/*!
 * Создание специального приватного канала для пользователя.
 * Данный канал служит для обмена статусной информацией.
 */
ChatChannel Storage::channel(ChatUser user)
{
  QByteArray id = SimpleID::setType(SimpleID::ChannelId, user->id());
  ChatChannel channel = m_channels.value(id);

  if (!channel) {
    QString normalName = QLatin1String("~") + user->normalNick();
    channel = ChatChannel(new ServerChannel(id, normalName, QLatin1String("~") + user->nick()));

    m_channels[id] = channel;
    m_channelNames[normalName] = channel;
  }

  user->addChannel(id);
  channel->addUser(user->id());

  return channel;
}


ChatChannel Storage::channel(const QByteArray &id)
{
  ChatChannel channel = m_channels.value(id);
  if (channel)
    return channel;

  channel = m_db->channel(id);
  if (channel) {
    m_channels[id] = channel;
    m_channelNames[channel->normalName()] = channel;
  }

  return channel;
}


ChatChannel Storage::channel(const QString &name)
{
  QString normalName = normalize(name);
  ChatChannel channel = m_channelNames.value(normalName);
  if (channel || name.startsWith(QLatin1Char('~')))
    return channel;

  QByteArray id = makeChannelId(normalName);
  channel = this->channel(id);
  if (!channel) {
    channel = ChatChannel(new ServerChannel(id, normalName, name, true));
    m_db->add(channel);
    m_channels[id] = channel;
    m_channelNames[normalName] = channel;
  }

  return channel;
}


ChatChannel Storage::channel(qint64 id)
{
  return m_db->channel(id);
}


/*!
 * Получение списка идентификаторов сокетов пользователей в канале.
 * \param channel Указатель на канал.
 */
QList<quint64> Storage::socketsFromChannel(ChatChannel channel)
{
  QList<quint64> out;
  if (!channel)
    return out;

  QList<QByteArray> users = channel->users();
  for (int i = 0; i < users.size(); ++i) {
    ChatUser user = this->user(users.at(i));
    if (user && !out.contains(user->socketId()))
      out += user->socketId();
  }

  return out;
}


/*!
 * Получение списка идентификаторов сокетов.
 */
QList<quint64> Storage::socketsFromIds(const QList<QByteArray> &ids)
{
  QList<quint64> out;
  if (ids.isEmpty())
    return out;

  foreach (QByteArray id, ids) {
    int type = SimpleID::typeOf(id);

    if (type == SimpleID::ChannelId) {
      ChatChannel channel = this->channel(id);
      if (channel) {
        QList<QByteArray> users = channel->users();

        for (int i = 0; i < users.size(); ++i) {
          ChatUser user = this->user(users.at(i));
          if (user && !out.contains(user->socketId()))
            out += user->socketId();
        }
      }
    }
    else if (type == SimpleID::UserId) {
      ChatUser user = this->user(id);
      if (user && !out.contains(user->socketId()))
        out += user->socketId();
    }
  }

  return out;
}


void Storage::addChannel(ChatChannel channel)
{
  channel->setNormalName(normalize(channel->name()));
  m_channels[channel->id()] = channel;
  m_channelNames[channel->normalName()] = channel;
  m_db->add(channel);
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



/*!
 * Выполняет нормализацию ника или канала, для использования его в качестве ключа
 * в соответствующих таблицах.
 */
QString Storage::normalize(const QString &text) const
{
  QString out = text.toLower().simplified();
  if (out.isEmpty())
    return out;

  for (int i = 0; i < out.size(); ++i) {
    if (m_normalize.contains(out.at(i)))
      out[i] = m_normalize.value(out.at(i));
  }

  return out;
}


QByteArray Storage::makeChannelId(const QString &name) const
{
  return QCryptographicHash::hash(QString("channel:" + m_serverData->privateId() + name).toUtf8(), QCryptographicHash::Sha1) += SimpleID::ChannelId;
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

  file.setFileName(keyFile);
  if (file.open(QIODevice::ReadOnly)) {
    conf.setPrivateKey(QSslKey(&file, QSsl::Rsa));
    file.close();
  }

  QSslConfiguration::setDefaultConfiguration(conf);
# endif
}
