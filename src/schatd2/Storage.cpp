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
  m_settings->setDefault(QLatin1String("Kernel"),      QString());
  m_settings->setDefault(QLatin1String("Listen"),      QStringList("0.0.0.0:7667"));
  m_settings->setDefault(QLatin1String("MainChannel"), QLatin1String("Main"));
  m_settings->setDefault(QLatin1String("PrivateId"),   QString(SimpleID::toBase64(SimpleID::uniqueId())));
  m_settings->setDefault(QLatin1String("ServerName"),  QString());

  m_db = new DataBase(this);
}


int Storage::start()
{
  m_serverData->setPrivateId(m_settings->value(QLatin1String("PrivateId")).toString().toUtf8());
  m_serverData->setName(m_settings->value(QLatin1String("ServerName")).toString());

  // FIXME Создание основного канала.
  QString mainChannel = m_settings->value(QLatin1String("MainChannel")).toString();
  if (!mainChannel.isEmpty()) {
    m_serverData->setChannelId(addChannel(mainChannel, true)->id());
  }

  m_db->start();
  return 0;
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
  m_sessions.insert(user->session(), user);
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
  m_sessions.remove(user->session());

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
 * Создание идентификатора пользователя.
 */
QByteArray Storage::makeUserId(int type, const QByteArray &clientId) const
{
  QString prefix;
  if (type == AuthRequestData::Anonymous) {
    prefix = "anonymous:";
  }

  return QCryptographicHash::hash(QString(prefix + m_serverData->privateId() + clientId).toLatin1(), QCryptographicHash::Sha1) += SimpleID::UserId;
}


ChatUser Storage::user(const QString &nick, bool normalize) const
{
  if (!normalize)
    return m_nicks.value(nick);

  return m_nicks.value(this->normalize(nick));
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
ChatChannel Storage::addChannel(ChatUser user)
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


/*!
 * Добавление нового канала.
 */
ChatChannel Storage::addChannel(const QString &name, bool permanent)
{
  QString normalName = normalize(name);
  if (m_channelNames.contains(normalName))
    return ChatChannel();

  ChatChannel ch = ChatChannel(new ServerChannel(makeChannelId(normalName), normalName, name, permanent));
  if (!ch->isValid())
    return ChatChannel();

  m_channels.insert(ch->id(), ch);
  m_channelNames.insert(normalName, ch);

  SCHAT_DEBUG_STREAM(this << "addChannel()" << normalName << name << permanent << ch->id().toHex())
  return ch;
}


ChatChannel Storage::channel(const QString &name, bool normalize) const
{
  if (!normalize)
    return m_channelNames.value(name);

  return m_channelNames.value(this->normalize(name));
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
}


QByteArray Storage::session() const
{
  return SimpleID::session(m_serverData->privateId());
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


QByteArray Storage::makeChannelId(const QString &name)
{
  return QCryptographicHash::hash(QString("channel:" + m_serverData->privateId() + name).toUtf8(), QCryptographicHash::Sha1) += SimpleID::ChannelId;
}
