/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include "ChatAlerts.h"
#include "PopupManager.h"
#include "ui/TabWidget.h"
#include "PopupWindow.h"

PopupManager::PopupManager(QObject *parent)
  : QObject(parent)
{
  m_desktop = new QDesktopWidget();

  connect(ChatAlerts::i(), SIGNAL(popup(Alert)), SLOT(popup(Alert)));
}


PopupManager::~PopupManager()
{
  delete m_desktop;
}


void PopupManager::popup(const Alert &alert)
{
  PopupWindow *window = new PopupWindow(alert);
  connect(window, SIGNAL(destroyed(QObject*)), SLOT(windowDestroyed(QObject*)));

  m_windows.prepend(window);
  layoutWidgets();
}


void PopupManager::windowDestroyed(QObject *obj)
{
  m_windows.removeAll(static_cast<PopupWindow *>(obj));
  layoutWidgets();
}


void PopupManager::layoutWidgets()
{
  QRect display = m_desktop->availableGeometry(TabWidget::i());
  int bottom = display.bottom() - PopupWindow::Space;
  foreach (PopupWindow *window, m_windows) {
    if (!window->isVisible())
      window->show();

    QRect geometry = window->frameGeometry();
    window->move(display.right() - geometry.width(), bottom - geometry.height());
    bottom -= geometry.height() + PopupWindow::Space;
  }
}
