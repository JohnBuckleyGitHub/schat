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

#ifndef USERUTILS_H_
#define USERUTILS_H_

#include <QIcon>

#include "schat.h"
#include "User.h"

class ChatView;

class SCHAT_CORE_EXPORT UserUtils
{
public:
  UserUtils();
  static ClientUser user(const QByteArray &id);
  static ClientUser user(const QUrl &url);
  static ClientUser user(const QVariant &id);
  static QByteArray userId();
  static QIcon icon(ClientUser user, bool status = true, bool offline = false);
  static QString statusTitle(int status);
  static QString toolTip(ClientUser user);
  static QUrl toUrl(ClientUser user, const QString &action = QString());
  static void clear();
  static void updateUserNick(ChatView *view, ClientUser user);

private:
  static QHash<QByteArray, ClientUser> m_users; ///< Оффлайновые пользователи отсутствующие в истории.
  static QStringList m_colors; ///< Цвета.
};

#endif /* USERUTILS_H_ */
