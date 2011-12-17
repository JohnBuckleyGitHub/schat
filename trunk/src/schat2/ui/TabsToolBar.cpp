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
#include <QToolButton>
#include <QEvent>

#include "ui/ChatIcons.h"
#include "ui/tabs/AbstractTab.h"
#include "ui/tabs/AlertTab.h"
#include "ui/TabsToolBar.h"
#include "ui/TabWidget.h"

TabsToolBar::TabsToolBar(QWidget *parent)
  : QToolBar(parent)
{
  setIconSize(QSize(16, 16));

  m_menu = new QMenu(this);

  m_button = new QToolButton(this);
  m_button->setIcon(SCHAT_ICON(MainTabMenu));
  m_button->setAutoRaise(true);
  m_button->setPopupMode(QToolButton::InstantPopup);
  m_button->setToolTip(tr("Menu"));
  m_button->setMenu(m_menu);

  m_channels = new QMenu(tr("Channels"), this);
  m_channels->setIcon(SCHAT_ICON(Channel));

  m_talks = new QMenu(tr("Talks"), this);
  m_talks->setIcon(SCHAT_ICON(Users));

  addWidget(m_button);

  connect(m_menu, SIGNAL(aboutToHide()), SLOT(hideMenu()));
  connect(m_menu, SIGNAL(aboutToShow()), SLOT(showMenu()));
}


void TabsToolBar::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QToolBar::changeEvent(event);
}


void TabsToolBar::hideMenu()
{
  m_menu->clear();
  m_channels->clear();
  m_talks->clear();
}


void TabsToolBar::showMenu()
{
  QList<QAction *> channels;
  QList<QAction *> talks;

  TabWidget *tabs = TabWidget::i();
  AbstractTab *currentTab = tabs->widget(tabs->currentIndex());

  for (int i = 0; i < tabs->count(); ++i) {
    AbstractTab *tab = tabs->widget(i);
    if (tab->type() == AbstractTab::ChannelType)
      channels.append(tab->action());
    else if (tab->type() == AbstractTab::PrivateType)
      talks.append(tab->action());

    tab->action()->setChecked(currentTab == tab);
  }

  if (!channels.isEmpty()) {
    m_channels->addActions(channels);
    m_menu->addMenu(m_channels);
  }

  if (!talks.isEmpty()) {
    m_talks->addActions(talks);
    m_menu->addMenu(m_talks);
  }

  m_menu->addSeparator();
  m_menu->addAction(tabs->alertTab()->action());
  tabs->alertTab()->action()->setChecked(currentTab == tabs->alertTab());
}


void TabsToolBar::retranslateUi()
{
  m_button->setToolTip(tr("Menu"));
  m_channels->setTitle(tr("Channels"));
  m_talks->setTitle(tr("Talks"));
}
