/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CH_H_
#define CH_H_

#include <QObject>

#include "net/SimpleID.h"
#include "ServerChannel.h"

class AuthRequest;

class SCHAT_EXPORT Ch : public QObject
{
  Q_OBJECT

public:
  Ch(QObject *parent = 0);
  ~Ch();
  inline static bool add(ChatChannel channel)                                              { return m_self->addImpl(channel); }
  inline static int rename(ChatChannel channel, const QString &name)                       { return m_self->renameImpl(channel, name); }
  inline static void add(Ch *hook)                                                         { if (!m_self->m_hooks.contains(hook)) m_self->m_hooks.append(hook); }
  inline static void load()                                                                { m_self->loadImpl(); }
  inline static void newUserChannel(ChatChannel channel, const AuthRequest &data, const QString &host, bool created = false) { m_self->userChannelImpl(channel, data, host, created); }
  inline static void remove(Ch *hook)                                                      { m_self->m_hooks.removeAll(hook); }
  inline static void remove(ChatChannel channel)                                           { m_self->removeImpl(channel); }
  static bool gc(ChatChannel channel);
  static bool isCollision(const QByteArray &id, const QString &name);
  static ChatChannel channel(const QByteArray &id, int type = SimpleID::ChannelId, bool db = true);
  static ChatChannel channel(const QString &name, ChatChannel user = ChatChannel());
  static ChatChannel server();
  static QByteArray cookie();
  static QByteArray makeId(const QByteArray &normalized);
  static QByteArray userId(const QByteArray &uniqueId);

protected:
  /// Внутренний кэш хранилища.
  class Cache
  {
  public:
    Cache() {}
    inline ChatChannel channel(const QByteArray &id) const { return m_channels.value(id); }
    void add(ChatChannel channel);
    void remove(const QByteArray &id);
    void rename(ChatChannel channel, const QByteArray &before);

  private:
    QHash<QByteArray, ChatChannel> m_channels;
  };

protected:
  int renameImpl(ChatChannel channel, const QString &name);
  virtual bool addImpl(ChatChannel channel);
  virtual ChatChannel channelImpl(const QByteArray &id, int type = SimpleID::ChannelId, bool db = true);
  virtual ChatChannel channelImpl(const QString &name, ChatChannel user);

  // Хуки.
  virtual void channelImpl(ChatChannel channel, ChatChannel user = ChatChannel());
  virtual void loadImpl();
  virtual void newChannelImpl(ChatChannel channel, ChatChannel user = ChatChannel());
  virtual void removeImpl(ChatChannel channel);
  virtual void serverImpl(ChatChannel channel, bool created);
  virtual void userChannelImpl(ChatChannel channel, const AuthRequest &data, const QString &host, bool created);

  // Служебные функции.
  void addNewFeedIsNotExist(ChatChannel channel, const QString &name, ChatChannel user = ChatChannel());
  void addNewUserFeedIsNotExist(ChatChannel channel, const QString &name);

  Cache m_cache;      ///< Кеш хранилища.

private:
  QList<Ch*> m_hooks; ///< Хуки.
  static Ch *m_self;  ///< Указатель на себя.
};

#endif /* CH_H_ */
