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

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "NetworkManager.h"
#include "ui/fields/GenderField.h"
#include "ui/fields/LanguageField.h"
#include "ui/fields/NickEdit.h"
#include "ui/network/NetworkEditor.h"
#include "ui/tabs/WelcomeTab.h"
#include "User.h"

WelcomeTab::WelcomeTab(TabWidget *parent)
  : AbstractTab(QByteArray(), WelcomeType, parent)
  , m_languageBox(0)
  , m_languageLabel(0)
{
  m_networkLabel = new QLabel(this);
  m_network = new NetworkEditor(this, NetworkEditor::ConnectButtonLayout);
  QGridLayout *networkLay = new QGridLayout;
  networkLay->addWidget(m_network, 0, 0, 1, 2);
  networkLay->setContentsMargins(20, 0, 3, 6);
  networkLay->setColumnStretch(0, 1);

  QGridLayout *languageLay = 0;
  if (!ChatCore::i()->networks()->count()) {
    m_languageLabel = new QLabel(this);
    m_languageBox = new LanguageField(this);
    languageLay = new QGridLayout;
    languageLay->addWidget(m_languageBox);
    languageLay->setContentsMargins(20, 0, 3, 6);
  }

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
  profileLay->setContentsMargins(20, 0, 3, 6);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_networkLabel, 0, 0);

  if (m_languageLabel)
    mainLay->addWidget(m_languageLabel, 0, 1);

  mainLay->addLayout(networkLay, 1, 0);

  if (languageLay)
    mainLay->addLayout(languageLay, 1, 1, Qt::AlignTop);

  mainLay->addWidget(m_profileLabel, 2, 0);
  mainLay->addLayout(profileLay, 3, 0);
  mainLay->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 4, 0);

  setIcon(SCHAT_ICON(SmallLogoIcon));
  retranslateUi();
}


void WelcomeTab::retranslateUi()
{
  setText(tr("Welcome"));
  m_networkLabel->setText(QLatin1String("<b>") + tr("Network") + QLatin1String("</b>"));
  m_profileLabel->setText(QLatin1String("<b>") + tr("Profile") + QLatin1String("</b>"));
  m_nickLabel->setText(tr("Nick:"));
  m_genderLabel->setText(tr("Gender:"));

  if (m_languageLabel)
    m_languageLabel->setText(QLatin1String("<b>") + tr("Language") + QLatin1String("</b>"));
}
