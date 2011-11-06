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

#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "ui/network/SignUpWidget.h"
#include "NetworkManager.h"
#include "QProgressIndicator/QProgressIndicator.h"

SignUpWidget::SignUpWidget(QWidget *parent)
  : QWidget(parent)
  , m_manager(ChatCore::i()->networks())
  , m_client(ChatCore::i()->client())
{
  m_nameLabel = new QLabel(this);
  m_nameEdit = new QLineEdit(this);

  m_passwordLabel = new QLabel(this);
  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);

  m_progress = new QProgressIndicator(this);
  m_progress->setAnimationDelay(100);
  m_progress->setMaximumSize(16, 16);
  m_progress->startAnimation();

  m_questionLabel = new QLabel(this);
  m_question = new QComboBox(this);
  m_question->addItem(tr("Choose a question ..."));

  m_answerLabel = new QLabel(this);
  m_answerEdit = new QLineEdit(this);

  m_signUp = new QPushButton(SCHAT_ICON(ArrowRightIcon), tr("Sign Up"), this);

  QHBoxLayout *nameLay = new QHBoxLayout;
  nameLay->addWidget(m_nameLabel);
  nameLay->addWidget(m_nameEdit);
  nameLay->addWidget(m_passwordLabel);
  nameLay->addWidget(m_passwordEdit);
  nameLay->setMargin(4);

  QFormLayout *formLay = new QFormLayout;
  formLay->addRow(m_questionLabel, m_question);
  formLay->addRow(m_answerLabel, m_answerEdit);
  formLay->setMargin(4);

  QHBoxLayout *buttonLay = new QHBoxLayout;
  buttonLay->addStretch();
  buttonLay->addWidget(m_progress);
  buttonLay->addWidget(m_signUp);
  buttonLay->setContentsMargins(4, 0, 4, 0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addLayout(nameLay);
  mainLay->addLayout(formLay);
  mainLay->addLayout(buttonLay);
  mainLay->setMargin(0);

  setSmall();
}


/*!
 * Проверка возможности регистрации.
 *
 * \param id Идентификатор сервера.
 * \return true если регистрация возможна.
 */
bool SignUpWidget::canSignUp() const
{
  if (m_client->clientState() != SimpleClient::ClientOnline)
    return false;

  if (m_client->serverId() != m_manager->selected())
    return false;

  NetworkItem item = m_manager->item(m_manager->selected());
  if (item.isAuthorized())
    return false;

  return true;
}


void SignUpWidget::reload()
{
}


void SignUpWidget::retranslateUi()
{
  m_nameLabel->setText(tr("Name:"));
  m_passwordLabel->setText(tr("Password:"));
  m_questionLabel->setText(tr("Security question:"));
  m_answerLabel->setText(tr("Answer:"));
  m_signUp->setText(tr("Sign Up"));

  m_question->setItemText(0, tr("Choose a question ..."));
}


void SignUpWidget::setSmall(bool small)
{
  m_nameLabel->setVisible(!small);
  m_nameEdit->setVisible(!small);
  m_passwordLabel->setVisible(!small);
  m_passwordEdit->setVisible(!small);
  m_progress->setVisible(!small);
  m_questionLabel->setVisible(!small);
  m_question->setVisible(!small);
  m_answerLabel->setVisible(!small);
  m_answerEdit->setVisible(!small);
  m_signUp->setVisible(!small);
  adjustSize();
}
