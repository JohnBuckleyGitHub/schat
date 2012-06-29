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

#include <QLabel>
#include <QVBoxLayout>

#include "hooks/PluginsPage.h"
#include "plugins/PluginsView.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"

PluginsPage::PluginsPage(QWidget *parent)
  : SettingsPage(SCHAT_ICON(Plugin), LS("plugins"), parent)
{
  m_label = new QLabel(this);
  m_view = new PluginsView(this);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_label);
  mainLay->addWidget(m_view);
  mainLay->setStretch(0, 0);
  mainLay->setStretch(1, 1);

  QMargins margins = mainLay->contentsMargins();
  margins.setBottom(0);
  mainLay->setContentsMargins(margins);

  retranslateUi();
}


void PluginsPage::retranslateUi()
{
  m_name = tr("Plugins");
  m_label->setText(LS("<b>") + m_name + LS("</b>"));
}


SettingsPage* PluginsPageCreator::page(QWidget *parent)
{
  return new PluginsPage(parent);
}
