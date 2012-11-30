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
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/tabs/UserView.h"

UserItem::UserItem(ClientChannel channel)
  : QStandardItem(ChatIcons::icon(channel), channel->name())
  , m_self(false)
  , m_channel(channel)
{
  if (ChatClient::channel()->id() == channel->id())
    m_self = true;

  if (m_channel->status().value() != Status::Online)
    setColor();

  setSortData();
}


bool UserItem::reload()
{
  setText(m_channel->name());
  setColor();
  setSortData();
  setIcon(ChatIcons::icon(m_channel));
  return true;
}


void UserItem::setColor()
{
  switch (m_channel->status().value()) {
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
  QString prefix = LS("6");
  if (m_self)
    prefix = LS("!");
  else if (m_channel->status().value() == Status::FreeForChat)
    prefix = LS("4");
  else if (m_channel->gender().value() == Gender::Bot)
    prefix = LS("2");

  setData(prefix + m_channel->name().toLower());
}


UserView::UserView(QWidget *parent)
  : QListView(parent)
  , m_sortable(true)
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
bool UserView::add(ClientChannel channel)
{
  if (!channel)
    return false;

  if (m_channels.contains(channel->id()))
    return reload(channel);

  UserItem *item = new UserItem(channel);

  m_model.appendRow(item);
  m_channels[channel->id()] = item;

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
  Hooks::ChannelMenu::bind(&menu, item->channel(), Hooks::UserViewScope);
  menu.exec(event->globalPos());
}


void UserView::mouseReleaseEvent(QMouseEvent *event)
{
  QModelIndex index = indexAt(event->pos());

  if (index.isValid() && event->modifiers() == Qt::ControlModifier && event->button() == Qt::LeftButton) {
    UserItem *item = static_cast<UserItem *>(m_model.itemFromIndex(index));
    ChatUrls::open(ChatUrls::toUrl(item->channel(), LS("insert")));
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
  QUrl url = ChatUrls::toUrl(static_cast<UserItem *>(m_model.itemFromIndex(index))->channel(), "open");
  ChatUrls::open(url);
}
