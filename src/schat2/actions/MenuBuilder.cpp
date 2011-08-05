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

#include <QMenu>
#include <QUrl>

#include "actions/MenuBuilder.h"
#include "ChatCore.h"

MenuBuilder::MenuBuilder(QObject *parent)
  : QObject(parent)
  , m_bind(false)
  , m_menu(0)
{
}


void MenuBuilder::bind(QMenu *menu)
{
  m_bind = true;
  m_menu = menu;
  connect(m_menu, SIGNAL(triggered(QAction *)), SLOT(triggered(QAction *)));
}


void MenuBuilder::triggered(QAction *action)
{
  if (!m_bind)
    return;

  if (action->data().type() == QVariant::Url) {
    ChatCore::i()->openUrl(action->data().toUrl());
    return;
  }
}
