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

#ifndef SERVERCHANNEL_H_
#define SERVERCHANNEL_H_

#include "Channel.h"

class SCHAT_EXPORT ServerChannel: public Channel
{
public:
  ServerChannel(ClientChannel channel);
  ServerChannel(const QByteArray &id, const QString &name);
  ~ServerChannel();
  bool setName(const QString &name);
  inline qint64 key() const { return m_key; }
//  inline QString normalName() const { return m_normalName; }
  inline void setKey(qint64 key) { m_key = key; }
  inline const QByteArray& normalized() const { return m_normalized; }

private:
  qint64 m_key;            ///< Ключ в таблице channels.
  QByteArray m_normalized; ///< Нормализованное имя канала.
//  QString m_normalName; ///< Нормализованное имя канала.
};

typedef QSharedPointer<ServerChannel> ChatChannel;

#endif /* SERVERCHANNEL_H_ */
