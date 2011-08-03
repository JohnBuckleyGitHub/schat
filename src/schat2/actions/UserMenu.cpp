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
#include <QUrl>

#include "actions/UserMenu.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "ui/UserUtils.h"

UserMenu::UserMenu(ClientUser user, QObject *parent)
  : MenuBuilder(parent)
  , m_self(false)
  , m_user(user)
  , m_ignore(0)
  , m_insert(0)
  , m_talk(0)
{
  init();
}


UserMenu::UserMenu(const QUrl &url, QObject *parent)
  : MenuBuilder(parent)
  , m_self(false)
  , m_ignore(0)
  , m_insert(0)
  , m_talk(0)
{
  QByteArray id = QByteArray::fromHex(url.host().toLatin1()); // FIXME Изменить работу с адресом.
  m_user = UserUtils::user(id);
  if (!m_user) {
    m_user = ClientUser(new User(QByteArray::fromHex(url.path().remove(0, 1).toLatin1())));
    m_user->setId(id);
  }

  init();
}


void UserMenu::insertNick(const QString &nick)
{
  ChatCore::i()->startNotify(ChatCore::InsertTextToSend, QLatin1String(" <b>") + Qt::escape(nick) + QLatin1String("</b> "));
}


void UserMenu::insertNick(const QUrl &url)
{
  QString nick;
  ClientUser user = UserUtils::user(QByteArray::fromHex(url.host().toLatin1())); // FIXME Изменить работу с адресом.
  if (user)
    insertNick(user->nick());
  else
    insertNick(QByteArray::fromHex(url.path().remove(0, 1).toLatin1()));
}


void UserMenu::bind(QMenu *menu)
{
  MenuBuilder::bind(menu);

  if (!m_self && ChatCore::i()->currentId() != m_user->id() && m_user->status() != User::OfflineStatus) {
    m_talk = new QAction(SCHAT_ICON(Balloon), tr("Private Talk"), this);
    menu->addAction(m_talk);
  }

  if (!m_self && (!SCHAT_OPTION("HideIgnore").toBool() || ChatCore::i()->isIgnored(m_user->id()))) {
    m_ignore = new QAction(SCHAT_ICON(Slash), tr("Ignore"), this);
    m_ignore->setCheckable(true);
    m_ignore->setChecked(ChatCore::i()->isIgnored(m_user->id()));
    menu->addAction(m_ignore);
  }

  m_insert = new QAction(tr("Insert Nick"), this);
  menu->addAction(m_insert);
}


void UserMenu::triggered(QAction *action)
{
  if (!m_bind)
    return;

  if (action == m_insert) {
    insertNick(m_user->nick());
  }
  else if (action == m_talk) {
    ChatCore::i()->startNotify(ChatCore::AddPrivateTab, m_user->id());
  }
  else if (action == m_ignore) {
    if (m_ignore->isChecked())
      ChatCore::i()->ignore(m_user->id());
    else
      ChatCore::i()->unignore(m_user->id());
  }
}


void UserMenu::init()
{
  m_self = m_user->id() == UserUtils::userId();
}
