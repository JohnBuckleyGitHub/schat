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

#ifndef CHANNELUTILS_H_
#define CHANNELUTILS_H_

#include "Channel.h"
#include "schat.h"

class SCHAT_CORE_EXPORT ChannelUtils
{
private:
  ChannelUtils() {}

public:
  /// Опции получения иконки канала.
  enum IconOptions {
    NoOptions = 0,    ///< Нет специальных опций
    Statuses = 1,     ///< Отображать статусы, кроме статуса "не в сети".
    OfflineStatus = 2 ///< Также отображать статус "не в сети".
  };

  static ClientChannel channel(const QByteArray &id);
  static QString webIcon(const QString &action);
  static QVariantMap toWebButton(const QByteArray &id, const QString &action, const QString &title);

  static QIcon icon(ClientChannel channel, int options = 1);

private:
  static QString overlay(int status);
};

#endif /* CHANNELUTILS_H_ */
