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

#include "hooks/PluginsPage.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"

PluginsPage::PluginsPage(QWidget *parent)
  : SettingsPage(SCHAT_ICON(Plugin), LS("plugins"), parent)
{
  retranslateUi();
}


void PluginsPage::retranslateUi()
{
  m_name = tr("Plugins");
}


SettingsPage* PluginsPageCreator::page(QWidget *parent)
{
  return new PluginsPage(parent);
}
