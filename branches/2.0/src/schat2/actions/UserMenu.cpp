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

#include "ChatCore.h"
#include "actions/UserMenu.h"
#include "client/SimpleClient.h"

UserMenu::UserMenu(ClientUser user, QObject *parent)
  : MenuBuilder(parent)
  , m_user(user)
{
  m_self = m_user->id() == ChatCore::i()->client()->userId();
  m_talk = new QAction(SCHAT_ICON(Balloon), tr("Private Talk"), this);
  m_insert = new QAction(tr("Insert Nick"), this);
}


void UserMenu::insertNick(const QString &nick)
{
  ChatCore::i()->startNotify(ChatCore::InsertTextToSend, QLatin1String(" <b>") + Qt::escape(nick) + QLatin1String("</b> "));
}


void UserMenu::bind(QMenu *menu)
{
  MenuBuilder::bind(menu);

  if (!m_self && ChatCore::i()->currentId() != m_user->id()) {
    menu->addAction(m_talk);
  }

  menu->addAction(m_insert);
}


void UserMenu::triggered(QAction *action)
{
  if (action == m_insert) {
    insertNick(m_user->nick());
  }
  else if (action == m_talk) {
    ChatCore::i()->startNotify(ChatCore::AddPrivateTab, m_user->id());
  }
}
