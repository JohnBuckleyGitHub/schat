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

#include <QDebug>

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
  m_self = m_user->id() == UserUtils::userId();
}


UserMenu *UserMenu::bind(QMenu *menu, const QVariant &id)
{
  ClientUser user = UserUtils::user(id);
  if (!user)
    return 0;

  UserMenu *out = new UserMenu(user, menu);
  out->bind(menu);

  return out;
}


void UserMenu::bind(QMenu *menu)
{
  MenuBuilder::bind(menu);

  if (!m_self && ChatCore::i()->currentId() != m_user->id()) {
    m_talk = new QAction(SCHAT_ICON(Balloon), tr("Talk..."), this);
    m_talk->setData(UserUtils::toUrl(m_user, QLatin1String("talk")));
    menu->addAction(m_talk);
  }

  if (!m_self && (!SCHAT_OPTION("HideIgnore").toBool() || ChatCore::i()->isIgnored(m_user->id()))) {
    m_ignore = new QAction(SCHAT_ICON(Slash), tr("Ignore"), this);
    m_ignore->setCheckable(true);
    m_ignore->setChecked(ChatCore::i()->isIgnored(m_user->id()));
    m_ignore->setData(UserUtils::toUrl(m_user, m_ignore->isChecked() ? QLatin1String("unignore") : QLatin1String("ignore")));

    menu->addAction(m_ignore);
  }

  m_insert = new QAction(tr("Insert Nick"), this);
  m_insert->setData(UserUtils::toUrl(m_user, QLatin1String("insert")));
  menu->addAction(m_insert);
}
