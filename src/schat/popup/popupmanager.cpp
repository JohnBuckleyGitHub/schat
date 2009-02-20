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
  : QObject(), usedSlot(0)
{
  maxSlots = static_cast<int>(QDesktopWidget().availableGeometry().bottom() / (PopupWindow::Height + PopupWindow::Space));
}


void PopupManagerPrivate::popupMsg(const PopupWindow::Message &message)
{
  usedSlot++;
  PopupWindow *window = new PopupWindow(message);
  window->start(usedSlot);

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


void PopupManager::popupMsg(const QString &nick, const QString &time, const QString &html, bool pub)
{
//  qDebug() << "PopupManager::popupMsg()";
//  qDebug() << nick << time << pub;
//  qDebug() << html;

  if (d->usedSlot < d->maxSlots)
    d->popupMsg(PopupWindow::Message(nick, time, html, pub));
}
