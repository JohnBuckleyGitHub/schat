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

#include <QTimer>

#include "NodeNotify.h"

NodeNotify *NodeNotify::m_self = 0;

NodeNotify::NodeNotify(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


void NodeNotify::start(int type, int subType, const QVariant &param1, const QVariant &param2)
{
  start(new NotifyItem(type, subType, param1, param2));
}


void NodeNotify::start(NotifyItem *notify)
{
  m_self->m_queue.enqueue(NotifyItemPtr(notify));
  if (m_self->m_queue.size() == 1)
    QTimer::singleShot(0, m_self, SLOT(start()));
}


void NodeNotify::start()
{
  while (!m_queue.isEmpty())
    emit notify(*m_queue.dequeue());
}
