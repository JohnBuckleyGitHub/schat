/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef POPUPMANAGER_P_H_
#define POPUPMANAGER_P_H_

#include <QHash>
#include <QObject>
#include <QQueue>

#include "popupwindow.h"

/*!
 * \brief Приватный D-класс для класса PopupManager.
 */
class PopupManagerPrivate : public QObject
{
  Q_OBJECT

public:
  PopupManagerPrivate();
  void popupMsg(const PopupWindow::Message &message);

  int maxSlots;                          ///< Максимальное число окон.
  int usedSlots;                         ///< Число занятых слотов.
  QHash<QString, PopupWindow *> windows; ///< Созданные окна в качестве ключа используется ник.
  QQueue<PopupWindow::Message> queue;    ///< Очередь сообщений для показа.

signals:
  void freeSlot(int slot);
  void openChat(const QString &nick, bool pub);

public slots:
  void popupClosed(const QString &nick, int slot);
};

#endif /* POPUPMANAGER_P_H_ */
