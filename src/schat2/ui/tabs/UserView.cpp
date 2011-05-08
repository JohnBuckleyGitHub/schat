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

#include "debugstream.h"
#include "ui/tabs/UserView.h"
#include "User.h"

UserItem::UserItem(User *user, int option)
  : QStandardItem(user->nick())
  , m_user(new User(user))
{
  if (option & UserView::SelfNick)
    setData("!" + user->nick().toLower());
  else
    setData("5" + user->nick().toLower());
}


UserItem::~UserItem()
{
  delete m_user;
}


UserView::UserView(QWidget *parent)
  : QListView(parent)
{
  setModel(&m_model);
  setFocusPolicy(Qt::TabFocus);
  setEditTriggers(QListView::NoEditTriggers);
  setSpacing(1);
  setFrameShape(QFrame::NoFrame);

  QPalette p = palette();
  if (p.color(QPalette::Base) == Qt::white) {
    setAlternatingRowColors(true);
    p.setColor(QPalette::AlternateBase, QColor(247, 250, 255));
    setPalette(p);
  }

  m_model.setSortRole(Qt::UserRole + 1);

  connect(this, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(addTab(const QModelIndex &)));
}


/*!
 * Добавление пользователя в список.
 *
 * \param user   Указатель на пользователя.
 * \param option \sa AddOptions.
 */
bool UserView::add(User *user, int option)
{
  if (!user) {
    sort();
    return false;
  }

  if (m_users.contains(user->id()))
    return false;

  UserItem *item = new UserItem(user, option);
  m_model.appendRow(item);
  m_users.insert(user->id(), item);

  if (!(option & NoSort))
    sort();

  return true;
}


/*!
 * Удаление пользователя из списка.
 */
bool UserView::remove(const QByteArray &id)
{
  UserItem *item = m_users.value(id);

  if (!item)
    return false;

  m_model.removeRow(m_model.indexFromItem(item).row());
  m_users.remove(id);
  return true;
}


void UserView::clear()
{
  m_model.clear();
  m_users.clear();
}


/*!
 * Обработка дабл-клика по пользователю в списке,
 * высылается сигнал с идентификатором пользователя.
 */
void UserView::addTab(const QModelIndex &index)
{
  UserItem *item = static_cast<UserItem *>(m_model.itemFromIndex(index));
  emit addTab(item->user()->id());
}


void UserView::sort()
{
  SCHAT_DEBUG_STREAM(this << "sort()");

  m_model.sort(0);
}
