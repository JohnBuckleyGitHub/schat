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

#include <QtGui>

#include "popupmanager.h"
#include "popupmanager_p.h"

/*!
 * Конструктор класса PopupManagerPrivate.
 */
PopupManagerPrivate::PopupManagerPrivate()
  : QObject(), usedSlots(0)
{
  maxSlots = static_cast<int>(QDesktopWidget().availableGeometry().bottom() / (PopupWindow::Height + PopupWindow::Space));
}


/*!
 * Создание всплывающего окна.
 */
void PopupManagerPrivate::popupMsg(const PopupWindow::Message &message)
{
  usedSlots++;
  PopupWindow *window = new PopupWindow(message);
  connect(window, SIGNAL(aboutToClose(const QString &, int)), SLOT(popupClosed(const QString &, int)));
  connect(this, SIGNAL(freeSlot(int)), window, SLOT(freeSlot(int)));

  window->start(usedSlots);
  if (!windows.contains(message.nick))
    windows.insert(message.nick, window);
}


/*!
 * Обработка закрытия всплывающего окна.
 * Если очередь сообщений не пуста, то создаются новое окно.
 */
void PopupManagerPrivate::popupClosed(const QString &nick, int slot)
{
  usedSlots--;
  emit freeSlot(slot);

  if (!queue.isEmpty())
    popupMsg(queue.dequeue());
}


/*!
 * Конструктор класса PopupManager.
 */
PopupManager::PopupManager(QObject *parent)
  : QObject(parent), d(new PopupManagerPrivate)
{
}


PopupManager::~PopupManager()
{
  delete d;
}


/*!
 * Получение запроса на создание нового окна.
 * Если есть свободные слоты, то сообщение сразу же отображается,
 * иначе оно добавляется в очередь.
 */
void PopupManager::popupMsg(const QString &nick, const QString &time, const QString &html, bool pub)
{
  PopupWindow::Message message(nick, time, html, pub);

  if (d->usedSlots < d->maxSlots)
    d->popupMsg(message);
  else
    d->queue.enqueue(message);
}
