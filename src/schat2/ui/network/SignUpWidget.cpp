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
#include "messages/MessageAdapter.h"
#include "NetworkManager.h"
#include "QProgressIndicator/QProgressIndicator.h"
#include "ui/network/SignUpWidget.h"

SignUpWidget::SignUpWidget(QWidget *parent)
  : QWidget(parent)
  , m_manager(ChatCore::i()->networks())
  , m_client(ChatCore::i()->client())
  , m_state(Idle)
{
  m_nameLabel = new QLabel(this);
  m_nameEdit = new QLineEdit(this);

  m_passwordLabel = new QLabel(this);
  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);

  m_progress = new QProgressIndicator(this);
  m_progress->setAnimationDelay(100);
  m_progress->setMaximumSize(16, 16);
  m_progress->setVisible(false);

  m_questionLabel = new QLabel(this);
  m_question = new QComboBox(this);
  m_question->addItem(tr("Choose a question ..."));

  m_answerLabel = new QLabel(this);
  m_answerEdit = new QLineEdit(this);

  m_signUp = new QPushButton(SCHAT_ICON(ArrowRightIcon), tr("Sign Up"), this);
  m_signUp->setEnabled(false);

  QHBoxLayout *nameLay = new QHBoxLayout;
  nameLay->addWidget(m_nameLabel);
  nameLay->addWidget(m_nameEdit);
  nameLay->addWidget(m_passwordLabel);
  nameLay->addWidget(m_passwordEdit);
  nameLay->setMargin(0);

  QHBoxLayout *buttonLay = new QHBoxLayout;
  buttonLay->addWidget(m_progress);
  buttonLay->addWidget(m_signUp);
  buttonLay->setMargin(0);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addLayout(nameLay, 0, 0, 1, 3);
  mainLay->addWidget(m_questionLabel, 1, 0);
  mainLay->addWidget(m_question, 1, 1, 1, 2);
  mainLay->addWidget(m_answerLabel, 2, 0);
  mainLay->addWidget(m_answerEdit, 2, 1);
  mainLay->addLayout(buttonLay, 2, 2);
  mainLay->setMargin(4);

  setSmall();

  connect(m_nameEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_passwordEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_answerEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_signUp, SIGNAL(clicked(bool)), SLOT(signUp()));

  retranslateUi();
}


SignUpWidget::~SignUpWidget()
{
  qDebug() << "~SignUpWidget()";
}


/*!
 * Возвращает \b true если виджет готов к регистрации.
 */
bool SignUpWidget::ready() const
{
  if (m_nameEdit->text().isEmpty())
    return false;

  if (m_passwordEdit->text().isEmpty())
    return false;

  if (m_answerEdit->text().isEmpty())
    return false;

  return true;
}


/*!
 * Проверка возможности регистрации.
 *
 * \param id Идентификатор сервера.
 * \return true если регистрация возможна.
 */
bool SignUpWidget::canSignUp()
{
  if (ChatCore::i()->client()->clientState() != SimpleClient::ClientOnline)
    return false;

  if (ChatCore::i()->client()->serverId() != ChatCore::i()->networks()->selected())
    return false;

  NetworkItem item = ChatCore::i()->networks()->item(ChatCore::i()->networks()->selected());
  if (item.isAuthorized())
    return false;

  return true;
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
//  m_progress->setVisible(!small);
  m_questionLabel->setVisible(!small);
  m_question->setVisible(!small);
  m_answerLabel->setVisible(!small);
  m_answerEdit->setVisible(!small);
  m_signUp->setVisible(!small);
  adjustSize();
}


void SignUpWidget::reload()
{
  m_signUp->setEnabled(ready());
}


void SignUpWidget::signUp()
{
  qDebug() << "SIGN UP";

  ChatCore::i()->adapter()->login("reg", m_nameEdit->text(), m_passwordEdit->text());
}


void SignUpWidget::setState(WidgetState state)
{
  m_state = state;
}
