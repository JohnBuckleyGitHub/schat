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

#include <QDebug>

#include "net/packets/notices.h"
#include "plugins/StorageHooks.h"
#include "Storage.h"

StorageHooks::StorageHooks()
  : m_storage(Storage::i())
{
}


/*!
 * Создание нового канала.
 * Эта функция будет вызвана только для только что созданных каналов, до добавления их в базу данных.
 */
int StorageHooks::createdNewChannel(ChatChannel channel)
{
  qDebug() << " -- HOOK: NEW CHANNEL" << channel->name();

  return Notice::OK;
}
