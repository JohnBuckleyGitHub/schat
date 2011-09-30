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

#ifndef CHATCORE_P_H_
#define CHATCORE_P_H_


class ChatCorePrivate
{
public:
  ChatCorePrivate();
  ~ChatCorePrivate();

  QStringList urlAction(const QUrl &url);
  void ignore(const QByteArray &id);
  void loadIgnoreList();
  void openChannelUrl(const QUrl &url);
  void openUserUrl(const QUrl &url);
  void unignore(const QByteArray &id);
  void writeIgnoreList();

  ChatCore *q;                    ///< Указатель на объект ChatCore.
  static QStringList icons;       ///< Иконки.
  UserUtils *userUtils;           ///< Утилиты манипуляции над пользователем.
  QList<QByteArray> ignoreList;   ///< Чёрный список.
};


#endif /* CHATCORE_P_H_ */
