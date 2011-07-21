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

#ifndef SIMPLECLIENT_P_H_
#define SIMPLECLIENT_P_H_

#include <QHash>

#include "client/SimpleClient.h"
#include "net/SimpleSocket_p.h"


class SimpleClientPrivate : public SimpleSocketPrivate
{
  Q_DECLARE_PUBLIC(SimpleClient);

public:
  SimpleClientPrivate() {}
};

/*!
 * Хранит данные синхронизации канала.
 */
class SyncChannelCache
{
public:
  SyncChannelCache() {}

  QByteArray id;           ///< Идентификатор синхронизируемого канала.
  QList<QByteArray> users; ///< Идентификатры пользователей.
};

#endif /* SIMPLECLIENT_P_H_ */
