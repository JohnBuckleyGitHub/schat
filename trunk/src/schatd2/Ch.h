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

class SCHAT_EXPORT Ch : public QObject
{
  Q_OBJECT

public:
  Ch(QObject *parent = 0);
  inline static bool add(ChatChannel channel)                                             { return m_self->addImpl(channel); }
  inline static bool gc(ChatChannel channel)                                              { return m_self->gcImpl(channel);  }
  inline static ChatChannel channel(const QByteArray &id, int type = SimpleID::ChannelId) { return m_self->channelImpl(id, type); }
  inline static ChatChannel channel(const QString &name)                                  { return m_self->channelImpl(name); }
  inline static void add(Ch *hook)                                                        { if (!m_self->m_hooks.contains(hook)) m_self->m_hooks.append(hook); }
  inline static void remove(Ch *hook)                                                     { m_self->m_hooks.removeAll(hook); }
  inline static void remove(ChatChannel channel)                                          { m_self->removeImpl(channel); }
  inline static void rename(ChatChannel channel, const QString &name)                     { m_self->renameImpl(channel, name); }
  static bool isCollision(const QByteArray &id, const QString &name);
  static QByteArray makeId(const QByteArray &normalized);

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
  virtual bool addImpl(ChatChannel channel);
  virtual bool gcImpl(ChatChannel channel);
  virtual ChatChannel channelImpl(const QByteArray &id, int type);
  virtual ChatChannel channelImpl(const QString &name);
  virtual void newChannelImpl(ChatChannel channel);
  virtual void removeImpl(ChatChannel channel);
  virtual void renameImpl(ChatChannel channel, const QString &name);

  Cache m_cache;      ///< Кеш хранилища.

private:
  QList<Ch*> m_hooks; ///< Хуки.
  static Ch *m_self;  ///< Указатель на себя.
};

#endif /* CH_H_ */
