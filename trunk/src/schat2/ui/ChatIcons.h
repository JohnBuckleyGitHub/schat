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

#ifndef CHATICONS_H_
#define CHATICONS_H_

#include <QIcon>

#include "Channel.h"

class SCHAT_CORE_EXPORT ChatIcons
{
  ChatIcons() {}

public:
  /// Опции получения иконки канала.
  enum ChannelIcon {
    NoOptions = 0,    ///< Нет специальных опций
    Statuses = 1,     ///< Отображать статусы, кроме статуса "не в сети".
    OfflineStatus = 2 ///< Также отображать статус "не в сети".
  };

  static QIcon icon(ClientChannel channel, int options = Statuses | OfflineStatus);
  static QIcon icon(const QIcon &icon, const QString &overlay);
  static QIcon icon(const QString &file, const QString &overlay);

private:
  static QString overlay(int status);
};

#endif /* CHATICONS_H_ */
