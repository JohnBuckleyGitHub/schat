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

#include <QMenu>
#include <QMouseEvent>
#include <QUrl>

#include "ChatCore.h"
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "debugstream.h"
#include "hooks/ChannelMenu.h"
#include "hooks/MessagesImpl.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/tabs/UserView.h"

UserItem::UserItem(ClientChannel user, ClientChannel channel)
  : QStandardItem()
  , m_self(false)
  , m_channel(channel)
  , m_user(user)
{
  m_self = ChatClient::id() == user->id();

  reload();
}


UserItem::~UserItem()
{
  qDebug() << this << "~" << text();
}


/*!
 * Обновление информации.
 */
bool UserItem::reload()
{
  setText(m_user->name());
  setForeground(colorize());
  setData(prefix() + m_user->name().toLower());
  setIcon(ChatIcons::icon(m_user));

  FeedPtr feed = m_channel->feed(LS("acl"), false);
  if (feed) {
    QFont font = this->font();
    int acl    = feed->head().acl().match(m_user.data());

    font.setBold(acl & Acl::Edit);
    font.setItalic(!(acl & Acl::Write) || Hooks::MessagesImpl::ignored(m_user));
    setFont(font);
  }

  return true;
}


/*!
 * Возвращает необходимый цвет текста.
 */
QBrush UserItem::colorize() const
{
  switch (m_user->status().value()) {
    case Status::Away:
    case Status::AutoAway:
    case Status::DnD:
      return QColor(0x90a4b3);
      break;

    default:
      return QPalette().brush(QPalette::WindowText);
      break;
  }
}


/*!
 * Префикс для сортировки.
 */
QString UserItem::prefix() const
{
  if (m_self)
    return LS("!");

  else if (m_user->status().value() == Status::FreeForChat)
    return LS("5");

  else if (m_user->gender().value() == Gender::Bot)
    return LS("3");

  return LS("7");
}


UserView::UserView(ClientChannel channel, QWidget *parent)
  : QListView(parent)
  , m_sortable(true)
  , m_channel(channel)
{
  setModel(&m_model);
  setFocusPolicy(Qt::TabFocus);
  setEditTriggers(QListView::NoEditTriggers);
  setSpacing(1);
  setFrameShape(QFrame::NoFrame);

  setTextElideMode(Qt::ElideMiddle);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setStyleSheet(LS("QScrollBar:vertical{background:transparent;width:6px}QScrollBar::handle:vertical{background:#ccc;border-radius:3px}QScrollBar::sub-line:vertical,QScrollBar::add-line:vertical{height:0}"));

  QPalette p = palette();
  if (p.color(QPalette::Base) == Qt::white) {
    setAlternatingRowColors(true);
    p.setColor(QPalette::AlternateBase, QColor(247, 250, 255));
    setPalette(p);
  }

  m_model.setSortRole(Qt::UserRole + 1);

  connect(this, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(addTab(const QModelIndex &)));
  connect(ChatClient::channels(), SIGNAL(channel(const ChannelInfo &)), SLOT(channel(const ChannelInfo &)));
}


/*!
 * Добавление канала-пользователя в список.
 *
 * \param channel Указатель на канал.
 */
bool UserView::add(ClientChannel user)
{
  if (!user)
    return false;

  if (m_channels.contains(user->id()))
    return reload(user);

  UserItem *item = new UserItem(user, m_channel);

  m_model.appendRow(item);
  m_channels[user->id()] = item;

  if (m_sortable)
    sort();

  return true;
}


bool UserView::reload(ClientChannel channel)
{
  UserItem *item = m_channels.value(channel->id());
  if (!item)
    return false;

  item->reload();
  if (m_sortable)
    sort();

  return true;
}


/*!
 * Удаление пользователя из списка.
 */
bool UserView::remove(const QByteArray &id)
{
  UserItem *item = m_channels.value(id);

  if (!item)
    return false;

  m_model.removeRow(m_model.indexFromItem(item).row());
  m_channels.remove(id);
  return true;
}


void UserView::clear()
{
  m_sortable = true;
  m_model.clear();
  m_channels.clear();
}


void UserView::sort()
{
  SCHAT_DEBUG_STREAM(this << "sort()");

  m_sortable = true;
  m_model.sort(0);
}


void UserView::channel(const ChannelInfo &info)
{
  UserItem *item = m_channels.value(info.id());
  if (!item)
    return;

  item->reload();

  if (info.option() == ChannelInfo::Renamed)
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
  Hooks::ChannelMenu::bind(&menu, item->user(), Hooks::UserViewScope);
  menu.exec(event->globalPos());
}


void UserView::mouseReleaseEvent(QMouseEvent *event)
{
  QModelIndex index = indexAt(event->pos());

  if (index.isValid() && event->modifiers() == Qt::ControlModifier && event->button() == Qt::LeftButton) {
    UserItem *item = static_cast<UserItem *>(m_model.itemFromIndex(index));
    ChatUrls::open(ChatUrls::toUrl(item->user(), LS("insert")));
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
  QUrl url = ChatUrls::toUrl(static_cast<UserItem *>(m_model.itemFromIndex(index))->user(), LS("open"));
  ChatUrls::open(url);
}
