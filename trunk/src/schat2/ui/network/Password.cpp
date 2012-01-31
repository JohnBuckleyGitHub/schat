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

Password::Password(QWidget *parent)
  : QWidget(parent)
{
  m_password = new QRadioButton(tr("Change password"), this);
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
}


void Password::setMode(Mode mode)
{
  m_mode = mode;
}


PasswordWidget::PasswordWidget(QWidget *parent)
  : QWidget(parent)
{
  m_passwordLabel = new QLabel(tr("Current password:"), this);
  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);

  m_newLabel = new QLabel(tr("New password:"), this);
  m_newEdit = new QLineEdit(this);
  m_newEdit->setEchoMode(QLineEdit::Password);

  m_ok = new QToolButton(this);
  m_ok->setIcon(SCHAT_ICON(OK));
  m_ok->setText(tr("OK"));
  m_ok->setToolTip(tr("OK"));
  m_ok->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_passwordLabel, 0, 0);
  mainLay->addWidget(m_passwordEdit, 0, 1, 1, 2);
  mainLay->addWidget(m_newLabel, 1, 0);
  mainLay->addWidget(m_newEdit, 1, 1);
  mainLay->addWidget(m_ok, 1, 2);
  mainLay->setContentsMargins(20, 0, 0, 0);
}


QuestionWidget::QuestionWidget(QWidget *parent)
  : QWidget(parent)
{
  m_passwordLabel = new QLabel(tr("Current password:"), this);
  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);

  m_questionLabel = new QLabel(tr("New question:"), this);
  m_question = new SecurityQuestion(this);

  m_answerLabel = new QLabel(tr("New answer:"), this);
  m_answerEdit = new QLineEdit(this);

  m_ok = new QToolButton(this);
  m_ok->setIcon(SCHAT_ICON(OK));
  m_ok->setText(tr("OK"));
  m_ok->setToolTip(tr("OK"));
  m_ok->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_passwordLabel, 0, 0);
  mainLay->addWidget(m_passwordEdit, 0, 1, 1, 2);
  mainLay->addWidget(m_questionLabel, 1, 0);
  mainLay->addWidget(m_question, 1, 1, 1, 2);
  mainLay->addWidget(m_answerLabel, 2, 0);
  mainLay->addWidget(m_answerEdit, 2, 1);
  mainLay->addWidget(m_ok, 2, 2);
  mainLay->setContentsMargins(20, 0, 0, 0);
}
