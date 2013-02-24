/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QDesktopWidget>
#include <QFile>
#include <QTimer>

#include "popupmanager.h"
#include "popupmanager_p.h"

/*!
 * Конструктор класса PopupManagerPrivate.
 */
PopupManagerPrivate::PopupManagerPrivate()
  : QObject(), normal(false), styleSheetsReaded(false), usedSlots(0)
{
  maxSlots = static_cast<int>(QDesktopWidget().availableGeometry().bottom() / (PopupWindow::Height + PopupWindow::Space));

  timer = new QTimer(this);
  timer->setInterval(1500);
  connect(timer, SIGNAL(timeout()), SLOT(flash()));
}


PopupManagerPrivate::~PopupManagerPrivate()
{
  if (!windows.isEmpty())
    qDeleteAll(windows);
}


/*!
 * Создание всплывающего окна.
 */
void PopupManagerPrivate::popupMsg(const PopupWindow::Message &message)
{
  if (!usedSlots) {
    if (!styleSheetsReaded)
      readStyleSheets();

    timer->start();
  }

  if (!windows.contains(message.nick)) {
    usedSlots++;
    PopupWindow *window = new PopupWindow(message);
    connect(window, SIGNAL(aboutToClose(const QString &, int)), SLOT(popupClosed(const QString &, int)));
    connect(window, SIGNAL(openChat(const QString &, bool, bool)), SIGNAL(openChat(const QString &, bool, bool)));
    connect(this, SIGNAL(freeSlot(int)), window, SLOT(freeSlot(int)));
    connect(this, SIGNAL(flash(const QString &)), window, SLOT(flash(const QString &)));

    if (normal)
      window->flash(normalStyle);
    else
      window->flash(flashStyle);

    window->start(usedSlots);

    windows.insert(message.nick, window);
  }
  else
    windows.value(message.nick)->setMessage(message);
}


/*!
 * Отложенное чтение стилей всплывающих окон.
 */
void PopupManagerPrivate::readStyleSheets()
{
  QFile file;
  file.setFileName(":/css/popupwindow-normal.css");
  if (file.open(QFile::ReadOnly)) {
    normalStyle = QLatin1String(file.readAll());
    file.close();
  }

  file.setFileName(":/css/popupwindow-flash.css");
  if (file.open(QFile::ReadOnly)) {
    flashStyle = QLatin1String(file.readAll());
    file.close();
  }

  styleSheetsReaded = true;
}


/*!
 * Изменение стиля по таймеру для создания эффекта мигания окна.
 */
void PopupManagerPrivate::flash()
{
  normal = !normal;

  if (normal)
    emit flash(normalStyle);
  else
    emit flash(flashStyle);
}


/*!
 * Обработка закрытия всплывающего окна.
 * Если очередь сообщений не пуста, то создаются новое окно.
 */
void PopupManagerPrivate::popupClosed(const QString &nick, int slot)
{
  usedSlots--;
  if (windows.contains(nick))
    windows.remove(nick);

  emit freeSlot(slot);

  if (!queue.isEmpty())
    popupMsg(queue.dequeue());
  else if (!usedSlots) {
    timer->stop();
    normal = false;
  }
}


/*!
 * Конструктор класса PopupManager.
 */
PopupManager::PopupManager(QObject *parent)
  : QObject(parent), d(new PopupManagerPrivate)
{
  connect(d, SIGNAL(openChat(const QString &, bool, bool)), SIGNAL(openChat(const QString &, bool, bool)));
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
