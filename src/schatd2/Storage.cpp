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
#include <QUuid>

#include "debugstream.h"
#include "net/packets/auth.h"
#include "net/Protocol.h"
#include "net/SimpleID.h"
#include "ServerChannel.h"
#include "ServerUser.h"
#include "Storage.h"

Storage::Storage(const QByteArray &id)
  : m_id(id)
{
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
}


Storage::~Storage()
{
}


/*!
 * Добавление нового пользователя.
 */
bool Storage::add(ServerUser *user)
{
  if (m_users.contains(user->id()))
    return false;

  m_users.insert(user->id(), user);
  m_nicks.insert(user->normalNick(), user);
  m_sessions.insert(user->session(), user);
  return true;
}


/*!
 * Удаление пользователя.
 * Пользователь удаляется из таблиц m_users, m_nicks и m_sessions,
 * а также из всех каналов, в которых он находиться.
 *
 * \param id Идентификатор пользователя.
 */
bool Storage::remove(const QByteArray &id)
{
  ServerUser *user = m_users.value(id);
  if (!user)
    return false;

  if (user->channelCount() > 0) {
    QList<QByteArray> channels = user->channels();
    for (int i = 0; i < channels.size(); ++i) {
      removeUserFromChannel(id, channels.at(i));
    }
  }

  m_users.remove(id);
  m_nicks.remove(user->normalNick());
  m_sessions.remove(user->session());
  delete user;

  return true;
}


/*!
 * Удаление пользователя из канала. Если канал не постоянный
 * и в нём больше нет пользователей, то канал также будет удалён.
 * \param userId    Идентификатор пользователя.
 * \param channelId Идентификатор канала.
 */
bool Storage::removeUserFromChannel(const QByteArray &userId, const QByteArray &channelId)
{
  ServerUser *user = this->user(userId);
  if (!user)
    return false;

  ServerChannel *chan = channel(channelId);
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

  return QCryptographicHash::hash(QString(prefix + m_id + clientId).toLatin1(), QCryptographicHash::Sha1) += SimpleID::UserId;
}


/*!
 * Получение списка идентификаторов сокетов всех пользователей
 * в каналах, которых находится данный пользователь.
 *
 * \param user Указатель на пользователя.
 */
QList<quint64> Storage::socketsFromUser(ServerUser *user)
{
  QList<quint64> out;
  if (!user)
    return out;

  QList<QByteArray> channels = user->channels();

  for (int i = 0; i < channels.size(); ++i) {
    ServerChannel *chan = channel(channels.at(i));
    if (!chan)
      continue;

    QList<quint64> list = socketsFromChannel(chan);

    for (int k = 0; k < list.size(); ++k) {
      if (!out.contains(list.at(k)))
        out.append(list.at(k));
    }
  }

  return out;
}


ServerUser* Storage::user(const QString &nick, bool normalize) const
{
  if (!normalize)
    return m_nicks.value(nick);

  return m_nicks.value(this->normalize(nick));
}


bool Storage::removeChannel(const QByteArray &id)
{
  ServerChannel *channel = m_channels.value(id);
  if (!channel)
    return false;

  SCHAT_DEBUG_STREAM(this << "removeChannel()" << channel->name())

  m_channels.remove(id);
  m_channelNames.remove(channel->normalName());
  return true;
}


/*!
 * Получение списка идентификаторов сокетов пользователей в канале.
 * \param channel Указатель на канал.
 */
QList<quint64> Storage::socketsFromChannel(ServerChannel *channel)
{
  QList<quint64> out;
  if (!channel)
    return out;

  QList<QByteArray> users = channel->users();
  for (int i = 0; i < users.size(); ++i) {
    ServerUser *user = this->user(users.at(i));
    if (user)
      out += user->socketId();
  }

  return out;
}


/*!
 * Добавление нового канала.
 */
ServerChannel* Storage::addChannel(const QString &name, bool permanent)
{
  QString normalName = normalize(name);
  if (m_channelNames.contains(normalName))
    return 0;

  ServerChannel *ch = new ServerChannel(makeChannelId(normalName), normalName, name, permanent);
  if (!ch->isValid()) {
    delete ch;
    return 0;
  }

  m_channels.insert(ch->id(), ch);
  m_channelNames.insert(normalName, ch);

  SCHAT_DEBUG_STREAM(this << "addChannel()" << normalName << name << permanent << ch->id().toHex())
  return ch;
}


ServerChannel* Storage::channel(const QString &name, bool normalize) const
{
  if (!normalize)
    return m_channelNames.value(name);

  return m_channelNames.value(this->normalize(name));
}


QByteArray Storage::session() const
{
  return SimpleID::session(m_id);
}



/*!
 * Выполняет нормализацию ника или канала, для использования его в качестве ключа
 * в соответствующих таблицах.
 */
QString Storage::normalize(const QString &text) const
{
  if (text.isEmpty())
    return "";

  QString out = text.toLower();
  for (int i = 0; i < out.size(); ++i) {
    if (m_normalize.contains(out.at(i)))
      out[i] = m_normalize.value(out.at(i));
  }

  return out;
}


QByteArray Storage::makeChannelId(const QString &name)
{
  return QCryptographicHash::hash(QString("channel:" + m_id + name).toUtf8(), QCryptographicHash::Sha1) += SimpleID::ChannelId;
}
