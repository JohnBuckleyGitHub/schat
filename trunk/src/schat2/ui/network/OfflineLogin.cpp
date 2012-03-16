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
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include "NetworkManager.h"
#include "ui/network/OfflineLogin.h"

OfflineLogin::OfflineLogin(QWidget *parent)
  : NetworkExtra(parent)
{
  m_anonymous = new QCheckBox(this);

  m_nameEdit = new QLineEdit(this);
  m_nameEdit->setMaxLength(255);
  m_nameLabel = new QLabel(this);
  m_nameLabel->setBuddy(m_nameEdit);

  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);
  m_passwordEdit->setMaxLength(255);
  m_passwordLabel = new QLabel(this);
  m_passwordLabel->setBuddy(m_passwordEdit);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_anonymous, 0, 0, 1, 4);
  mainLay->addWidget(m_nameLabel, 1, 0);
  mainLay->addWidget(m_nameEdit, 1, 1);
  mainLay->addWidget(m_passwordLabel, 1, 2);
  mainLay->addWidget(m_passwordEdit, 1, 3);
  mainLay->setMargin(0);

  connect(m_anonymous, SIGNAL(clicked(bool)), SLOT(clicked(bool)));

  retranslateUi();

  bool passwordRequired = NetworkManager::isPasswordRequired();
  m_anonymous->setChecked(!passwordRequired);
  clicked(!passwordRequired);
}


bool OfflineLogin::isAnonymous() const
{
  return m_anonymous->isChecked();
}


QString OfflineLogin::name() const
{
  return m_nameEdit->text();
}


QString OfflineLogin::password() const
{
  return m_passwordEdit->text();
}


void OfflineLogin::retranslateUi()
{
  m_anonymous->setText(tr("Anonymous connection"));
  m_nameLabel->setText(tr("&Name:"));
  m_passwordLabel->setText(tr("&Password:"));
}


void OfflineLogin::clicked(bool checked)
{
  m_nameLabel->setVisible(!checked);
  m_nameEdit->setVisible(!checked);
  m_passwordLabel->setVisible(!checked);
  m_passwordEdit->setVisible(!checked);

  if (!checked)
    m_nameEdit->setFocus();
}
