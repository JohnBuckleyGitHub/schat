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
#include <QTextDocument>

#include "actions/UserMenu.h"
#include "ChatCore.h"
#include "client/SimpleClient.h"

UserMenu::UserMenu(ClientUser user, QObject *parent)
  : MenuBuilder(parent)
  , m_self(false)
  , m_user(user)
{
  init();
}


UserMenu::UserMenu(const QUrl &url, QObject *parent)
  : MenuBuilder(parent)
  , m_self(false)
{
  m_user = ChatCore::i()->client()->user(QByteArray::fromHex(url.host().toLatin1()));
  if (!m_user)
    m_nick = QByteArray::fromHex(url.path().remove(0, 1).toLatin1());

  init();
}


void UserMenu::insertNick(const QString &nick)
{
  ChatCore::i()->startNotify(ChatCore::InsertTextToSend, QLatin1String(" <b>") + Qt::escape(nick) + QLatin1String("</b> "));
}


void UserMenu::insertNick(const QUrl &url)
{
  QString nick;
  ClientUser user = ChatCore::i()->client()->user(QByteArray::fromHex(url.host().toLatin1()));
  if (user)
    insertNick(user->nick());
  else
    insertNick(QByteArray::fromHex(url.path().remove(0, 1).toLatin1()));
}


void UserMenu::bind(QMenu *menu)
{
  MenuBuilder::bind(menu);

  if (m_user && !m_self && ChatCore::i()->currentId() != m_user->id() && m_user->status() != User::OfflineStatus) {
    menu->addAction(m_talk);
  }

  menu->addAction(m_insert);
}


void UserMenu::triggered(QAction *action)
{
  if (action == m_insert) {
    if (m_user)
      insertNick(m_user->nick());
    else
      insertNick(m_nick);
  }
  else if (action == m_talk) {
    ChatCore::i()->startNotify(ChatCore::AddPrivateTab, m_user->id());
  }
}


void UserMenu::init()
{
  if (m_user) {
    m_self = m_user->id() == ChatCore::i()->client()->userId();
    m_talk = new QAction(SCHAT_ICON(Balloon), tr("Private Talk"), this);
  }

  m_insert = new QAction(tr("Insert Nick"), this);
}
