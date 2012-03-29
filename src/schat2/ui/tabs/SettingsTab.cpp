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
#include <QToolButton>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "ChatUrls.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/fields/GenderField.h"
#include "ui/fields/LanguageField.h"
#include "ui/fields/NickEdit.h"
#include "ui/network/NetworkWidget.h"
#include "ui/profile/ProfileLayout.h"
#include "ui/tabs/SettingsTab.h"
#include "ui/tabs/SettingsTab_p.h"

AbstractSettingsPage::AbstractSettingsPage(const QIcon &icon, const QString &id, QWidget *parent)
  : QWidget(parent)
  , m_settings(ChatCore::settings())
  , m_icon(icon)
  , m_id(id)
{
}


AbstractSettingsPage::AbstractSettingsPage(QWidget *parent)
  : QWidget(parent)
  , m_settings(ChatCore::settings())
{
}


ProfilePage::ProfilePage(QWidget *parent)
  : AbstractSettingsPage(SCHAT_ICON(Profile), LS("profile"), parent)
{
  m_profileLabel = new QLabel(this);
  m_nickLabel = new QLabel(this);
  m_nickEdit = new NickEdit(this);

  m_genderLabel = new QLabel(this);
  m_genderField = new GenderField(this);

  m_layout = new ProfileLayout(this);

  QGridLayout *profileLay = new QGridLayout;
  profileLay->addWidget(m_nickLabel, 0, 0);
  profileLay->addWidget(m_nickEdit, 0, 1, 1, 2);
  profileLay->addWidget(m_genderLabel, 1, 0);
  profileLay->addWidget(m_genderField, 1, 1);
  profileLay->addWidget(m_layout->button(), 1, 2);
  profileLay->setContentsMargins(10, 0, 3, 0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_profileLabel);
  mainLay->addLayout(profileLay);
  mainLay->addWidget(m_layout);
  mainLay->addStretch();

  retranslateUi();
}


void ProfilePage::retranslateUi()
{
  m_name = tr("Profile");

  m_profileLabel->setText(LS("<b>") + tr("Profile") + LS("</b>"));
  m_nickLabel->setText(tr("Nick:"));
  m_genderLabel->setText(tr("Gender:"));

  m_layout->retranslateUi();
}


NetworkPage::NetworkPage(QWidget *parent)
  : AbstractSettingsPage(SCHAT_ICON(Globe), LS("network"), parent)
{
  m_networkLabel = new QLabel(this);
  m_network = new NetworkWidget(this);

  QVBoxLayout *networkLay = new QVBoxLayout;
  networkLay->addWidget(m_network);
  networkLay->setContentsMargins(10, 0, 3, 0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_networkLabel);
  mainLay->addLayout(networkLay);
  mainLay->addStretch();
  mainLay->addStretch();

  retranslateUi();
}


void NetworkPage::retranslateUi()
{
  m_name = tr("Network");
  m_networkLabel->setText(LS("<b>") + tr("Network") + LS("</b>"));
}


LocalePage::LocalePage(QWidget *parent)
  : AbstractSettingsPage(SCHAT_ICON(Locale), LS("locale"), parent)
{
  m_localeLabel = new QLabel(this);

  m_language = new LanguageField(this);
  m_language->setMinimumWidth(m_language->width() + 20);

  QHBoxLayout *languageLay = new QHBoxLayout;
  languageLay->addWidget(m_language);
  languageLay->addStretch();
  languageLay->setContentsMargins(10, 0, 3, 0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_localeLabel);
  mainLay->addLayout(languageLay);
  mainLay->addStretch();
  mainLay->addStretch();

  retranslateUi();
}


void LocalePage::retranslateUi()
{
  m_name = tr("Language");
  m_localeLabel->setText(LS("<b>") + tr("Language") + LS("</b>"));
}


SettingsTab::SettingsTab(TabWidget *parent)
  : AbstractTab(QByteArray(), SettingsType, parent)
{
  m_contents = new QListWidget(this);
  m_contents->setSpacing(1);
  m_contents->setFrameShape(QFrame::NoFrame);
  m_apply = new QPushButton(SCHAT_ICON(OK), tr("Apply"), this);
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
  addPage(new LocalePage(this));

  m_contents->setCurrentRow(0);
  setIcon(SCHAT_ICON(Settings));
  setText(tr("Preferences"));

  connect(m_contents, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(pageChanged(QListWidgetItem *, QListWidgetItem*)));
}


void SettingsTab::addPage(AbstractSettingsPage *page)
{
  if (m_ids.contains(page->id()))
    return;

  QListWidgetItem *item = new QListWidgetItem(page->icon(), page->name(), m_contents);
  m_items.append(item);
  m_ids.append(page->id());

  QScrollArea *scroll = new QScrollArea(this);
  scroll->setWidget(page);
  scroll->setWidgetResizable(true);
  scroll->setFrameShape(QFrame::NoFrame);
  m_pages->addWidget(scroll);
}


void SettingsTab::openUrl(const QUrl &url)
{
  if (url.isEmpty() || url.scheme() != LS("chat") || url.host() != LS("settings"))
    return;

  QStringList path = ChatUrls::path(url);
  if (path.isEmpty())
    return;

  int page = m_ids.indexOf(path.at(0));
  if (page != -1)
    m_contents->setCurrentRow(page);
}


void SettingsTab::showEvent(QShowEvent *event)
{
  while (m_contents->horizontalScrollBar()->isVisible())
    m_contents->setMinimumWidth(m_contents->width() + 16);

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
  for (int i = 0; i < m_ids.size(); ++i) {
    AbstractSettingsPage *page = static_cast<AbstractSettingsPage *>(static_cast<QScrollArea *>(m_pages->widget(i))->widget());
    page->retranslateUi();
    m_contents->item(i)->setText(page->name());
  }

  m_apply->setText(tr("Apply"));
  setText(tr("Preferences"));
}
