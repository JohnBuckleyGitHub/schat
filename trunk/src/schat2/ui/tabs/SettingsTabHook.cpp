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

#include "ChatCore.h"
#include "hooks/SettingsTabImpl.h"
#include "ui/tabs/SettingsTabHook.h"

SettingsTabHook *SettingsTabHook::m_self = 0;

SettingsPage::SettingsPage(const QIcon &icon, const QString &id, QWidget *parent)
  : QWidget(parent)
  , m_settings(ChatCore::settings())
  , m_icon(icon)
  , m_id(id)
{
}


SettingsPage::SettingsPage(QWidget *parent)
  : QWidget(parent)
  , m_settings(ChatCore::settings())
{
}


SettingsPage* SettingsPageCreator::page(QWidget */*parent*/)
{
  return 0;
}



SettingsTabHook::SettingsTabHook(QObject *parent)
  : QObject(parent)
{
  m_self = this;

  add(new ProfilePageCreator());
  add(new NetworkPageCreator());
  add(new LocalePageCreator());
}


SettingsTabHook::~SettingsTabHook()
{
  qDeleteAll(m_pages);
}


bool SettingsTabHook::add(SettingsPageCreator *creator)
{
  if (m_self->m_pages.contains(creator->weight())) {
    delete creator;
    return false;
  }

  m_self->m_pages[creator->weight()] = creator;
  return true;
}
