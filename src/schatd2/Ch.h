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
class ChHook;

class SCHAT_EXPORT Ch : public QObject
{
  Q_OBJECT

public:
  Ch(QObject *parent = 0);
  ~Ch();
  inline static bool add(ChatChannel channel)                                              { return m_self->addImpl(channel); }
  inline static int rename(ChatChannel channel, const QString &name)                       { return m_self->renameImpl(channel, name); }
  inline static void add(ChHook *hook)                                                     { if (!m_self) return; if (!m_self->m_hooks.contains(hook)) m_self->m_hooks.append(hook); }
  inline static void add2(Ch *hook)                                                        { if (!m_self->m_hooks2.contains(hook)) m_self->m_hooks2.append(hook); }
  inline static void load()                                                                { m_self->loadImpl(); }
  inline static void remove(ChatChannel channel)                                           { m_self->removeImpl(channel); }
  inline static void remove(ChHook *hook)                                                  { if (!m_self) return; m_self->m_hooks.removeAll(hook); }
  inline static void remove2(Ch *hook)                                                     { m_self->m_hooks2.removeAll(hook); }
  static bool gc(ChatChannel channel);
  static bool isCollision(const QByteArray &id, const QString &name, bool override = false);
  static ChatChannel channel(const QByteArray &id, int type = SimpleID::ChannelId, bool db = true);
  static ChatChannel channel(const QString &name, ChatChannel user = ChatChannel());
  static ChatChannel server();
  static QByteArray cookie();
  static QByteArray makeId(const QByteArray &normalized);
  static QByteArray userId(const QByteArray &uniqueId);
  static void newUserChannel(ChatChannel channel, const AuthRequest &data, const QString &host, bool created = false, quint64 socket = 0);

  // Служебные функции.
  static void addNewFeedIfNotExist(ChatChannel channel, const QString &name, ChatChannel user = ChatChannel());
  static void addNewUserFeedIfNotExist(ChatChannel channel, const QString &name);

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

  // Хуки.
  virtual void loadImpl();
  virtual void newChannelImpl(ChatChannel channel, ChatChannel user = ChatChannel());
  virtual void removeImpl(ChatChannel channel);
  virtual void serverImpl(ChatChannel channel, bool created);
  virtual void userChannelImpl(ChatChannel channel, const AuthRequest &data, const QString &host, bool created, quint64 socket);

  Cache m_cache;      ///< Кеш хранилища.

private:
  ChatChannel channelImpl(const QByteArray &id, int type = SimpleID::ChannelId, bool db = true);
  ChatChannel channelImpl(const QString &name, ChatChannel user);
  void sync(ChatChannel channel, ChatChannel user = ChatChannel());

  QList<Ch*> m_hooks2;     ///< Хуки.
  QList<ChHook *> m_hooks; ///< Хуки.
  static Ch *m_self;       ///< Указатель на себя.
};

#endif /* CH_H_ */
