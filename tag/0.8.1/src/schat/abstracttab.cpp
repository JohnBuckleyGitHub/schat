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

#include "abstracttab.h"
#include "channellog.h"

/*!
 * \brief Конструктор класса AbstractTab.
 */
AbstractTab::AbstractTab(Type type, const QIcon &icon, QWidget *parent, bool text)
  : QWidget(parent),
  m_notice(false),
  m_view(0),
  m_icon(icon),
  m_type(type)
{
  setAttribute(Qt::WA_DeleteOnClose);

  if (text) {
    m_view = new ChatView(this);
    connect(m_view, SIGNAL(emoticonsClicked(const QString &)), SIGNAL(emoticonsClicked(const QString &)));
    connect(m_view, SIGNAL(nickClicked(const QString &)), SIGNAL(nickClicked(const QString &)));
    connect(m_view, SIGNAL(popupMsg(const QString &, const QString &, const QString &, bool)), SIGNAL(popupMsg(const QString &, const QString &, const QString &, bool)));
  }
}
