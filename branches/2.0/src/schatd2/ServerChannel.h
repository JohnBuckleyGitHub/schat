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

class ServerChannel: public Channel
{
public:
  ServerChannel(const QByteArray &id, const QString &normalName, const QString &name, bool permanent = false);
  ~ServerChannel();
  inline QString normalName() const { return m_normalName; }
  inline void setPermanent(bool permanent) { m_permanent = permanent; }
  inline bool isPermanent() const { return m_permanent; }

private:
  bool m_permanent;     ///< Канал не будет удалён если из него выйдут все пользователи.
  QString m_normalName; ///< Нормализованное имя канала.
};

#endif /* SERVERCHANNEL_H_ */
