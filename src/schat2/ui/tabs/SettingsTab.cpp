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

#include <QCheckBox>
#include <QDebug>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStackedWidget>

#include "ChatCore.h"
#include "ui/NetworkWidget.h"
#include "ui/fields/NickEdit.h"
#include "ui/tabs/SettingsTab.h"
#include "ui/tabs/SettingsTab_p.h"
#include "User.h"
#include "ChatSettings.h"

#if defined(SCHAT_OPTION)
  #undef SCHAT_OPTION
  #define SCHAT_OPTION(x) m_settings->value(ChatSettings::x)
#endif

AbstractSettingsPage::AbstractSettingsPage(QWidget *parent)
  : QWidget(parent)
  , m_settings(ChatCore::i()->settings())
{
}


void AbstractSettingsPage::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


ProfilePage::ProfilePage(QWidget *parent)
  : AbstractSettingsPage(parent)
{
  m_profileLabel = new QLabel("<b>" + tr("Default profile") + "</b>", this);
  m_nickLabel = new QLabel(tr("Nick:"), this);
  m_nickEdit = new NickEdit(this);

  QGridLayout *profileLay = new QGridLayout;
  profileLay->addWidget(m_nickLabel, 0, 0);
  profileLay->addWidget(m_nickEdit, 0, 1);
  profileLay->setContentsMargins(20, 0, 3, 16);

  m_networkLabel = new QLabel("<b>" + tr("Network") + "</b>", this);
  m_networks = new NetworkWidget(this);
  m_defaultProfile = new QCheckBox(tr("Use default profile"), this);
  m_defaultProfile->setChecked(SCHAT_OPTION(DefaultProfile).toBool());
  m_defaultProfile->setVisible(false);

  QVBoxLayout *networkLay = new QVBoxLayout;
  networkLay->addWidget(m_networks);
  networkLay->addWidget(m_defaultProfile);
  networkLay->setContentsMargins(20, 0, 3, 6);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_profileLabel);
  mainLay->addLayout(profileLay);
  mainLay->addWidget(m_networkLabel);
  mainLay->addLayout(networkLay);
  mainLay->addStretch();
}


SettingsTab::SettingsTab(TabWidget *parent)
  : AbstractTab(QByteArray(), SettingsType, parent)
{
  m_contents = new QListWidget(this);
  m_contents->setSpacing(1);
  m_contents->setFrameShape(QFrame::NoFrame);
  m_apply = new QPushButton(SCHAT_ICON(OkIcon), tr("Apply"), this);
  m_apply->setVisible(false);
  m_pages = new QStackedWidget(this);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_contents, 0, 0);
  mainLay->addWidget(m_apply, 1, 0);
  mainLay->addWidget(m_pages, 0, 1, 2, 1);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);
  mainLay->setColumnStretch(0, 1);
  mainLay->setColumnStretch(1, 4);

  addPage(SCHAT_ICON(ProfileIcon), tr("Profile"), new ProfilePage(this));

  m_contents->setCurrentRow(0);
  m_icon = SCHAT_ICON(SettingsIcon);
}


void SettingsTab::showEvent(QShowEvent *event)
{
  while (m_contents->horizontalScrollBar()->isVisible()) {
    m_contents->setMinimumWidth(m_contents->width() + 16);
  }

  AbstractTab::showEvent(event);
}


void SettingsTab::addPage(const QIcon &icon, const QString &text, AbstractSettingsPage *page)
{
  QListWidgetItem *item = new QListWidgetItem(icon, text, m_contents);
  m_items.append(item);

  QScrollArea *scrool = new QScrollArea(this);
  scrool->setWidget(page);
  scrool->setWidgetResizable(true);
  scrool->setFrameShape(QFrame::NoFrame);
  m_pages->addWidget(scrool);
}
