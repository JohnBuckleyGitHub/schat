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

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QToolButton>

#include "ui/ChatIcons.h"
#include "ui/network/Password.h"
#include "ui/network/SecurityQuestion.h"
#include "client/ChatClient.h"
#include "ui/network/NetworkButton.h"

Password::Password(QWidget *parent)
  : QWidget(parent)
{
  m_password = new QRadioButton(tr("Change password"), this);
  m_password->setChecked(true);
  QFont font = m_password->font();
  font.setBold(true);
  m_password->setFont(font);

  m_question = new QRadioButton(tr("Change security question"), this);
  m_question->setFont(font);

  m_passwordWidget = new PasswordWidget(this);
  m_questionWidget = new QuestionWidget(this);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_password);
  mainLay->addWidget(m_passwordWidget);
  mainLay->addWidget(m_question);
  mainLay->addWidget(m_questionWidget);
  mainLay->setMargin(0);

  setMode(PasswordMode);

  connect(m_password, SIGNAL(toggled(bool)), SLOT(toggled()));
}


void Password::toggled()
{
  if (m_password->isChecked())
    setMode(PasswordMode);
  else
    setMode(QuestionMode);
}


void Password::setMode(Mode mode)
{
  m_passwordWidget->setVisible(mode == PasswordMode);
  m_questionWidget->setVisible(mode == QuestionMode);
  m_mode = mode;
}


PasswordBase::PasswordBase(QWidget *parent)
  : QWidget(parent)
{
  m_passwordLabel = new QLabel(tr("Current password:"), this);
  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);

  m_ok = new NetworkButton(tr("OK"), this);

//  m_progress = new QProgressIndicator(this);
//  m_progress->setAnimationDelay(100);
//  m_progress->setMaximumSize(16, 16);
//  m_progress->startAnimation();

  connect(m_passwordEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
}


void PasswordBase::reload()
{
  m_ok->setReady(isReady());
}


bool PasswordBase::isReady() const
{
  if (ChatClient::state() != ChatClient::Online)
    return false;

  if (m_passwordEdit->text().isEmpty())
    return false;

  return true;
}


PasswordWidget::PasswordWidget(QWidget *parent)
  : PasswordBase(parent)
{
  m_newLabel = new QLabel(tr("New password:"), this);
  m_newEdit = new QLineEdit(this);
  m_newEdit->setEchoMode(QLineEdit::Password);

  setTabOrder(m_passwordEdit, m_newEdit);
  setTabOrder(m_newEdit, m_ok);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_passwordLabel, 0, 0);
  mainLay->addWidget(m_passwordEdit, 0, 1, 1, 2);
  mainLay->addWidget(m_newLabel, 1, 0);
  mainLay->addWidget(m_newEdit, 1, 1);
  mainLay->addWidget(m_ok, 1, 2);
  mainLay->setContentsMargins(20, 0, 0, 0);

  connect(m_newEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));

  reload();
}


bool PasswordWidget::isReady() const
{
  if (!PasswordBase::isReady())
    return false;

  if (m_newEdit->text().isEmpty())
    return false;

  return true;
}


QuestionWidget::QuestionWidget(QWidget *parent)
  : PasswordBase(parent)
{
  m_questionLabel = new QLabel(tr("New question:"), this);
  m_question = new SecurityQuestion(this);

  m_answerLabel = new QLabel(tr("New answer:"), this);
  m_answerEdit = new QLineEdit(this);

  setTabOrder(m_passwordEdit, m_question);
  setTabOrder(m_question, m_answerEdit);
  setTabOrder(m_answerEdit, m_ok);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_passwordLabel, 0, 0);
  mainLay->addWidget(m_passwordEdit, 0, 1, 1, 2);
  mainLay->addWidget(m_questionLabel, 1, 0);
  mainLay->addWidget(m_question, 1, 1, 1, 2);
  mainLay->addWidget(m_answerLabel, 2, 0);
  mainLay->addWidget(m_answerEdit, 2, 1);
  mainLay->addWidget(m_ok, 2, 2);
  mainLay->setContentsMargins(20, 0, 0, 0);

  connect(m_answerEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_question, SIGNAL(currentIndexChanged(int)), SLOT(reload()));

  reload();
}


bool QuestionWidget::isReady() const
{
  if (!PasswordBase::isReady())
    return false;

  if (m_answerEdit->text().isEmpty())
    return false;

  if (m_question->currentIndex() < 1)
    return false;

  return true;
}
