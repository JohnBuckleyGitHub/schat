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

#ifndef STORAGEHOOKS_H_
#define STORAGEHOOKS_H_

#include "ServerChannel.h"
#include "schat.h"

class AuthRequest;

class SCHAT_EXPORT StorageHooks : public QObject
{
  Q_OBJECT

public:
  StorageHooks(QObject *parent = 0);
  inline static int newChannel(ChatChannel channel)    { return m_self->newChannelImpl(channel); }
  inline static int newUserChannel(ChatChannel channel, const AuthRequest &data, const QString &host) { return m_self->newUserChannelImpl(channel, data, host); }
  inline static void add(StorageHooks *hook)           { if (!m_self->m_hooks.contains(hook)) m_self->m_hooks.append(hook); }
  inline static void remove(StorageHooks *hook)        { m_self->m_hooks.removeAll(hook); }

protected:
  virtual int newChannelImpl(ChatChannel channel);
  virtual int newUserChannelImpl(ChatChannel channel, const AuthRequest &data, const QString &host);

private:
  QList<StorageHooks*> m_hooks; ///< Хуки.
  static StorageHooks *m_self;  ///< Указатель на себя.
};

#endif /* STORAGEHOOKS_H_ */
