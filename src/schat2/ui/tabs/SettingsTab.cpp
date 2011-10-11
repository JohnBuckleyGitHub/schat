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
#include "ui/fields/GenderField.h"

#if defined(SCHAT_OPTION)
  #undef SCHAT_OPTION
  #define SCHAT_OPTION(x) m_settings->value(ChatSettings::x)
#endif

AbstractSettingsPage::AbstractSettingsPage(const QIcon &icon, const QString &id, QWidget *parent)
  : QWidget(parent)
  , m_settings(ChatCore::i()->settings())
  , m_icon(icon)
  , m_id(id)
{
}


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
  : AbstractSettingsPage(SCHAT_ICON(ProfileIcon), "profile", parent)
{
  m_profileLabel = new QLabel(this);
  m_nickLabel = new QLabel(this);
  m_nickEdit = new NickEdit(this);

  m_genderLabel = new QLabel(this);
  m_genderField = new GenderField(this);

  QGridLayout *profileLay = new QGridLayout;
  profileLay->addWidget(m_nickLabel, 0, 0);
  profileLay->addWidget(m_nickEdit, 0, 1);
  profileLay->addWidget(m_genderLabel, 1, 0);
  profileLay->addWidget(m_genderField, 1, 1);
  profileLay->setContentsMargins(20, 0, 3, 16);

  m_networkLabel = new QLabel(this);
  m_networks = new NetworkWidget(this);

  QVBoxLayout *networkLay = new QVBoxLayout;
  networkLay->addWidget(m_networks);
  networkLay->setContentsMargins(20, 0, 3, 6);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_profileLabel);
  mainLay->addLayout(profileLay);
  mainLay->addWidget(m_networkLabel);
  mainLay->addLayout(networkLay);
  mainLay->addStretch();

  retranslateUi();
}


void ProfilePage::retranslateUi()
{
  m_name = tr("Profile");

  m_profileLabel->setText(QLatin1String("<b>") + tr("Profile") + QLatin1String("</b>"));
  m_nickLabel->setText(tr("Nick:"));
  m_genderLabel->setText(tr("Gender:"));
  m_networkLabel->setText(QLatin1String("<b>") + tr("Network") + QLatin1String("</b>"));
}


NetworkPage::NetworkPage(QWidget *parent)
  : AbstractSettingsPage(SCHAT_ICON(GlobeIcon), "network", parent)
{
  retranslateUi();
}


void NetworkPage::retranslateUi()
{
  m_name = tr("Network");
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

  addPage(new ProfilePage(this));
  addPage(new NetworkPage(this));

  m_contents->setCurrentRow(0);
  setIcon(SCHAT_ICON(SettingsIcon));
  setText(tr("Preferences"));

  connect(m_contents, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(pageChanged(QListWidgetItem *, QListWidgetItem*)));
}


void SettingsTab::addPage(AbstractSettingsPage *page)
{
  QListWidgetItem *item = new QListWidgetItem(page->icon(), page->name(), m_contents);
  m_items.append(item);

  QScrollArea *scrool = new QScrollArea(this);
  scrool->setWidget(page);
  scrool->setWidgetResizable(true);
  scrool->setFrameShape(QFrame::NoFrame);
  m_pages->addWidget(scrool);
}


void SettingsTab::showEvent(QShowEvent *event)
{
  while (m_contents->horizontalScrollBar()->isVisible()) {
    m_contents->setMinimumWidth(m_contents->width() + 16);
  }

  AbstractTab::showEvent(event);
}


void SettingsTab::pageChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
    current = previous;

  m_pages->setCurrentIndex(m_contents->row(current));
}


void SettingsTab::retranslateUi()
{
  QListWidgetItem *item = m_contents->item(0);
  item->setText(tr("Profile"));

  m_apply->setText(tr("Apply"));
  setText(tr("Preferences"));
}
