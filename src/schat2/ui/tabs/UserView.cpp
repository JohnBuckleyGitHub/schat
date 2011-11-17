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
#include <QMouseEvent>
#include <QUrl>

#include "actions/UserMenu.h"
#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "debugstream.h"
#include "ui/tabs/UserView.h"
#include "ui/UserUtils.h"

UserItem::UserItem(ClientUser user, int option)
  : QStandardItem(UserUtils::icon(user), user->nick())
  , m_self(false)
  , m_user(user)
{
  if (option & UserView::SelfNick)
    m_self = true;

  if (m_user->status() != Status::Online)
    setColor();

  setSortData();
}


bool UserItem::update()
{
  setText(m_user->nick());
  setColor();
  setSortData();
  setIcon(UserUtils::icon(m_user));
  return true;
}


void UserItem::setColor()
{
  switch (m_user->status()) {
    case Status::Away:
    case Status::AutoAway:
    case Status::DnD:
      setForeground(QBrush(QColor(0x90a4b3)));
      break;

    default:
      setForeground(QPalette().brush(QPalette::WindowText));
      break;
  }
}


void UserItem::setSortData()
{
  setToolTip(UserUtils::toolTip(m_user));

  QString prefix = QLatin1String("6");
  if (m_self)
    prefix = QLatin1String("!");
  else if (m_user->status() == Status::FreeForChat)
    prefix = QLatin1String("4");
  else if (m_user->gender().value() == Gender::Bot)
    prefix = QLatin1String("2");

  setData(prefix + m_user->nick().toLower());
}


UserView::UserView(QWidget *parent)
  : QListView(parent)
  , m_sortable(false)
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
  connect(ChatCore::i()->client(), SIGNAL(userDataChanged(const QByteArray &, int)), SLOT(userDataChanged(const QByteArray &, int)));
}


/*!
 * Добавление пользователя в список.
 *
 * \param user Указатель на пользователя.
 */
bool UserView::add(ClientUser user)
{
  if (!user)
    return false;

  if (m_users.contains(user->id()))
    return false;

  int option = NoOptions;
  if (m_users.isEmpty())
    option = SelfNick;

  UserItem *item = new UserItem(user, option);

  m_model.appendRow(item);
  m_users.insert(user->id(), item);

  if (m_sortable)
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


bool UserView::update(ClientUser user)
{
  UserItem *item = m_users.value(user->id());
  if (!item)
    return false;

  item->update();
  if (m_sortable)
    sort();

  return true;
}


void UserView::clear()
{
  m_sortable = false;
  m_model.clear();
  m_users.clear();
}


void UserView::sort()
{
  SCHAT_DEBUG_STREAM(this << "sort()");

  m_sortable = true;
  m_model.sort(0);
}


void UserView::userDataChanged(const QByteArray &userId, int changed)
{
  UserItem *item = m_users.value(userId);
  if (!item)
    return;

  item->update();
  if (m_sortable && changed & SimpleClient::UserNickChanged)
    sort();
}


void UserView::contextMenuEvent(QContextMenuEvent *event)
{
  QModelIndex index = indexAt(event->pos());

  if (!index.isValid()) {
    QListView::contextMenuEvent(event);
    return;
  }

  UserItem *item = static_cast<UserItem *>(m_model.itemFromIndex(index));
  QMenu menu(this);
  UserMenu userMenu(item->user(), this);
  userMenu.bind(&menu);

  menu.exec(event->globalPos());
}


void UserView::mouseReleaseEvent(QMouseEvent *event)
{
  QModelIndex index = indexAt(event->pos());

  if (index.isValid(), event->modifiers() == Qt::ControlModifier && event->button() == Qt::LeftButton) {
    UserItem *item = static_cast<UserItem *>(m_model.itemFromIndex(index));

    ChatCore::i()->openUrl(UserUtils::toUrl(item->user(), QLatin1String("insert")));
  }
  else if (event->button() == Qt::LeftButton && !index.isValid()) {
    setCurrentIndex(QModelIndex());
  }
  else
    QListView::mouseReleaseEvent(event);
}


/*!
 * Обработка дабл-клика по пользователю в списке,
 * высылается сигнал с идентификатором пользователя.
 */
void UserView::addTab(const QModelIndex &index)
{
  UserItem *item = static_cast<UserItem *>(m_model.itemFromIndex(index));
  ChatCore::i()->startNotify(ChatCore::AddPrivateTab, item->user()->id());
}
